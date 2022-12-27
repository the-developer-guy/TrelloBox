#include "m5control.h"

void m5Setup(M5EPD& m5, M5EPD_Canvas& canvas, uint64_t sleepUs)
{
  M5.begin(false, true, true, false, true);
  M5.EPD.SetRotation(0);
  M5.EPD.Clear(true);
  M5.RTC.begin();
  canvas.loadFont("/ArialBlack.ttf", SD);
  canvas.createCanvas(960, 540);
  canvas.createRender(32, 1024);
  canvas.setTextSize(32);
  canvas.setTextColor(15);
  canvas.drawString("Updating...", 400, 250);
  canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
  esp_sleep_enable_timer_wakeup(sleepUs);
  gpio_hold_en((gpio_num_t)M5EPD_MAIN_PWR_PIN);
  gpio_deep_sleep_hold_en();
  canvas.clear();
  Serial.println("ESP started");
}