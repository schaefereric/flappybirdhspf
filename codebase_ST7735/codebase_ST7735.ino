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

void buttonPolling_blocking(); // Forward Declaration
void buttonPolling_nB();

#define do_not_erase 0
#define erase_screen 1
#define erase_square 2

struct buttons_class {
  struct button {
    bool state;
    int pin;
    int digitalread;
    int counter;
    unsigned long timer;
    unsigned long nDelay;

    button () {
      this->digitalread = 1;
      this->state = false;
      this->counter = 0;
      this->timer = 0;
      this->nDelay = 50;
      pinMode(this->pin, INPUT_PULLUP);
    }

    bool isOn() {
      return state;
    }

    void readPin() {
      this->digitalread = digitalRead(this->pin);
    }

    void refresh() {
      readPin();
      if (this->digitalread == LOW && this->counter == 0) {
        this->state = true;
        counter = 1;
        this->timer = millis();
        return;
      }
      if (this->digitalread == LOW && this->counter == 1) {
        this->state = false;
        counter = 2;
        return;
      }
      if (this->digitalread == HIGH && this->counter >= 1 && millis() >= this->timer + this->nDelay) {
        this->state = false;
        counter = 0;
        this->timer = 0;
      }
    }

  };

  button A;
  button B;
  button C;
  button D;

  buttons_class () {
    this->A.pin = buttonpinA;
    this->B.pin = buttonpinB;
    this->C.pin = buttonpinC;
    this->D.pin = buttonpinD;
  }

  void refreshAll() {
    this->A.refresh();
    this->B.refresh();
    this->C.refresh();
    this->D.refresh();
  }

};

struct bird_class {
  double position;
  double previous_position;
  int g;
  double maxSpeed;
  double speed;
  double speed_diff;
  int xPos; 
  bool runLoop;

  bird_class() {
    this->reset();
  }

  void applyPhysics() {
    this->previous_position = this->position;

    position = (position + (speed * speed_diff)) - (0.5 * this->g * speed_diff * speed_diff);
	  speed = speed + (this->g * speed_diff);
  }

  void flap() {
    this->speed = -45.0;
  }

  int checkBoundaries() {
    if (this->position >= 128) {
      tft.setCursor(0, 20);
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(1);
      tft.setTextWrap(true);
      tft.print("Bird out of bounds!  (y-axis)");

      this->position = 128;
      return 0;
    }
    else if (this->position < 0) {
      tft.setCursor(0, 20);
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(1);
      tft.setTextWrap(true);
      tft.print("Bird out of bounds!  (y-axis)");

      this->position = 0;
      return 0;
    }
    else return 1;
  }

  void draw(int eraseMethod) {                // refer for parameters to top of code
    switch (eraseMethod) {
    case do_not_erase:
      break;

    case erase_screen: 
      tft.fillScreen(ST7735_BLACK);
      break;

    case erase_square:
      tft.drawRect(xPos, previous_position, 15, 15, ST7735_BLACK);
      break;

    default:
      break; 
    }

    tft.drawRect(xPos, position, 15, 15, ST7735_GREEN);
  }

  void reset() {
    this->runLoop = false;

    this->position          = 30.0;
    this->previous_position = 0.0;
    this->g                 = 30;          // default: 20
    this->maxSpeed          = 20.0;        // default: 20.0
    this->speed             = maxSpeed;
    this->speed_diff        = 0.175;       // default: 0.05
    this->xPos              = 50;
  }

  // void serialDebug() {
  //   Serial.println("position " + this->position);
  //   Serial.println("speed " + this->speed);
  // }

  void onScreenDebug() {
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
    tft.setTextSize(1);
    tft.setTextWrap(true);

    tft.print("position ");
    tft.println(this->position);
    tft.print("speed ");
    tft.println(this->speed);
    //tft.print("test");
  }

};

struct gameloop {
  bird_class * bird;
  buttons_class * buttonsobj;
  bool runLoop;
  bool pause;

  gameloop() {
    bird = new bird_class;
    buttonsobj = new buttons_class;
    this->runLoop = false;
    this->pause = false;
  }

  ~gameloop() {
    delete bird;
    delete buttonsobj;
  }

  void loop() {
    unsigned long oldTimer = millis();
    unsigned long nDelay = 20;

    tft.fillScreen(ST7735_BLACK);

    while (runLoop) {
      buttonsobj->refreshAll();
      if (buttonsobj->D.isOn()) this->switchPause();

      while (!pause) {
        if (millis() >= oldTimer + nDelay) {
          this->bird->checkBoundaries();
          buttonsobj->refreshAll();
          //this->onScreenDebug();

          if (buttonsobj->A.isOn()) this->bird->reset();
          if (buttonsobj->B.isOn()) this->bird->flap();
          if (buttonsobj->C.isOn()) {
            this->bird->applyPhysics();
            this->bird->checkBoundaries();
            this->bird->draw(erase_screen); 
          }
          if (buttonsobj->D.isOn()) this->switchPause();

          this->bird->applyPhysics();
          this->bird->draw(erase_square); 

          oldTimer = millis();
        }

      } // while (!pause)
    } // while (runLoop)
  }

  void setLoop(bool activateLoop) {
    this->runLoop = activateLoop;
  }

  void setPause(bool activatePause) {
    this->pause = activatePause;
  }

  void switchLoop() {
         if (this->runLoop == false) { this->runLoop = true; }
    else if (this->runLoop == true)  { this->runLoop = false; }
  }
  
