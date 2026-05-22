/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G351X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G351X
#define CONFIG_MSPM0G3519

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_0 */
#define PWM_0_INST                                                        TIMG14
#define PWM_0_INST_IRQHandler                                  TIMG14_IRQHandler
#define PWM_0_INST_INT_IRQN                                    (TIMG14_INT_IRQn)
#define PWM_0_INST_CLK_FREQ                                              1000000
/* GPIO defines for channel 3 */
#define GPIO_PWM_0_C3_PORT                                                 GPIOB
#define GPIO_PWM_0_C3_PIN                                          DL_GPIO_PIN_1
#define GPIO_PWM_0_C3_IOMUX                                      (IOMUX_PINCM13)
#define GPIO_PWM_0_C3_IOMUX_FUNC                    IOMUX_PINCM13_PF_TIMG14_CCP3
#define GPIO_PWM_0_C3_IDX                                    DL_TIMER_CC_3_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMA0)
#define TIMER_0_INST_IRQHandler                                 TIMA0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                          (1999U)
/* Defines for TIMER_1 */
#define TIMER_1_INST                                                     (TIMA1)
#define TIMER_1_INST_IRQHandler                                 TIMA1_IRQHandler
#define TIMER_1_INST_INT_IRQN                                   (TIMA1_INT_IRQn)
#define TIMER_1_INST_LOAD_VALUE                                          (3199U)
/* Defines for TIMER_2 */
#define TIMER_2_INST                                                     (TIMG0)
#define TIMER_2_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_2_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_2_INST_LOAD_VALUE                                         (63999U)
/* Defines for TIMER_3 */
#define TIMER_3_INST                                                     (TIMG6)
#define TIMER_3_INST_IRQHandler                                 TIMG6_IRQHandler
#define TIMER_3_INST_INT_IRQN                                   (TIMG6_INT_IRQn)
#define TIMER_3_INST_LOAD_VALUE                                          (1999U)



/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_FREQUENCY                                           32000000
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_32_MHZ_115200_BAUD                                      (17)
#define UART_0_FBRD_32_MHZ_115200_BAUD                                      (23)




/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI0
#define SPI_0_INST_IRQHandler                                   SPI0_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI0_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOB
#define GPIO_SPI_0_PICO_PIN                                       DL_GPIO_PIN_2
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM15)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM15_PF_SPI0_PICO
#define GPIO_SPI_0_POCI_PORT                                              GPIOA
#define GPIO_SPI_0_POCI_PIN                                       DL_GPIO_PIN_4
#define GPIO_SPI_0_IOMUX_POCI                                    (IOMUX_PINCM9)
#define GPIO_SPI_0_IOMUX_POCI_FUNC                    IOMUX_PINCM9_PF_SPI0_POCI
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOB
#define GPIO_SPI_0_SCLK_PIN                                       DL_GPIO_PIN_3
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM16)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM16_PF_SPI0_SCLK



/* Defines for ADC12_0 */
#define ADC12_0_INST                                                        ADC0
#define ADC12_0_INST_IRQHandler                                  ADC0_IRQHandler
#define ADC12_0_INST_INT_IRQN                                    (ADC0_INT_IRQn)
#define ADC12_0_ADCMEM_0                                      DL_ADC12_MEM_IDX_0
#define ADC12_0_ADCMEM_0_REF                DL_ADC12_REFERENCE_VOLTAGE_VDDA_VSSA
#define GPIO_ADC12_0_C2_PORT                                               GPIOA
#define GPIO_ADC12_0_C2_PIN                                       DL_GPIO_PIN_25
#define GPIO_ADC12_0_IOMUX_C2                                    (IOMUX_PINCM55)
#define GPIO_ADC12_0_IOMUX_C2_FUNC                (IOMUX_PINCM55_PF_UNCONNECTED)



