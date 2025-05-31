idf_component_register(SRCS "spi_udp_uart.c"
                    PRIV_REQUIRES
                        spi_flash
                        nvs_flash
                    REQUIRES
                        driver
                        esp_event
                        esp_netif
                        esp_wifi
                        lwip
                    INCLUDE_DIRS ".")
