/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2016 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MassStorage.c
 * Purpose: USB Host Mass Storage example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "cmsis_os.h"
#include "rl_usb.h"

#include "stm32f4xx_hal.h"

#include "USBH_MSC.h"

#ifdef RTE_CMSIS_RTOS_RTX
extern   uint32_t               os_time;
uint32_t HAL_GetTick (void) {
  return os_time;
}
#endif


/*------------------------------------------------------------------------------
 * SystemClock_Config: System Clock Configuration
 *----------------------------------------------------------------------------*/
static void SystemClock_Config (void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}


int main (void) {
  usbStatus usb_status;                 // USB status
  int32_t   msc_status;                 // MSC status
  FILE     *f;                          // Pointer to stream object
  uint8_t   con = 0U;                   // Connection status of MSC(s)

  HAL_Init();                           // Initialize the HAL Library
  SystemClock_Config();                 // Configure the System Clock
  SystemCoreClockUpdate();              // Update system clock

  usb_status = USBH_Initialize (0U);    // Initialize USB Host 0
  if (usb_status != usbOK) {
    for (;;) {}                         // Handle USB Host 0 init failure
  }

  for (;;) {
    msc_status = USBH_MSC_DriveGetMediaStatus ("U0:"); // Get MSC device status
    if (msc_status == USBH_MSC_OK) {
      if (con == 0U) {                  // If stick was not connected previously
        con = 1U;                       // Stick got connected
        msc_status = USBH_MSC_DriveMount ("U0:");
        if (msc_status != USBH_MSC_OK) {
          continue;                     // Handle U0: mount failure
        }
        f = fopen ("Test.txt", "w");    // Open/create file for writing
        if (f == NULL) {
          continue;                     // Handle file opening/creation failure
        }
        fprintf (f, "USB Host Mass Storage!\n");
        fclose  (f);                    // Close file
        msc_status = USBH_MSC_DriveUnmount ("U0:");
        if (msc_status != USBH_MSC_OK) {
          continue;                     // Handle U0: dismount failure
        }
      }
    } else {
      if (con == 1U) {                  // If stick was connected previously
        con = 0U;                       // Stick got disconnected
      }
    }
    osDelay(100U);
  }
}
