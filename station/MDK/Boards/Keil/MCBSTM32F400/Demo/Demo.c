/*----------------------------------------------------------------------------
 * Name:    Demo.c
 * Purpose: Demo example for MCBSTM32F400
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2014 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "Board_LED.h"
#include "Board_ADC.h"
#include "Board_Buttons.h"
#include "Board_Joystick.h"
#include "Board_Camera.h"
#include "Board_Touch.h"
#include "Board_Accelerometer.h"
#include "Board_GyroScope.h"


/* Ruler position definitions */
#define RUL_2M      ( 8*6+3)
#define RUL_1M      (17*6+3)
#define RUL_0       (26*6+3)
#define RUL_1P      (35*6+3)
#define RUL_2P      (44*6+3)
#define RUL_HGT     (5)
#define RUL_LN(ln) ((ln)*8-2)

/* Bargraph position definitions */
#define BAR_ST      (8*6+3)
#define BAR_LEN     (217)
#define BAR_HGT     (20)
#define BAR_LN(ln)  ((ln)*24)

/* Joystick position definitions */
#define JOY_X       (13*16)
#define JOY_Y       ( 5*24 + 6)

/* Button position definitions */
#define BTN_X       (9)
#define BTN_Y       (190)
#define BTN_X_EVAL  (20)
#define BTN_Y_EVAL  (182)

/* Bulb position definition */
#define BULB_X       (209)
#define BULB_Y       (190)
#define BULB_X_EVAL  (9 * 16)
#define BULB_Y_EVAL  (8 * 24)

/* Keys bit masks */
#define KEY_WAKEUP  4
#define KEY_TAMPER  2
#define KEY_USER    1

/* Mode definitions */
enum {
  M_INIT = 0,
  M_STD,
  M_MOTION,
  M_CAM,
};

/* Periodic timer definition */
void Timer_Callback (void const *arg);
osTimerDef (PeriodicTimer, Timer_Callback);
osTimerId TimerId;

/* Mode switch timeout variable */
uint32_t KeyTick;

/* Extern variables */
extern unsigned char Arrows_16bpp_red[];
extern unsigned char Button_16bpp[];
extern unsigned char Bulb_16bpp[];
extern GLCD_FONT     GLCD_Font_6x8;
extern GLCD_FONT     GLCD_Font_16x24;
#ifdef __RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) {
  return os_time; 
}
#endif

/*-----------------------------------------------------------------------------
  System Clock Configuration
 *----------------------------------------------------------------------------*/
