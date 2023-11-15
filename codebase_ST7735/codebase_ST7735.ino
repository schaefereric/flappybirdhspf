/* 128x160

Pinnung
 *  Display (Pin) - Arduino Nano
 *  GND  (1) - GND  
 *  VCC  (2) - 5V
 *  SCK  (3) - D13
 *  SDA  (4) - D11
 *  RES  (5) - D8
 *  RS   (6) - D9
 *  CS   (7) - D10
 *  LEDA (8) - 3.3V
 *  
 *  LEDA kann auch an 5V doch dann wird das Display sehr schnell sehr heiß - was ich nicht für optimal halte.
 *  Beim Betrieb mit 3.3V ist das Dispklay nur minimal dunkler und bleibt kalt.

https://learn.adafruit.com/adafruit-gfx-graphics-library/coordinate-system-and-units
https://www.greth.me/arduino-nano-mit-1-77-zoll-spi-tft-display-st7735-chipsatz/

#define ST7735_BLACK ST77XX_BLACK
#define ST7735_WHITE ST77XX_WHITE
#define ST7735_RED ST77XX_RED
#define ST7735_GREEN ST77XX_GREEN
#define ST7735_BLUE ST77XX_BLUE
#define ST7735_CYAN ST77XX_CYAN
#define ST7735_MAGENTA ST77XX_MAGENTA
#define ST7735_YELLOW ST77XX_YELLOW
#define ST7735_ORANGE ST77XX_ORANGE
 */




#define TFT_PIN_CS   10 // Arduino-Pin an Display CS   
#define TFT_PIN_DC   9  // Arduino-Pin an 
#define TFT_PIN_RST  8  // Arduino Reset-Pin

//#include <SPI.h>             // SPI für die Kommunikation
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_RST);  // ST7735-Bibliothek Setup
#define TFT_MOSI 11  // Data out
#define TFT_SCLK 13  // Clock out
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_MOSI, TFT_SCLK, TFT_PIN_RST);

void setup() {
  tft.initR(INITR_GREENTAB); 

  tft.fillScreen(ST7735_BLACK);
  
  //tft.drawRect(14, 30, 100, 100, ST7735_GREEN);
  //tft.drawRect(18, 34, 92, 92, ST7735_GREEN);

  int parameter = 4;

  for (int i = 1; i < 13; i++) {
    tft.drawRect(14 + (parameter * i), 30 + (parameter * i), 100 - (parameter * 2 * i), 100 - (parameter * 2* i), ST7735_GREEN);
  }
}

void loop() {

}