  void switchPause() {
         if (this->pause == false) { this->pause = true; }
    else if (this->pause == true)  { this->pause = false; }
  }

};

//buttons_class buttonsobj;
//bird b;
gameloop gameloop1;

void setup() {
  // Setup Serial Communication
  Serial.begin(9600);
  Serial.println();
  Serial.println("setup");

  // Initialize Screen
  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(1);      // equivalent to adafruit doc example orientation

  // Initialize Buttons
  pinMode(buttonpinA, INPUT_PULLUP);
  pinMode(buttonpinB, INPUT_PULLUP);
  pinMode(buttonpinC, INPUT_PULLUP);
  pinMode(buttonpinD, INPUT_PULLUP);

  tft.fillScreen(ST7735_BLACK);
  
  //tft.drawRect(0, 0, 100, 100, ST7735_GREEN);
  //b.setButtonsRef(buttonsobj);
  gameloop1.setLoop(true);
}

void loop() {
  
  //b.onScreenDebug();
  //buttons.refreshAll();
  //buttons.A.refresh();
  gameloop1.loop();
  //gameloop1.buttonsobj->refreshAll();

  //  tft.setCursor(0, 0);
  //  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);
  //  tft.setTextSize(1);
  //  tft.setTextWrap(true);

  //  tft.println(millis());

  //  tft.print("A Read:");
  //  tft.println(gameloop1.buttonsobj->A.digitalread);
  //  tft.print("A State:");
  //  tft.println(gameloop1.buttonsobj->A.isOn());
  //  tft.print("A Counter:");
  //  tft.println(gameloop1.buttonsobj->A.counter);

  //   tft.print("D Read:");
  //   tft.println(gameloop1.buttonsobj->D.digitalread);
  //   tft.print("D State:");
  //   tft.println(gameloop1.buttonsobj->D.isOn());
  //   tft.print("D Counter:");
  //   tft.println(gameloop1.buttonsobj->D.counter);

//   tft.print("C State:");
//   tft.println(buttons.C.isOn());
//   tft.print("C Counter:");
//   tft.println(buttons.C.counter);

//   tft.print("D State:");
//   tft.println(buttons.D.isOn());
//   tft.print("D Counter:");
//   tft.println(buttons.D.counter);
  }

/*
void buttonPolling_blocking() {
  // Read Buttons
  buttonReadA = digitalRead(buttonpinA);
  buttonReadB = digitalRead(buttonpinB);
  buttonReadC = digitalRead(buttonpinC);
  buttonReadD = digitalRead(buttonpinD);

  if (buttonReadA == LOW) {

    b.reset();

    // Sticky Button
    while (buttonReadA == LOW) {
      buttonReadA = digitalRead(buttonpinA);
      Serial.println("holding button A");
    }
  }

  if (buttonReadB == LOW) {

    b.flap();

    // Sticky Button
    while (buttonReadB == LOW) {
      buttonReadB = digitalRead(buttonpinB);
      Serial.println("holding button B");
    }
  }

  if (buttonReadC == LOW) {

    b.applyPhysics();
    b.checkBoundaries();
    b.draw(erase_screen);    

    // // Sticky Button
    // while (buttonReadC == LOW) {
    //   buttonReadC = digitalRead(buttonpinC);
    //   Serial.println("holding button C");
    // }
  }

  if (buttonReadD == LOW) {

    b.switchLoop();

    // Sticky Button
    while (buttonReadD == LOW) {
      buttonReadD = digitalRead(buttonpinD);
      Serial.println("holding button D");
    }
  }
}

void buttonPolling_nB() {
  unsigned long nDelay = 200;

  static int buttonCountA = 0;
  static unsigned long buttonTimerA = 0;

  static int buttonCountB = 0;
  static unsigned long buttonTimerB = 0;

  static int buttonCountC = 0;
  static unsigned long buttonTimerC = 0;

  static int buttonCountD = 0;
  static unsigned long buttonTimerD = 0;

  buttonReadA = digitalRead(buttonpinA);
  buttonReadB = digitalRead(buttonpinB);
  buttonReadC = digitalRead(buttonpinC);
  buttonReadD = digitalRead(buttonpinD);


  if (buttonReadA == LOW && buttonCountA == 0 && millis() >= buttonTimerA + nDelay) {
    buttonA = true;

    buttonCountA = 1;
    buttonTimerA = millis();
  }
  if (buttonReadA == HIGH && buttonCountA == 1) {
    buttonCountA = 0;
    buttonA = false;
  }

  if (buttonReadB == LOW && buttonCountB == 0 && millis() >= buttonTimerB + nDelay) {
    buttonB = true;

    buttonCountB = 1;
    buttonTimerB = millis();
  }
  if (buttonReadB == HIGH && buttonCountB == 1) {
    buttonCountB = 0;
    buttonB = false;
  }

  if (buttonReadC == LOW && buttonCountC == 0 && millis() >= buttonTimerC + nDelay) {
    buttonC = true; 

    buttonCountC = 1;
    buttonTimerC = millis();
  }
  if (buttonReadC == HIGH && buttonCountC == 1) {
    buttonCountC = 0;
    buttonC = false;
  }

  if (buttonReadD == LOW && buttonCountD == 0 && millis() >= buttonTimerD + nDelay) {
    buttonD = true;

    buttonCountD = 1;
    buttonTimerD = millis();
  }
  if (buttonReadD == HIGH && buttonCountD == 1) {
    buttonCountD = 0;
    buttonD = false;
  }
  
}
*/