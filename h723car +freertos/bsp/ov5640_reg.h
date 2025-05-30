#ifndef OV5640_REG_H_
#define OV5640_REG_H_

#include <cmsis_compiler.h>

#define  OV5640_ID                                 0x5640U

/**
  * @brief  OV5640 Registers
  */
/* system and IO pad control [0x3000 ~ 0x3052]        */
#define OV5640_SYSREM_RESET00                      0x3000U
#define OV5640_SYSREM_RESET01                      0x3001U
#define OV5640_SYSREM_RESET02                      0x3002U
#define OV5640_SYSREM_RESET03                      0x3003U
#define OV5640_CLOCK_ENABLE00                      0x3004U
#define OV5640_CLOCK_ENABLE01                      0x3005U
#define OV5640_CLOCK_ENABLE02                      0x3006U
#define OV5640_CLOCK_ENABLE03                      0x3007U
#define OV5640_SYSTEM_CTROL0                       0x3008U
#define OV5640_CHIP_ID_HIGH_BYTE                   0x300AU
#define OV5640_CHIP_ID_LOW_BYTE                    0x300BU
#define OV5640_MIPI_CONTROL00                      0x300EU
#define OV5640_PAD_OUTPUT_ENABLE00                 0x3016U
#define OV5640_PAD_OUTPUT_ENABLE01                 0x3017U
#define OV5640_PAD_OUTPUT_ENABLE02                 0x3018U
#define OV5640_PAD_OUTPUT_VALUE00                  0x3019U
#define OV5640_PAD_OUTPUT_VALUE01                  0x301AU
#define OV5640_PAD_OUTPUT_VALUE02                  0x301BU
#define OV5640_PAD_SELECT00                        0x301CU
#define OV5640_PAD_SELECT01                        0x301DU
#define OV5640_PAD_SELECT02                        0x301EU
#define OV5640_CHIP_REVISION                       0x302AU
#define OV5640_PAD_CONTROL00                       0x301CU
#define OV5640_SC_PWC                              0x3031U
#define OV5640_SC_PLL_CONTRL0                      0x3034U
#define OV5640_SC_PLL_CONTRL1                      0x3035U
#define OV5640_SC_PLL_CONTRL2                      0x3036U
#define OV5640_SC_PLL_CONTRL3                      0x3037U
#define OV5640_SC_PLL_CONTRL4                      0x3038U
#define OV5640_SC_PLL_CONTRL5                      0x3039U
#define OV5640_SC_PLLS_CTRL0                       0x303AU
#define OV5640_SC_PLLS_CTRL1                       0x303BU
#define OV5640_SC_PLLS_CTRL2                       0x303CU
#define OV5640_SC_PLLS_CTRL3                       0x303DU
#define OV5640_IO_PAD_VALUE00                      0x3050U
#define OV5640_IO_PAD_VALUE01                      0x3051U
#define OV5640_IO_PAD_VALUE02                      0x3052U

/* SCCB control [0x3100 ~ 0x3108]                       */
#define OV5640_SCCB_ID                             0x3100U
#define OV5640_SCCB_SYSTEM_CTRL0                   0x3102U
#define OV5640_SCCB_SYSTEM_CTRL1                   0x3103U
#define OV5640_SYSTEM_ROOT_DIVIDER                 0x3108U

/* SRB control [0x3200 ~ 0x3213]                        */
#define OV5640_GROUP_ADDR0                         0x3200U
#define OV5640_GROUP_ADDR1                         0x3201U
#define OV5640_GROUP_ADDR2                         0x3202U
#define OV5640_GROUP_ADDR3                         0x3203U
#define OV5640_SRM_GROUP_ACCESS                    0x3212U
#define OV5640_SRM_GROUP_STATUS                    0x3213U

/* AWB gain control [0x3400 ~ 0x3406]                   */
#define OV5640_AWB_R_GAIN_MSB                      0x3400U
#define OV5640_AWB_R_GAIN_LSB                      0x3401U
#define OV5640_AWB_G_GAIN_MSB                      0x3402U
#define OV5640_AWB_G_GAIN_LSB                      0x3403U
#define OV5640_AWB_B_GAIN_MSB                      0x3404U
#define OV5640_AWB_B_GAIN_LSB                      0x3405U
#define OV5640_AWB_MANUAL_CONTROL                  0x3406U

/* AEC/AGC control [0x3500 ~ 0x350D]                    */
#define OV5640_AEC_PK_EXPOSURE_19_16               0x3500U
#define OV5640_AEC_PK_EXPOSURE_HIGH                0x3501U
#define OV5640_AEC_PK_EXPOSURE_LOW                 0x3502U
#define OV5640_AEC_PK_MANUAL                       0x3503U
#define OV5640_AEC_PK_REAL_GAIN_9_8                0x350AU
#define OV5640_AEC_PK_REAL_GAIN_LOW                0x350BU
#define OV5640_AEC_PK_VTS_HIGH                     0x350CU
#define OV5640_AEC_PK_VTS_LOW                      0x350DU

/* VCM control [0x3600 ~ 0x3606]                        */
#define OV5640_VCM_CONTROL_0                       0x3602U
#define OV5640_VCM_CONTROL_1                       0x3603U
#define OV5640_VCM_CONTROL_2                       0x3604U
#define OV5640_VCM_CONTROL_3                       0x3605U
#define OV5640_VCM_CONTROL_4                       0x3606U