void SystemClock_Config(void) {
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
  RCC_OscInitStruct.PLL.PLLM = 25;
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

/*-----------------------------------------------------------------------------
  Periodic timer callback
 *----------------------------------------------------------------------------*/
void Timer_Callback (void const *arg) {
  static uint32_t ticks;

  if (KeyTick == 0) {
    if (++ticks == 50) {
      KeyTick = 1;
      ticks = 0;
    }
  }
  else ticks = 0;
}


/*-----------------------------------------------------------------------------
  Draw vertical line at specified coordinates (x,y) with specified height (h)
 *----------------------------------------------------------------------------*/
void DrawRuler (uint32_t x, uint32_t y, uint32_t h) {
  uint32_t i;

  for (i = 0; i < h; i++) {
    GLCD_DrawPixel (x, y - i);
  }
}

/*-----------------------------------------------------------------------------
  Switch between modes
 *----------------------------------------------------------------------------*/
uint32_t SwitchMode (uint32_t mode) {

  GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen ();

  switch (mode) {
    case M_INIT:
      /* Prepare display for ADC, Keys, Joystick */
      GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
      GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
      GLCD_SetFont            (&GLCD_Font_16x24);
      GLCD_DrawString (0, 0*24, "    MCBSTM32F400    ");
      GLCD_DrawString (0, 1*24, "    Demo Example    ");
      GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
      GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
      GLCD_DrawString (0, 3*24, "AD value:           ");
      GLCD_DrawString (0, 4*24, "Buttons :           ");
      GLCD_DrawString (0, 6*24, "Joystick:           ");

      mode = M_STD;
      break;

    case M_STD:
      /* Prepare display for accelerator and gyroscope */
      GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
      GLCD_SetBackgroundColor (GLCD_COLOR_NAVY);
      GLCD_ClearScreen ();

      /* Draw rulers */
      DrawRuler (RUL_2M, RUL_LN(6), RUL_HGT);
      DrawRuler (RUL_1M, RUL_LN(6), RUL_HGT);
      DrawRuler (RUL_0,  RUL_LN(6), RUL_HGT);
      DrawRuler (RUL_1P, RUL_LN(6), RUL_HGT);
      DrawRuler (RUL_2P, RUL_LN(6), RUL_HGT);

      DrawRuler (RUL_2M, RUL_LN(21), RUL_HGT);
      DrawRuler (RUL_1M, RUL_LN(21), RUL_HGT);
      DrawRuler (RUL_0,  RUL_LN(21), RUL_HGT);
      DrawRuler (RUL_1P, RUL_LN(21), RUL_HGT);
      DrawRuler (RUL_2P, RUL_LN(21), RUL_HGT);
      
      /* Accelerator graphics */
      GLCD_DrawString (0, 0*24, "  Acceleration [g]  ");
      
      GLCD_SetFont    (&GLCD_Font_6x8);
      GLCD_DrawString ( 7*6,  4*8, "-2");
      GLCD_DrawString (16*6,  4*8, "-1");
      GLCD_DrawString (26*6,  4*8, "0");
      GLCD_DrawString (35*6,  4*8, "1");
      GLCD_DrawString (44*6,  4*8, "2");
      
      GLCD_SetFont    (&GLCD_Font_16x24);
      GLCD_DrawString (0, 2*24, "X:");
      GLCD_DrawString (0, 3*24, "Y:");
      GLCD_DrawString (0, 4*24, "Z:");
      
      /* Gyroscope graphics */
      GLCD_DrawString (0,5*24, " Angular rate [d/s] ");

      GLCD_SetFont    (&GLCD_Font_6x8);      
      GLCD_DrawString ( 6*6, 19*8, "-250");
      GLCD_DrawString (15*6, 19*8, "-125");
      GLCD_DrawString (26*6, 19*8, "0");
      GLCD_DrawString (34*6, 19*8, "125");
      GLCD_DrawString (43*6, 19*8, "250");
      
      GLCD_SetFont    (&GLCD_Font_16x24);
      GLCD_DrawString (0, 7*24, "X:");
      GLCD_DrawString (0, 8*24, "Y:");
      GLCD_DrawString (0, 9*24, "Z:");

      GLCD_SetBackgroundColor (GLCD_COLOR_DARK_GREY);
      mode = M_MOTION;
      break;

    case M_MOTION:
      /* Prepare display for video stream from digital camera */
      GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
      GLCD_ClearScreen ();
      GLCD_FrameBufferAccess (true);
      /* Turn camera on */
      Camera_On ();
      mode = M_CAM;
      break;

    case M_CAM:
      /* Turn camera off */
      Camera_Off ();
      GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
      GLCD_ClearScreen ();
      mode = M_INIT;
      break;

    default:
      mode = M_INIT;
      break;
  }
  return (mode);
}


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
  TOUCH_STATE tsc_state;
  ACCELEROMETER_STATE acc;
  GYROSCOPE_STATE ang;
  uint32_t mode, ofs, addr;
  uint32_t keyMsk, joyMsk, adcVal;
  int32_t key  = -1;
  int32_t joy  = -1;
  int32_t adc  = -1;

  HAL_Init();                               /* Initialize the HAL Library     */

  SystemClock_Config();                     /* Configure the System Clock     */

  TimerId = osTimerCreate (osTimer(PeriodicTimer), osTimerPeriodic, NULL);

  if (TimerId) {
    osTimerStart (TimerId, 10);
  }

  LED_Initialize();                     /* LED Init                           */
  ADC_Initialize();                     /* A/D Converter Init                 */
  Buttons_Initialize();                 /* Buttons Init                       */
  Joystick_Initialize();                /* Joystick Init                      */
  Touch_Initialize();                   /* Touchscreen Controller Init        */
  Accelerometer_Initialize();           /* Accelerometer Init                 */
  Gyroscope_Initialize();               /* Gyroscope Init                     */
  GLCD_Initialize();                    /* Graphical Display Init             */

  addr = GLCD_FrameBufferAddress();     /* Retrieve fremebuffer address       */
  Camera_Initialize(addr);              /* Camera Init                        */

  GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen ();
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetFont            (&GLCD_Font_16x24);

  GLCD_DrawString (0, 0*24, "    MCBSTM32F400    ");
  GLCD_DrawString (0, 1*24, "    Demo Example    ");
  GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
  GLCD_SetForegroundColor (GLCD_COLOR_BLUE);

  GLCD_DrawString (0, 3*24, " Use WAKEUP key to  ");
  GLCD_DrawString (0, 4*24, "   switch example   ");
  GLCD_DrawString (0, 5*24, "        mode        ");

  GLCD_DrawString (0, 8*24, "  Press any key to  ");
  GLCD_DrawString (0, 9*24, "      continue      ");

  /* Wait for user to press the key */
  while (!Buttons_GetState() && !Joystick_GetState());
  mode = SwitchMode (M_INIT);
  KeyTick = 0;

  while (1) {    
    if (KeyTick && (Buttons_GetState() & KEY_WAKEUP)) {
      mode = SwitchMode (mode);
      KeyTick = 0;
      /* Force refresh */
      key = -1;
      joy = -1;
      adc = -1;
    }

    switch (mode) {
      /* Show potentiometer, keys, joystick */
      case M_STD:
        keyMsk = Buttons_GetState();    /* Show buttons state                 */
        if (key ^ keyMsk) {          
          GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
          if (keyMsk & KEY_TAMPER)    { GLCD_DrawString (9*16, 4*24, "TAMPER"); }
          if (keyMsk & KEY_USER  )    { GLCD_DrawString (9*16, 5*24, "USER");   }
          
          GLCD_SetForegroundColor (GLCD_COLOR_LIGHT_GREY);
          if (!(keyMsk & KEY_TAMPER)) { GLCD_DrawString (9*16, 4*24, "TAMPER"); }
          if (!(keyMsk & KEY_USER  )) { GLCD_DrawString (9*16, 5*24, "USER");   }
        }

        joyMsk = Joystick_GetState(); /* Show joystick arrows               */
        if (joy ^ joyMsk) {
          joy = joyMsk;

          /* Left arrow white/red? */
          ofs = (joy & JOYSTICK_DOWN)   ? (2416) : (0);
          GLCD_DrawBitmap(JOY_X+ 0, JOY_Y+20, 19, 14, &Arrows_16bpp_red[ofs + 0*532]);

          /* Right arrow white/red? */
          ofs = (joy & JOYSTICK_UP)     ? (2416) : (0);
          GLCD_DrawBitmap(JOY_X+35, JOY_Y+20, 19, 14, &Arrows_16bpp_red[ofs + 1*532]);

          /* Center dot white/red */
          ofs = (joy & JOYSTICK_CENTER) ? (2416) : (0);
          GLCD_DrawBitmap(JOY_X+21, JOY_Y+21, 12, 12, &Arrows_16bpp_red[ofs + 4*532]);

          /* Up arrow white/red? */
          ofs = (joy & JOYSTICK_LEFT)   ? (2416) : (0);
          GLCD_DrawBitmap(JOY_X+20, JOY_Y+ 0, 14, 19, &Arrows_16bpp_red[ofs + 2*532]);

          /* Down arrow white/red? */
          ofs = (joy & JOYSTICK_RIGHT)  ? (2416) : (0);
          GLCD_DrawBitmap(JOY_X+20, JOY_Y+35, 14, 19, &Arrows_16bpp_red[ofs + 3*532]);
        }

        ADC_StartConversion();          /* Show A/D conversion bargraph       */
        adcVal = ADC_GetValue();
        if (adc ^ adcVal) {
          GLCD_SetForegroundColor (GLCD_COLOR_GREEN);
          GLCD_DrawBargraph (144, 3*24, 160, 20, (adcVal * 100) / ((1 << ADC_GetResolution()) - 1));
        }
        
        Touch_GetState (&tsc_state);    /* Get touch screen state             */

        if (!tsc_state.pressed) {
          tsc_state.x = 0;
          tsc_state.y = 0;
        }

        if (tsc_state.x >= 200 && tsc_state.x <= 1000 &&
            tsc_state.y >= 260 && tsc_state.y <= 1650) {
          GLCD_DrawBitmap(BTN_X,  BTN_Y,  120, 48, &Button_16bpp[1*11520]);
          GLCD_DrawBitmap(BULB_X, BULB_Y,  26, 40, &Bulb_16bpp[1*2080]);
        }
        else {
          GLCD_DrawBitmap(BTN_X,  BTN_Y,  120, 48, &Button_16bpp[0*11520]);
          GLCD_DrawBitmap(BULB_X, BULB_Y,  26, 40, &Bulb_16bpp[0*2080]);
        }
        break;

      /* Show accelerator and gyroscope outputs */
      case M_MOTION:
        if (Accelerometer_GetState (&acc)) {    /* Get acceleration data      */
          acc.x = 0; acc.y = 0; acc.z = 0;
        }
        GLCD_SetForegroundColor (GLCD_COLOR_RED);
        GLCD_DrawBargraph (BAR_ST, BAR_LN(2), BAR_LEN, BAR_HGT, 50 + (acc.x * 25/1000));
        GLCD_DrawBargraph (BAR_ST, BAR_LN(3), BAR_LEN, BAR_HGT, 50 + (acc.y * 25/1000));
        GLCD_DrawBargraph (BAR_ST, BAR_LN(4), BAR_LEN, BAR_HGT, 50 + (acc.z * 25/1000));

        if (Gyroscope_GetState (&ang)) {        /* Get angular rate data      */
          ang.x = 0; ang.y = 0; ang.z = 0;
        }
        GLCD_SetForegroundColor (GLCD_COLOR_YELLOW);
        GLCD_DrawBargraph (BAR_ST, BAR_LN(7), BAR_LEN, BAR_HGT, 50 + (ang.x * 25/1250));
        GLCD_DrawBargraph (BAR_ST, BAR_LN(8), BAR_LEN, BAR_HGT, 50 + (ang.y * 25/1250));
        GLCD_DrawBargraph (BAR_ST, BAR_LN(9), BAR_LEN, BAR_HGT, 50 + (ang.z * 25/1250));
        break;

      /* Show digital camera output on graphical LCD */
      case M_CAM:
        break;

      default:
        mode = SwitchMode (mode);
        break;
    }
  }
}
