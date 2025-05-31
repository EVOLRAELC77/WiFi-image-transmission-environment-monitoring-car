#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "driver/uart.h"  // 新增：串口驱动

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  配置参数区                  /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef CONFIG_IDF_TARGET_ESP32
#define RCV_HOST    HSPI_HOST
#else
#define RCV_HOST    SPI2_HOST
#endif

// SPI 引脚配置
#define GPIO_HANDSHAKE      2
#define GPIO_MOSI           12
#define GPIO_MISO           13
#define GPIO_SCLK           15
#define GPIO_CS             14

// WiFi 配置
#define ESP_WIFI_SSID       "LAPTOP"
#define ESP_WIFI_PASS       "cfx2004703"
#define ESP_STATIC_IP       "192.168.137.100"
#define ESP_STATIC_GW       "192.168.137.1"
#define ESP_STATIC_NETMASK  "255.255.255.0"

// UDP 配置（新增40000端口用于双向通信）
#define UDP_SERVER_IP       "192.168.137.1"
#define UDP_SERVER_PORT     30000      // 服务器端口
#define UDP_COMMAND_PORT    40000      // 控制指令接收端口
#define UDP_JPEG_PORT       20000      // jpeg数据流端口}
#define UDP_AHT20_PORT      50000      // AHT温湿度发送端口

// 串口配置（STM32连接引脚：ESP32 TX=17, RX=16）
#define UART_NUM            UART_NUM_1
#define UART_TX_PIN         17
#define UART_RX_PIN         16
#define UART_BAUD_RATE      115200     // 波特率
#define UART_BUF_SIZE       1024       // 接收缓冲区大小

// 事件与信号量
#define WIFI_CONNECTED_BIT      BIT0
#define WIFI_FAIL_BIT           BIT1
#define DATA_READY_BIT          BIT2
EventGroupHandle_t s_wifiEventGroup;
EventGroupHandle_t s_sendEventGroup;
SemaphoreHandle_t s_recvMutex;
SemaphoreHandle_t s_uartMutex;      // 新增：串口互斥锁

// 全局缓冲区
char *sendbuf;
char *recvbuf;
uint16_t recv_cnt = 0;
const char *TAG = "MAIN";

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  串口初始化函数             /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart_init(void) {
    uart_config_t uart_config = 
    {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    // 初始化UART驱动
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUF_SIZE, 0, 0, NULL, 0));
    
    // 创建串口互斥锁
    s_uartMutex = xSemaphoreCreateMutex();
    
    ESP_LOGI(TAG, "UART初始化完成，波特率：%d", UART_BAUD_RATE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  UDP接收任务（服务器→STM32） /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void udp_recv_task(void *pvParameter) {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) 
    {
        ESP_LOGE(TAG, "创建UDP接收套接字失败");
        vTaskDelete(NULL);
        return;
    }

    // 绑定到40000端口接收服务器控制指令
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(UDP_COMMAND_PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) 
    {
        ESP_LOGE(TAG, "绑定UDP接收端口失败");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "UDP接收任务启动，监听端口%d", UDP_COMMAND_PORT);

    char recv_buffer[UART_BUF_SIZE];
    memset(recv_buffer,0,UART_BUF_SIZE);
    struct sockaddr_in src_addr;
    socklen_t src_len = sizeof(src_addr);

    while (1) 
    {
        // 接收UDP数据（阻塞模式）
        int recv_len = recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0, 
                              (struct sockaddr *)&src_addr, &src_len);
                              
        if (recv_len > 0) 
        {
            ESP_LOGI(TAG, "接收到%d字节UDP数据，来自：%s:%d", 
                    recv_len, inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port));
            
            // 通过串口转发给STM32
            if (xSemaphoreTake(s_uartMutex, portMAX_DELAY) == pdPASS) 
            {
                uart_write_bytes(UART_NUM, recv_buffer, recv_len);
                xSemaphoreGive(s_uartMutex);
                ESP_LOGI(TAG, "已通过串口转发%s数据给STM32", recv_buffer);
                ESP_LOGI(TAG, "已通过串口转发%d字节数据给STM32", recv_len);
            }
        }
    }

    close(sock);
    vTaskDelete(NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  串口接收任务（STM32→服务器） /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void uart_recv_task(void *pvParameter) {
    // 创建UDP发送套接字（使用50000端口）
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) 
    {
        ESP_LOGE(TAG, "创建UDP发送套接字失败");
        vTaskDelete(NULL);
        return;
    }

        // 配置本地固定端口,发送AHT20数据
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(UDP_AHT20_PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) 
    {
        ESP_LOGE(TAG, "绑定本地端口失败: %d", UDP_AHT20_PORT);
        close(sock);
        vTaskDelete(NULL);
        return;
    }
    
    ESP_LOGI(TAG, "UDP本地端口绑定成功: %d", UDP_AHT20_PORT);

    // 配置服务器地址（30000端口）
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);
    socklen_t server_len = sizeof(server_addr);

    ESP_LOGI(TAG, "串口接收任务启动，等待STM32数据");

    char uart_buffer[UART_BUF_SIZE];
    int uart_len;

    while (1) 
    {
        // 等待串口数据（超时设置为100ms）
        uart_len = uart_read_bytes(UART_NUM, uart_buffer, UART_BUF_SIZE, 100 / portTICK_PERIOD_MS);
        
        if (uart_len > 0) 
        {
            ESP_LOGI(TAG, "从STM32接收到%d字节数据", uart_len);
            
            // 通过UDP发送给服务器
            int result = sendto(sock, uart_buffer, uart_len, 0, 
                               (struct sockaddr*)&server_addr, server_len);
                               
            if (result < 0) 
            {
                ESP_LOGE(TAG, "UDP发送失败，错误码：%d", errno);
            } else 
            {
                ESP_LOGI(TAG, "已通过UDP发送%d字节数据给服务器", result);
            }
        }
    }

    close(sock);
    vTaskDelete(NULL);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  SPI 相关函数               /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SPI 事务回调函数