/* timing control [0x3800 ~ 0x3821]                    */
#define OV5640_TIMING_HS_HIGH                      0x3800U
#define OV5640_TIMING_HS_LOW                       0x3801U
#define OV5640_TIMING_VS_HIGH                      0x3802U
#define OV5640_TIMING_VS_LOW                       0x3803U
#define OV5640_TIMING_HW_HIGH                      0x3804U
#define OV5640_TIMING_HW_LOW                       0x3805U
#define OV5640_TIMING_VH_HIGH                      0x3806U
#define OV5640_TIMING_VH_LOW                       0x3807U
#define OV5640_TIMING_DVPHO_HIGH                   0x3808U
#define OV5640_TIMING_DVPHO_LOW                    0x3809U
#define OV5640_TIMING_DVPVO_HIGH                   0x380AU
#define OV5640_TIMING_DVPVO_LOW                    0x380BU
#define OV5640_TIMING_HTS_HIGH                     0x380CU
#define OV5640_TIMING_HTS_LOW                      0x380DU
#define OV5640_TIMING_VTS_HIGH                     0x380EU
#define OV5640_TIMING_VTS_LOW                      0x380FU
#define OV5640_TIMING_HOFFSET_HIGH                 0x3810U
#define OV5640_TIMING_HOFFSET_LOW                  0x3811U
#define OV5640_TIMING_VOFFSET_HIGH                 0x3812U
#define OV5640_TIMING_VOFFSET_LOW                  0x3813U
#define OV5640_TIMING_X_INC                        0x3814U
#define OV5640_TIMING_Y_INC                        0x3815U
#define OV5640_HSYNC_START_HIGH                    0x3816U
#define OV5640_HSYNC_START_LOW                     0x3817U
#define OV5640_HSYNC_WIDTH_HIGH                    0x3818U
#define OV5640_HSYNC_WIDTH_LOW                     0x3819U
#define OV5640_TIMING_TC_REG20                     0x3820U
#define OV5640_TIMING_TC_REG21                     0x3821U

/* AEC/AGC power down domain control [0x3A00 ~ 0x3A25] */
#define OV5640_AEC_CTRL00                          0x3A00U
#define OV5640_AEC_CTRL01                          0x3A01U
#define OV5640_AEC_CTRL02                          0x3A02U
#define OV5640_AEC_CTRL03                          0x3A03U
#define OV5640_AEC_CTRL04                          0x3A04U
#define OV5640_AEC_CTRL05                          0x3A05U
#define OV5640_AEC_CTRL06                          0x3A06U
#define OV5640_AEC_CTRL07                          0x3A07U
#define OV5640_AEC_B50_STEP_HIGH                   0x3A08U
#define OV5640_AEC_B50_STEP_LOW                    0x3A09U
#define OV5640_AEC_B60_STEP_HIGH                   0x3A0AU
#define OV5640_AEC_B60_STEP_LOW                    0x3A0BU
#define OV5640_AEC_AEC_CTRL0C                      0x3A0CU
#define OV5640_AEC_CTRL0D                          0x3A0DU
#define OV5640_AEC_CTRL0E                          0x3A0EU
#define OV5640_AEC_CTRL0F                          0x3A0FU
#define OV5640_AEC_CTRL10                          0x3A10U
#define OV5640_AEC_CTRL11                          0x3A11U
#define OV5640_AEC_CTRL13                          0x3A13U
#define OV5640_AEC_MAX_EXPO_HIGH                   0x3A14U
#define OV5640_AEC_MAX_EXPO_LOW                    0x3A15U
#define OV5640_AEC_CTRL17                          0x3A17U
#define OV5640_AEC_GAIN_CEILING_HIGH               0x3A18U
#define OV5640_AEC_GAIN_CEILING_LOW                0x3A19U
#define OV5640_AEC_DIFF_MIN                        0x3A1AU
#define OV5640_AEC_CTRL1B                          0x3A1BU
#define OV5640_LED_ADD_ROW_HIGH                    0x3A1CU
#define OV5640_LED_ADD_ROW_LOW                     0x3A1DU
#define OV5640_AEC_CTRL1E                          0x3A1EU
#define OV5640_AEC_CTRL1F                          0x3A1FU
#define OV5640_AEC_CTRL20                          0x3A20U
#define OV5640_AEC_CTRL21                          0x3A21U
#define OV5640_AEC_CTRL25                          0x3A25U

/* strobe control [0x3B00 ~ 0x3B0C]                      */
#define OV5640_STROBE_CTRL                         0x3B00U
#define OV5640_FREX_EXPOSURE02                     0x3B01U
#define OV5640_FREX_SHUTTER_DLY01                  0x3B02U
#define OV5640_FREX_SHUTTER_DLY00                  0x3B03U
#define OV5640_FREX_EXPOSURE01                     0x3B04U
#define OV5640_FREX_EXPOSURE00                     0x3B05U
#define OV5640_FREX_CTRL07                         0x3B06U
#define OV5640_FREX_MODE                           0x3B07U
#define OV5640_FREX_RQST                           0x3B08U
#define OV5640_FREX_HREF_DLY                       0x3B09U
#define OV5640_FREX_RST_LENGTH                     0x3B0AU
#define OV5640_STROBE_WIDTH_HIGH                   0x3B0BU
#define OV5640_STROBE_WIDTH_LOW                    0x3B0CU

/* 50/60Hz detector control [0x3C00 ~ 0x3C1E]            */
#define OV5640_5060HZ_CTRL00                       0x3C00U
#define OV5640_5060HZ_CTRL01                       0x3C01U
#define OV5640_5060HZ_CTRL02                       0x3C02U
#define OV5640_5060HZ_CTRL03                       0x3C03U
#define OV5640_5060HZ_CTRL04                       0x3C04U
#define OV5640_5060HZ_CTRL05                       0x3C05U
#define OV5640_LIGHTMETER1_TH_HIGH                 0x3C06U
#define OV5640_LIGHTMETER1_TH_LOW                  0x3C07U



#endif


