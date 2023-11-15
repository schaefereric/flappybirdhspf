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

#define TFT_MOSI 11  // Data out
#define TFT_SCLK 13  // Clock out

#include <SPI.h>             // SPI für die Kommunikation
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_RST);                       // vieeel viel schneller als mit anderer init
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_MOSI, TFT_SCLK, TFT_PIN_RST); // sau langsam

#define buttonpinA 2
#define buttonpinB 3
#define buttonpinC 4
#define buttonpinD 5

int buttonStateA = 1; // Button A
int buttonStateB = 1; // Button B
int buttonStateC = 1; // Button C
int buttonStateD = 1; // Button D

void buttonPolling(); // Forward Declaration

#define do_not_erase 0
#define erase_screen 1
#define erase_square 2


struct bird {
  double position;
  double previous_position;
  int g;
  double maxSpeed;
  double speed;
  double speed_diff;
  int xPos; 
  bool runLoop;

  bird() {
    this->position          = 30.0;
    this->previous_position = 0.0;
    this->g                 = 40;         // default: 20
    this->maxSpeed          = 20.0;       // default: 20.0
    this->speed             = maxSpeed;
    this->speed_diff        = 0.05;       // default: 0.05
    this->xPos              = 50;
  }

  void applyPhysics() {
    position = (position + (speed * speed_diff)) - (0.5 * this->g * speed_diff * speed_diff);
	  speed = speed + (this->g * speed_diff);
  }

  void flap() {
    this->speed = -40.0;
  }

  int checkBoundaries() {
    if (this->position >= 160) {
      tft.setCursor(0, 20);
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(1);
      tft.setTextWrap(true);
      tft.print("Bird out of bounds! (y-axis)");

      this->position = 157;
      return 0;
    }
    else if (this->position < 0) {
      tft.setCursor(0, 20);
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(1);
      tft.setTextWrap(true);
      tft.print("Bird out of bounds! (y-axis)");

      this->position = 0;
      return 0;
    }
    else return 1;
  }

  void draw(bool erase) {
    if (erase) tft.fillScreen(ST7735_BLACK);

    tft.drawRect(xPos, position, 15, 15, ST7735_GREEN);
  }

  void loop() {
    while (runLoop) {
      this->applyPhysics();
      this->checkBoundaries();
      buttonPolling();
      this->onScreenDebug();

      this->draw(true); 
    }
  }

  void switchLoop() {
         if (this->runLoop == false) { this->runLoop = true; }
    else if (this->runLoop == true) { this->runLoop = false; }
  }

  void reset() {
    this->runLoop = false;

    this->position          = 30.0;
    this->previous_position = 0.0;
    this->g                 = 40;         // default: 20
    this->maxSpeed          = 20.0;       // default: 20.0
    this->speed             = maxSpeed;
    this->speed_diff        = 0.05;       // default: 0.05
    this->xPos              = 50;
  }

  // void serialDebug() {
  //   Serial.println("position " + this->position);
  //   Serial.println("speed " + this->speed);
  // }

  void onScreenDebug() {
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(1);
    tft.setTextWrap(true);

    tft.print("position ");
    tft.println(this->position);
    tft.print("speed ");
    tft.println(this->speed);
    //tft.print("test");
  }

};

bird b;

void setup() {
  // Setup Serial Communication
  Serial.begin(9600);
  Serial.println();
  Serial.println("setup");

  // Initialize Screen
  tft.initR(INITR_GREENTAB); 
  //tft.setRotation(1);      // equivalent to adafruit doc example orientation


  // Initialize Buttons
  pinMode(buttonpinA, INPUT_PULLUP);
  pinMode(buttonpinB, INPUT_PULLUP);
  pinMode(buttonpinC, INPUT_PULLUP);
  pinMode(buttonpinD, INPUT_PULLUP);

  tft.fillScreen(ST7735_BLACK);
  
  //tft.drawRect(0, 0, 100, 100, ST7735_GREEN);

  
  
}

void loop() {

  buttonPolling();
  b.onScreenDebug();
  b.loop();

}

void buttonPolling() {
  // Read Buttons
  buttonStateA = digitalRead(buttonpinA);
  buttonStateB = digitalRead(buttonpinB);
  buttonStateC = digitalRead(buttonpinC);
  buttonStateD = digitalRead(buttonpinD);

  if (buttonStateA == LOW) {

    b.reset();

    // Sticky Button
    while (buttonStateA == LOW) {
      buttonStateA = digitalRead(buttonpinA);
      Serial.println("holding button A");
    }
  }

  if (buttonStateB == LOW) {

    b.flap();

    // Sticky Button
    while (buttonStateB == LOW) {
      buttonStateB = digitalRead(buttonpinB);
      Serial.println("holding button B");
    }
  }

  if (buttonStateC == LOW) {

    b.applyPhysics();
    b.checkBoundaries();
    b.draw(true);    

    // // Sticky Button
    // while (buttonStateC == LOW) {
    //   buttonStateC = digitalRead(buttonpinC);
    //   Serial.println("holding button C");
    // }
  }

  if (buttonStateD == LOW) {

    b.switchLoop();

    // Sticky Button
    while (buttonStateD == LOW) {
      buttonStateD = digitalRead(buttonpinD);
      Serial.println("holding button D");
    }
  }

}