/* Defines for LED3: GPIOB.26 with pinCMx 57 on package pin 76 */
#define LED_LED3_PORT                                                    (GPIOB)
#define LED_LED3_PIN                                            (DL_GPIO_PIN_26)
#define LED_LED3_IOMUX                                           (IOMUX_PINCM57)
/* Defines for LED4: GPIOB.28 with pinCMx 65 on package pin 24 */
#define LED_LED4_PORT                                                    (GPIOB)
#define LED_LED4_PIN                                            (DL_GPIO_PIN_28)
#define LED_LED4_IOMUX                                           (IOMUX_PINCM65)
/* Defines for LED5: GPIOB.29 with pinCMx 66 on package pin 25 */
#define LED_LED5_PORT                                                    (GPIOB)
#define LED_LED5_PIN                                            (DL_GPIO_PIN_29)
#define LED_LED5_IOMUX                                           (IOMUX_PINCM66)
/* Defines for LED6: GPIOB.30 with pinCMx 67 on package pin 26 */
#define LED_LED6_PORT                                                    (GPIOB)
#define LED_LED6_PIN                                            (DL_GPIO_PIN_30)
#define LED_LED6_IOMUX                                           (IOMUX_PINCM67)
/* Defines for LED2: GPIOA.17 with pinCMx 39 on package pin 54 */
#define LED_LED2_PORT                                                    (GPIOA)
#define LED_LED2_PIN                                            (DL_GPIO_PIN_17)
#define LED_LED2_IOMUX                                           (IOMUX_PINCM39)
/* Defines for LED1: GPIOA.14 with pinCMx 36 on package pin 43 */
#define LED_LED1_PORT                                                    (GPIOA)
#define LED_LED1_PIN                                            (DL_GPIO_PIN_14)
#define LED_LED1_IOMUX                                           (IOMUX_PINCM36)
/* Defines for LED7: GPIOC.6 with pinCMx 84 on package pin 63 */
#define LED_LED7_PORT                                                    (GPIOC)
#define LED_LED7_PIN                                             (DL_GPIO_PIN_6)
#define LED_LED7_IOMUX                                           (IOMUX_PINCM84)
/* Defines for LED8: GPIOC.7 with pinCMx 85 on package pin 64 */
#define LED_LED8_PORT                                                    (GPIOC)
#define LED_LED8_PIN                                             (DL_GPIO_PIN_7)
#define LED_LED8_IOMUX                                           (IOMUX_PINCM85)
/* Defines for SW1: GPIOA.7 with pinCMx 14 on package pin 17 */
#define SW_SW1_PORT                                                      (GPIOA)
// pins affected by this interrupt request:["SW1","SW2","SW3","SW4","SW5","SW6","SW7"]
#define SW_GPIOA_INT_IRQN                                       (GPIOA_INT_IRQn)
#define SW_GPIOA_INT_IIDX                       (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define SW_SW1_IIDX                                          (DL_GPIO_IIDX_DIO7)
#define SW_SW1_PIN                                               (DL_GPIO_PIN_7)
#define SW_SW1_IOMUX                                             (IOMUX_PINCM14)
/* Defines for SW2: GPIOA.8 with pinCMx 19 on package pin 22 */
#define SW_SW2_PORT                                                      (GPIOA)
#define SW_SW2_IIDX                                          (DL_GPIO_IIDX_DIO8)
#define SW_SW2_PIN                                               (DL_GPIO_PIN_8)
#define SW_SW2_IOMUX                                             (IOMUX_PINCM19)
/* Defines for SW3: GPIOA.9 with pinCMx 20 on package pin 23 */
#define SW_SW3_PORT                                                      (GPIOA)
#define SW_SW3_IIDX                                          (DL_GPIO_IIDX_DIO9)
#define SW_SW3_PIN                                               (DL_GPIO_PIN_9)
#define SW_SW3_IOMUX                                             (IOMUX_PINCM20)
/* Defines for SW4: GPIOA.12 with pinCMx 34 on package pin 41 */
#define SW_SW4_PORT                                                      (GPIOA)
#define SW_SW4_IIDX                                         (DL_GPIO_IIDX_DIO12)
#define SW_SW4_PIN                                              (DL_GPIO_PIN_12)
#define SW_SW4_IOMUX                                             (IOMUX_PINCM34)
/* Defines for SW5: GPIOA.13 with pinCMx 35 on package pin 42 */
#define SW_SW5_PORT                                                      (GPIOA)
#define SW_SW5_IIDX                                         (DL_GPIO_IIDX_DIO13)
#define SW_SW5_PIN                                              (DL_GPIO_PIN_13)
#define SW_SW5_IOMUX                                             (IOMUX_PINCM35)
/* Defines for SW6: GPIOA.16 with pinCMx 38 on package pin 45 */
#define SW_SW6_PORT                                                      (GPIOA)
#define SW_SW6_IIDX                                         (DL_GPIO_IIDX_DIO16)
#define SW_SW6_PIN                                              (DL_GPIO_PIN_16)
#define SW_SW6_IOMUX                                             (IOMUX_PINCM38)
/* Defines for SW7: GPIOA.22 with pinCMx 47 on package pin 62 */
#define SW_SW7_PORT                                                      (GPIOA)
#define SW_SW7_IIDX                                         (DL_GPIO_IIDX_DIO22)
#define SW_SW7_PIN                                              (DL_GPIO_PIN_22)
#define SW_SW7_IOMUX                                             (IOMUX_PINCM47)
/* Defines for SW8: GPIOB.22 with pinCMx 50 on package pin 69 */
#define SW_SW8_PORT                                                      (GPIOB)
// groups represented: ["Keybord_in","SW"]
// pins affected: ["PIN_V1","PIN_V2","PIN_V3","PIN_V4","SW8"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define SW_SW8_IIDX                                         (DL_GPIO_IIDX_DIO22)
#define SW_SW8_PIN                                              (DL_GPIO_PIN_22)
#define SW_SW8_IOMUX                                             (IOMUX_PINCM50)
/* Defines for OLED_DC: GPIOC.8 with pinCMx 86 on package pin 65 */
#define OLED_OLED_DC_PORT                                                (GPIOC)
#define OLED_OLED_DC_PIN                                         (DL_GPIO_PIN_8)
#define OLED_OLED_DC_IOMUX                                       (IOMUX_PINCM86)
/* Defines for OLED_CS: GPIOC.9 with pinCMx 87 on package pin 66 */
#define OLED_OLED_CS_PORT                                                (GPIOC)
#define OLED_OLED_CS_PIN                                         (DL_GPIO_PIN_9)
#define OLED_OLED_CS_IOMUX                                       (IOMUX_PINCM87)
/* Defines for OLED_RES: GPIOB.23 with pinCMx 51 on package pin 70 */
#define OLED_OLED_RES_PORT                                               (GPIOB)
#define OLED_OLED_RES_PIN                                       (DL_GPIO_PIN_23)
#define OLED_OLED_RES_IOMUX                                      (IOMUX_PINCM51)
/* Port definition for Pin Group Keybord_out */
#define Keybord_out_PORT                                                 (GPIOB)

