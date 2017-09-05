#include "GUI.h"
#include "main.h"
#include "BUTTON.h"
#include "DIALOG.h"

// Data will get from wifi
uint8_t bus_number = 10;
char *station_name = "Taipei Main Station";
char *bus_name[15] = {"123", "234", "998", "666", "820", "Red 5", "Blue 4", "4443", "3232", "433"};

static void GetInfo(void)
{
  /*************************************/
  /* get station information from wifi */
  /* function(){ Dealing Wifi... }     */
  /*************************************/
  // station_name = "...";
  // bus_number = 10;
  // bus_name = { ... };
}

// Button sets
static void _DemoButton(void)
{
  // Bus button binding
  BUTTON_Handle busBt[bus_number];

  for (uint8_t i = 0; i < bus_number; i++)
  {
    /* Create Button */
    busBt[i] = BUTTON_CreateEx(50 + 140 * (i % 5), 140 + (i / 5) * 110, 120, 70, 0, WM_CF_SHOW, 0, i);
    /* Button font */
    BUTTON_SetFont(busBt[i], &GUI_Font20_ASCII);
    /* Button name */
    BUTTON_SetText(busBt[i], bus_name[i]);
    /* Background color when button is not pressed */
    BUTTON_SetBkColor(busBt[i], BUTTON_CI_UNPRESSED, GUI_DARKGREEN);
    /* Background color when button is pressed */
    BUTTON_SetBkColor(busBt[i], BUTTON_CI_PRESSED, GUI_GREEN);
  }

  /* Show buttons */
  GUI_Exec();

  while (1)
  {
    /* Get pressed key */
    for (uint8_t i = i; i < bus_number; i++)
    {
      /* Button I pressed */
      if (GUI_GetKey() == i)
      {
        // Call API return info via wifi
        // function callAPI(Url){ ... }
      }
    }
  }
}

void MainTask(void)
{
  // get information from wifi
  GetInfo();
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
  GUI_DispStringHCenterAt(station_name, 400, 20);
  // Draw introduction
  GUI_SetFont(&GUI_Font20B_ASCII);
  GUI_DispStringHCenterAt("Please choose the bus you want to take:", 250, 90);

  while (1)
  {
    _DemoButton();
  }
}
