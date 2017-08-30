/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2016 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Keyboard.c
 * Purpose: USB Host HID Keyboard example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "cmsis_os.h"
#include "rl_usb.h"

#include "stm32f4xx_hal.h"

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
  int       ch;                         // Character
  uint8_t   con = 0U;                   // Connection status of keyboard

  HAL_Init();                           // Initialize the HAL Library
  SystemClock_Config();                 // Configure the System Clock
  SystemCoreClockUpdate();              // Update system clock

  usb_status = USBH_Initialize (0U);    // Initialize USB Host 0
  if (usb_status != usbOK) {
    for (;;) {}                         // Handle USB Host 0 init failure
  }

  for (;;) {
    usb_status = USBH_HID_GetDeviceStatus (0U); // Get HID device status
    if (usb_status == usbOK) {
      if (con == 0U) {                  // If keyboard was not connected previously
        con = 1U;                       // Keyboard got connected
        printf ("Keyboard connected!\n");
      }
    } else {
      if (con == 1U) {                  // If keyboard was connected previously
        con = 0U;                       // Keyboard got disconnected
        printf ("\nKeyboard disconnected!\n");
      }
    }
    if (con != 0U) {                    // If keyboard is active
      ch = USBH_HID_GetKeyboardKey (0U);// Get pressed key
      if (ch != -1) {                   // If valid key value
        if ((ch & 0x10000) != 0) {      // Handle non-ASCII translated keys (Keypad 0 .. 9)
                                        // Bit  16:    non-ASCII bit (0 = ASCII, 1 = not ASCII)
                                        // Bits 15..8: modifiers (SHIFT, ALT, CTRL, GUI)
                                        // Bits  7..0: ASCII or HID key Usage ID if not ASCII
                                        // HID Usage ID values can be found in following link:
                                        // http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
          ch &= 0xFF;                   // Remove non-ASCII bit and modifiers
          if ((ch>=0x59)&&(ch<=0x61)) { // Keypad 1 .. 9 key convert to
            ch = (ch - 0x59) + '1';     // ASCII  1 .. 9
          } else if (ch == 0x62) {      // Keypad 0 key convert to
            ch = '0';                   // ASCII  0
          } else {                      // If not Keypad 0 .. 9
            ch = -1;                    // invalidate the key
          }
        }
        if ((ch > 0) && (ch < 128)) {   // Output ASCII 0 .. 127 range
          putchar(ch);
          fflush(stdout);
        }
      }
    }
    osDelay(10U);
  }
}