/* Defines for PIN_H1: GPIOB.6 with pinCMx 23 on package pin 30 */
#define Keybord_out_PIN_H1_PIN                                   (DL_GPIO_PIN_6)
#define Keybord_out_PIN_H1_IOMUX                                 (IOMUX_PINCM23)
/* Defines for PIN_H2: GPIOB.7 with pinCMx 24 on package pin 31 */
#define Keybord_out_PIN_H2_PIN                                   (DL_GPIO_PIN_7)
#define Keybord_out_PIN_H2_IOMUX                                 (IOMUX_PINCM24)
/* Defines for PIN_H3: GPIOB.8 with pinCMx 25 on package pin 32 */
#define Keybord_out_PIN_H3_PIN                                   (DL_GPIO_PIN_8)
#define Keybord_out_PIN_H3_IOMUX                                 (IOMUX_PINCM25)
/* Defines for PIN_H4: GPIOB.9 with pinCMx 26 on package pin 33 */
#define Keybord_out_PIN_H4_PIN                                   (DL_GPIO_PIN_9)
#define Keybord_out_PIN_H4_IOMUX                                 (IOMUX_PINCM26)
/* Port definition for Pin Group Keybord_in */
#define Keybord_in_PORT                                                  (GPIOB)

/* Defines for PIN_V1: GPIOB.20 with pinCMx 48 on package pin 67 */
#define Keybord_in_PIN_V1_IIDX                              (DL_GPIO_IIDX_DIO20)
#define Keybord_in_PIN_V1_PIN                                   (DL_GPIO_PIN_20)
#define Keybord_in_PIN_V1_IOMUX                                  (IOMUX_PINCM48)
/* Defines for PIN_V2: GPIOB.24 with pinCMx 52 on package pin 71 */
#define Keybord_in_PIN_V2_IIDX                              (DL_GPIO_IIDX_DIO24)
#define Keybord_in_PIN_V2_PIN                                   (DL_GPIO_PIN_24)
#define Keybord_in_PIN_V2_IOMUX                                  (IOMUX_PINCM52)
/* Defines for PIN_V3: GPIOB.25 with pinCMx 56 on package pin 75 */
#define Keybord_in_PIN_V3_IIDX                              (DL_GPIO_IIDX_DIO25)
#define Keybord_in_PIN_V3_PIN                                   (DL_GPIO_PIN_25)
#define Keybord_in_PIN_V3_IOMUX                                  (IOMUX_PINCM56)
/* Defines for PIN_V4: GPIOB.27 with pinCMx 58 on package pin 77 */
#define Keybord_in_PIN_V4_IIDX                              (DL_GPIO_IIDX_DIO27)
#define Keybord_in_PIN_V4_PIN                                   (DL_GPIO_PIN_27)
#define Keybord_in_PIN_V4_IOMUX                                  (IOMUX_PINCM58)



/* Defines for DAC12 */
#define DAC12_IRQHandler                                         DAC0_IRQHandler
#define DAC12_INT_IRQN                                           (DAC0_INT_IRQn)
#define GPIO_DAC12_OUT_PORT                                                GPIOA
#define GPIO_DAC12_OUT_PIN                                        DL_GPIO_PIN_15
#define GPIO_DAC12_IOMUX_OUT                                     (IOMUX_PINCM37)
#define GPIO_DAC12_IOMUX_OUT_FUNC                   IOMUX_PINCM37_PF_UNCONNECTED


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_0_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_TIMER_1_init(void);
void SYSCFG_DL_TIMER_2_init(void);
void SYSCFG_DL_TIMER_3_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_SPI_0_init(void);
void SYSCFG_DL_ADC12_0_init(void);

void SYSCFG_DL_DAC12_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