void my_post_setup_cb(spi_slave_transaction_t *trans)
{
    gpio_set_level(GPIO_HANDSHAKE, 1);
}

void my_post_trans_cb(spi_slave_transaction_t *trans)
{
    gpio_set_level(GPIO_HANDSHAKE, 0);
}

// SPI 初始化
void spi_init(void)
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_slave_interface_config_t slvcfg = {
        .mode = 1,
        .spics_io_num = GPIO_CS,
        .queue_size = 8,
        .flags = 0,
        .post_setup_cb = my_post_setup_cb,
        .post_trans_cb = my_post_trans_cb
    };

    // 配置握手引脚
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(GPIO_HANDSHAKE),
    };
    gpio_config(&io_conf);
    gpio_set_level(GPIO_HANDSHAKE, 0);

    // 使能上拉电阻
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    // 初始化SPI从机
    ret = spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);

    // 分配DMA内存
    sendbuf = spi_bus_dma_memory_alloc(RCV_HOST, 4000, 0);
    recvbuf = spi_bus_dma_memory_alloc(RCV_HOST, 40000, 0);
    assert(sendbuf && recvbuf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  WiFi 相关函数               /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 设置静态IP地址
static void set_static_ip(esp_netif_t *esp_netif)
{
    esp_netif_ip_info_t ip_info;
    
    // 转换IP地址字符串为二进制格式
    ESP_ERROR_CHECK(esp_netif_dhcpc_stop(esp_netif));
    
    memset(&ip_info, 0, sizeof(ip_info));
    inet_pton(AF_INET, ESP_STATIC_IP, &ip_info.ip);
    inet_pton(AF_INET, ESP_STATIC_GW, &ip_info.gw);
    inet_pton(AF_INET, ESP_STATIC_NETMASK, &ip_info.netmask);
    
    ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif, &ip_info));
    
    ESP_LOGI(TAG, "静态IP设置成功: %s", ESP_STATIC_IP);
    ESP_LOGI(TAG, "网关: %s", ESP_STATIC_GW);
    ESP_LOGI(TAG, "子网掩码: %s", ESP_STATIC_NETMASK);
}

