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
        return;
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
  // speed diff koennte zeit sein
  // siehe: senkrechter wurf nach oben

  void flap() {
    this->speed = -35.0;
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
    this->g                 = 20;          // default: 20
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

struct vector2 {
  int x;
  int y;
  vector2()               { this->x = 0;  this->y = 0; }
  vector2(int ix, int iy) { this->x = ix; this->y = iy; }
  
  void set(int ix, int iy) { this->x = ix; this->y = iy; }
};

enum pillar_directions {bottom, top};
int PILLAR_WIDTH = 20;

struct corners {
  vector2 topLeft;
  vector2 topRight;
  vector2 bottomLeft;
  vector2 bottomRight;
  corners () {
    topLeft.set(0, 0);
    topRight.set(0, 0);
    bottomLeft.set(0, 0);
    bottomRight.set(0, 0);
  }
};

struct pillars {

  struct single_pillar {
    corners corner;
    pillar_directions direction;
    int height;
    
    single_pillar () {
      setTopLeft(0, 0);
      height = 0;
    }

    single_pillar (int iHeight) {
      this->height = iHeight;
    }

    void setTopLeft(int ix, int iy) {
      this->corner.topLeft.set(ix, iy);
    }

    void calculateCorners(pillar_directions iDirection) { 
      switch (iDirection) {
        case bottom: // topLeft is calculated in make_pillar()
          corner.topRight.x = corner.topLeft.x + PILLAR_WIDTH;
          corner.topRight.y = corner.topLeft.y;

          corner.bottomLeft.x = corner.topLeft.x;
          corner.bottomLeft.y = corner.topLeft.y + height;

          corner.bottomRight.x = corner.topLeft.x + PILLAR_WIDTH;
          corner.bottomRight.y = corner.topLeft.y + height;
          break;
        case top:

        break;
      }
    }

    void make_pillar_bottom(int iHeight, int x_coordinate) {
      this->height = iHeight;
      this->direction = bottom;
      corner.topLeft.x = x_coordinate;          // topLeft is generated here
      corner.topLeft.y = 128 - iHeight;

      corner.topRight.x = corner.topLeft.x + PILLAR_WIDTH;
      corner.topRight.y = corner.topLeft.y;

      corner.bottomLeft.x = corner.topLeft.x;
      corner.bottomLeft.y = corner.topLeft.y + height;

      corner.bottomRight.x = corner.topLeft.x + PILLAR_WIDTH;
      corner.bottomRight.y = corner.topLeft.y + height;
    }

    void make_pillar_top(int x_coordinate, int distance, vector2 given_topLeft, int given_height) {
      this->direction = top;
      // top topleft muss von bottom topleft abgelesen werden -> bottom zuerst spawnen
      //corner.topLeft.x = given_topLeft.x;
      corner.topLeft.x = x_coordinate;
      corner.topLeft.y = given_topLeft.y;

      corner.bottomLeft.x = corner.topLeft.x;
      corner.bottomLeft.y = corner.topLeft.y + distance;

      corner.topLeft.y = 0;

      corner.topRight.x = corner.topLeft.x + PILLAR_WIDTH;
      corner.topRight.y = 0;

      corner.bottomRight.x = corner.topRight.x;
      corner.bottomRight.y = corner.bottomLeft.y;
      

      this->height = 128 - (distance + height);

    }

    void draw() {
      /*switch (eraseMethod) {
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
        }*/
      tft.fillRect(corner.topLeft.x, corner.topLeft.y, PILLAR_WIDTH, height, ST7735_GREEN);
    }
    

  };

  
  
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
pillars::single_pillar sp_b;
pillars::single_pillar sp_t;

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
  
  //tft.drawRect(90, 60, 20, -20, ST7735_GREEN);
  //b.setButtonsRef(buttonsobj);
  //gameloop1.setLoop(true);
  //sp1.make_pillar(60, 100, bottom, 0);
  //sp1.draw();
  sp_b.make_pillar_bottom(30, 80);
  sp_t.make_pillar_top(40, 10, sp_b.corner.topLeft, 30);
  sp_b.draw();
  sp_t.draw();
}

void loop() {
 
  //gameloop1.loop();

  }
