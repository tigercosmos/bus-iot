#include "GUI.h"
#include "main.h"
#include "BUTTON.h"
#include "DIALOG.h"

static void _DemoButton(void)
{
  BUTTON_Handle hButton, hB1, hB2, hB3, hB4;
  PROGBAR_Handle hProgbar;
  uint8_t i;

  /* Create progress bar at location x = 10, y = 10, length = 219, height = 30 */
  hProgbar = PROGBAR_CreateEx(10, 10, 219, 30, 0, WM_CF_SHOW, 0, GUI_ID_PROGBAR0);
  /* Set progress bar font */
  PROGBAR_SetFont(hProgbar, &GUI_Font8x16);
  /* Set progress bar text */
  PROGBAR_SetText(hProgbar, "LOADING..Please wait..");

  /* Imitate loading */
  for (i = 1; i <= 100; i++)
  {
    /* Set bar */
    PROGBAR_SetValue(hProgbar, i);
    /* Little delay, update value on LCD */
    GUI_Delay(20);
  }

  // /* Create button with GUI_ID_OK ID number */
  // hButton = BUTTON_CreateEx(10, 50, 219, 30, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
  // /* Set text and font */
  // BUTTON_SetText(hButton, "Click me to continue..");
  // BUTTON_SetFont(hButton, &GUI_Font8x15B_ASCII);

  // /* Execute, show button */
  // GUI_Exec();

  // /* Wait till button pressed */
  // while (1)
  // {
  //   /* Check if button was pressed */
  //   if (GUI_GetKey() == GUI_ID_BUTTON0)
  //   {
  //     /* Led Off */
  //     BSP_LED_Toggle(LED4);

  //     /* Stop while loop */
  //     break;
  //   }
  //   /* Toggle green led */
  //   BSP_LED_Toggle(LED2);
  //   /* Delay 100ms */
  //   GUI_Delay(100);
  // }

  // /* Delete button functionality */
  // GUI_DispStringHCenterAt("Delete", 400, 20);
  // /* Delete button from LCD */
  // GUI_ClearRect(10, 50, 269, 90);

  /* Create buttons for leds control */
  hB1 = BUTTON_CreateEx(10, 90, 105, 50, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
  hB2 = BUTTON_CreateEx(124, 90, 105, 50, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON2);
  hB3 = BUTTON_CreateEx(10, 150, 105, 50, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON3);
  hB4 = BUTTON_CreateEx(124, 150, 105, 50, 0, WM_CF_SHOW, 0, GUI_ID_BUTTON4);

  /* Set font for buttons */
  BUTTON_SetFont(hB1, &GUI_Font13HB_ASCII);
  BUTTON_SetFont(hB2, &GUI_Font13HB_ASCII);
  BUTTON_SetFont(hB3, &GUI_Font13HB_ASCII);
  BUTTON_SetFont(hB4, &GUI_Font13HB_ASCII);

  /* Set button text */
  BUTTON_SetText(hB1, "GREEN on");
  BUTTON_SetText(hB2, "GREEN off");
  BUTTON_SetText(hB3, "RED on");
  BUTTON_SetText(hB4, "RED off");

  /* Button styling */
  /* Background color when button is not pressed */
  BUTTON_SetBkColor(hB1, BUTTON_CI_UNPRESSED, GUI_DARKGREEN);
  /* Background color when button is pressed */
  BUTTON_SetBkColor(hB1, BUTTON_CI_PRESSED, GUI_GREEN);

  /* Background color when button is not pressed */
  BUTTON_SetBkColor(hB3, BUTTON_CI_UNPRESSED, GUI_DARKRED);
  /* Background color when button is pressed */
  BUTTON_SetBkColor(hB3, BUTTON_CI_PRESSED, GUI_RED);

  /* Show buttons */
  GUI_Exec();

  while (1)
  {
    /* Get pressed key */
    switch (GUI_GetKey())
    {
    case GUI_ID_BUTTON1:
      /* Button 1 pressed */
      GUI_DispStringHCenterAt("Taipei Main Station", 400, 260);
      break;
    case GUI_ID_BUTTON2:
      /* Button 2 pressed */
      GUI_DispStringHCenterAt("Taipei Main Station", 400, 280);
      break;
    case GUI_ID_BUTTON3:
      /* Button 3 pressed */
      GUI_DispStringHCenterAt("Taipei Main Station", 400, 300);
      break;
    case GUI_ID_BUTTON4:
      /* Button 4 pressed */
      GUI_DispStringHCenterAt("Taipei Main Station", 400, 320);
      break;
    default:
      break;
    }
  }
}

void MainTask(void)
{
  // TouchCalibrate();
  GUI_Clear();
  // Draw backgreound
  GUI_SetBkColor(GUI_BLACK);
  GUI_DrawGradientH(0, 0, 800, 70, 0x0000FF, 0x00FFFF);
  GUI_DrawGradientH(0, 71, 800, 480, GUI_BLUE, GUI_GREEN);
  // Text setting
  GUI_SetTextMode(GUI_TM_TRANS);
  GUI_SetColor(GUI_WHITE);
  // Draw title
  GUI_SetFont(GUI_FONT_32B_1);
  GUI_DispStringHCenterAt("Taipei Main Station", 400, 20);
  // Draw introduction
  GUI_SetFont(&GUI_Font20B_ASCII);
  GUI_DispStringHCenterAt("Please choose the bus you want to take:", 250, 90);

  while (1)
  {
    _DemoButton();
  }
}