// WiFi 事件回调
void event_CallBack(void* arg, esp_event_base_t event_base, 
                    int32_t event_id, void* event_data)
{
    static uint8_t connect_try_count = 0;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        if (connect_try_count < 5) 
        {
            esp_wifi_connect();
            connect_try_count++;
        } else 
        {
            xEventGroupSetBits(s_wifiEventGroup, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event_Ip_Msg = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "已成功连接到AP\n获取到的的IP地址为:"IPSTR"", IP2STR(&event_Ip_Msg->ip_info.ip));
        ESP_LOGI(TAG, "网关的IP地址为:"IPSTR"", IP2STR(&event_Ip_Msg->ip_info.gw));
        xEventGroupSetBits(s_wifiEventGroup, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifiEventGroup = xEventGroupCreate();
    s_sendEventGroup = xEventGroupCreate();
    s_recvMutex = xSemaphoreCreateMutex();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // 创建默认WiFi STA接口并设置静态IP
    esp_netif_create_default_wifi_sta();
    esp_netif_t *sta_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    set_static_ip(sta_netif);

    // 注册事件回调
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                       &event_CallBack, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                       &event_CallBack, NULL, &instance_got_ip));

    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = 
    {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  SPI 接收任务               /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void spi_receive_task(void *pvParameter)
{
    spi_slave_transaction_t trans = {0};
    trans.length = 4000 * 8;
    trans.tx_buffer = sendbuf;
    trans.rx_buffer = recvbuf;

    while (1) 
    {
        // 重置缓冲区
        if (xSemaphoreTake(s_recvMutex, portMAX_DELAY) == pdPASS) 
        {
            memset(recvbuf, 0, 40000);
            xSemaphoreGive(s_recvMutex);
        }

        // 接收JPEG数据块
        uint8_t *jpeg_buf_temp = (uint8_t *)recvbuf;
        uint jpeg_start_index = UINT32_MAX, jpeg_end_index = UINT32_MAX;

        // 首先接收头部直到找到SOI标记
        while (1) 
        {
            trans.rx_buffer = recvbuf;
            spi_slave_transmit(RCV_HOST, &trans, portMAX_DELAY);
            ESP_LOGI(TAG, "接收头部块：%02X %02X", recvbuf[0], recvbuf[1]);
            
            // 检查SOI标记
            if (recvbuf[0] == 0xFF && recvbuf[1] == 0xD8) 
            {
                jpeg_start_index = 0;
                break;
            }
        }

        // 接收后续数据块
        for (uint8_t i = 1; i < 10; i++) 
        {
            trans.rx_buffer = recvbuf + 4000 * i;
            spi_slave_transmit(RCV_HOST, &trans, portMAX_DELAY);
        }
        

        // 解析完整JPEG数据
        for (uint jpeg_index = jpeg_start_index; jpeg_index < 40000 - 2; jpeg_index++) 
        {
            if (jpeg_buf_temp[jpeg_index] == 0xFF && jpeg_buf_temp[jpeg_index + 1] == 0xD9) 
            {
                jpeg_end_index = jpeg_index;
                break;
            }
        }

        // 处理有效数据
        if (jpeg_end_index != UINT32_MAX) 
        {
            recv_cnt = jpeg_end_index - jpeg_start_index + 2;
            ESP_LOGI(TAG, "SPI接收完成：JPEG长度=%d", recv_cnt);
            
            // 通知UDP任务发送数据
            if (xSemaphoreTake(s_recvMutex, portMAX_DELAY) == pdPASS) 
            {
                xEventGroupSetBits(s_sendEventGroup, DATA_READY_BIT);
                xSemaphoreGive(s_recvMutex);
            }
        } else
        {
            ESP_LOGW(TAG, "无效的JPEG数据，未找到完整标记");
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  UDP 发送任务               /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void udp_send_task(void *pvParameters)
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) 
    {
        ESP_LOGE(TAG, "创建UDP套接字失败");
        vTaskDelete(NULL);
        return;
    }

    // 配置本地固定端口
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(UDP_JPEG_PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) 
    {
        ESP_LOGE(TAG, "绑定本地端口失败: %d", UDP_JPEG_PORT);
        close(sock);
        vTaskDelete(NULL);
        return;
    }
    
    ESP_LOGI(TAG, "UDP本地端口绑定成功: %d", UDP_JPEG_PORT);

    // 配置服务器地址
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);
    socklen_t server_len = sizeof(server_addr);

    ESP_LOGI(TAG, "UDP套接字创建成功，等待数据发送");

    while (1) 
    {
        // 等待数据就绪事件
        xEventGroupWaitBits(s_sendEventGroup, DATA_READY_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
        
        // 加锁保护数据缓冲区
        if (xSemaphoreTake(s_recvMutex, portMAX_DELAY) == pdPASS) 
        {
            if (recv_cnt > 0) 
            {
                ESP_LOGI(TAG, "开始发送数据，总长度：%d字节", recv_cnt);

                int result = sendto(sock, recvbuf, recv_cnt, 0,
                                    (struct sockaddr*)&server_addr, server_len);
                                    
                if (result < 0) 
                {
                    ESP_LOGE(TAG, "数据发送失败，错误码：%d", errno);
                    break;
                }
                ESP_LOGI(TAG, "已发送%d字节", result);
            }
            xSemaphoreGive(s_recvMutex);
            ESP_LOGI(TAG, "数据发送完成");
        }
    }

    close(sock);
    vTaskDelete(NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                  主函数入口               /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void app_main(void) 
{
    ESP_ERROR_CHECK(nvs_flash_init());
    
    // 初始化SPI
    spi_init();
    
    // 初始化WiFi
    wifi_init_sta();
    
    // 初始化串口
    uart_init();
    
    // 等待WiFi连接
    EventBits_t wifi_bits = xEventGroupWaitBits(s_wifiEventGroup, 
                                                WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                                pdTRUE, pdFALSE, portMAX_DELAY);
                                                
    if (wifi_bits & WIFI_FAIL_BIT) 
    {
        ESP_LOGE(TAG, "WiFi连接失败");
        return;
    }
    
    // 创建所有任务（优先级调整）
    xTaskCreate(spi_receive_task, "spi_task",   8192, NULL, 4, NULL);  // SPI数据采集
    xTaskCreate(udp_send_task,    "udp_task",   8192, NULL, 5, NULL);  // SPI数据上传
    xTaskCreate(udp_recv_task,    "udp_recv",   4096, NULL, 6, NULL);  // UDP接收（服务器→STM32）
    xTaskCreate(uart_recv_task,   "uart_recv",  4096, NULL, 6, NULL);  // 串口接收（STM32→服务器）
}