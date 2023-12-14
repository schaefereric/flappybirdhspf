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
#include <ArduinoSTL.h>

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_PIN_RST);                       // vieeel viel schneller als mit anderer init
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_PIN_CS, TFT_PIN_DC, TFT_MOSI, TFT_SCLK, TFT_PIN_RST); // sau langsam

#define buttonpinA 2
#define buttonpinB 3
#define buttonpinC 4
#define buttonpinD 5

#define do_not_erase 0
#define erase_screen 1
#define erase_square 2

enum pillar_directions {bottom, top};
#define PILLAR_WIDTH 20
#define DISTANCE_BETWEEN_PILLARS 40
#define BIRD_SIZE 15


struct buttons_class {
  struct button {
    bool state;
    int pin;
    int digitalread;
    int counter;
    unsigned long timer;
    unsigned long nDelay;

    button () {
      //this->pin = iPin;
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

struct vector2 {
  int x;
  int y;
  vector2()               { this->x = 0;  this->y = 0; }
  vector2(int ix, int iy) { this->x = ix; this->y = iy; }

  void set(int ix, int iy) { this->x = ix; this->y = iy; }
};


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


struct bird_class {
  //double position;
  //double previous_position;
  int g;
  double maxSpeed;
  double speed;
  double speed_diff;
  int xPos; 
  corners corner;
  vector2 previous_position;

  bird_class() {
    this->reset();
  }

  void applyPhysics() {
    this->previous_position.y = this->corner.topLeft.y;

    corner.topLeft.y = (corner.topLeft.y + (speed * speed_diff)) - (0.5 * this->g * speed_diff * speed_diff);
    speed = speed + (this->g * speed_diff);

    // Prevent bird from exceeding vertical border
    if (corner.topLeft.y < 0) {
      corner.topLeft.y = 0;
    }

    this->calculateCorners_topLeft();
  }
  // speed diff koennte zeit sein
  // siehe: senkrechter wurf nach oben

  void flap() {
    this->speed = -27.0;
  }


  void draw(int eraseMethod) {                // refer for parameters to top of code
    switch (eraseMethod) {
    case do_not_erase:
      break;

    case erase_screen: 
      tft.fillScreen(ST7735_BLACK);
      break;

    case erase_square:
      tft.fillRect(previous_position.x, previous_position.y, BIRD_SIZE, BIRD_SIZE, ST7735_BLACK);
      break;

    default:
      break; 
    }

    tft.fillRect(corner.topLeft.x, corner.topLeft.y, BIRD_SIZE, BIRD_SIZE, ST7735_YELLOW);
  }

  void reset() {
    //this->position          = 30.0;
    //this->previous_position = 0.0;
    corner.topLeft.y = 30;                 // Relevant Parameter !!!
    corner.topLeft.x = 30;
    previous_position.y = 30;
    previous_position.x = 30;

    this->g                 = 20;          // default: 20
    this->maxSpeed          = 20.0;        // default: 20.0
    this->speed             = maxSpeed;
    this->speed_diff        = 0.175;       // default: 0.05
    //this->xPos              = 50;
  }

  void calculateCorners_topLeft() {
    corner.topRight.x = corner.topLeft.x + BIRD_SIZE;
    corner.topRight.y = corner.topLeft.y;

    corner.bottomLeft.x = corner.topLeft.x;
    corner.bottomLeft.y = corner.topLeft.y + BIRD_SIZE;

    corner.bottomRight.x = corner.topLeft.x + BIRD_SIZE;
    corner.bottomRight.y = corner.topLeft.y + BIRD_SIZE;
  }

  bool check_border_collision() {
    // Returns true if border is hit
    calculateCorners_topLeft();
    if (corner.bottomLeft.y >= 128) {
      return true;
    }
    else return false;
  }

};


struct pillars {

  struct single_pillar {
    corners corner;
    pillar_directions direction;
    int height;
    vector2 previous_position;

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

    void make_pillar_from_height(int iHeight, int x_coordinate, int y_offset) {
      this->height = iHeight;
      corner.topLeft.x = x_coordinate;
      corner.topLeft.y = 128 - iHeight - y_offset;
    }

    void calculateCorners_topLeft() {
      corner.topRight.x = corner.topLeft.x + PILLAR_WIDTH;
      corner.topRight.y = corner.topLeft.y;

      corner.bottomLeft.x = corner.topLeft.x;
      corner.bottomLeft.y = corner.topLeft.y + height;

      corner.bottomRight.x = corner.topLeft.x + PILLAR_WIDTH;
      corner.bottomRight.y = corner.topLeft.y + height;

    }

    /* Um zwei Pillar zu generieren muss erst der untere Pillar generiert werden
     * Nötige Parameter: Random Height, Random Distance, X-Koordinate
     * Der untere Pillar braucht nur die Height und X-Koordinate,
     * der obere Pillar braucht die selbe Height, eine Distance und eine X-Koordinate
     * Davon ausgehend generiert sich der obere Pillar eigenständig
     */

    void make_pillar_bottom(int iHeight, int x_coordinate) {
      this->direction = bottom;

      make_pillar_from_height(iHeight, x_coordinate, 0);
      calculateCorners_topLeft();
    }

    void make_pillar_top(int iHeight_of_bottom, int x_coordinate, int distance) {
      this->direction = top;
      // WAS IST DIE HEIGHT ???????
      // HEIGHT-OBEN = 128 - distance - height
      // y_offset = distance + height

      int upper_height = 128 - distance - iHeight_of_bottom;
      int offset       = distance + iHeight_of_bottom;

      make_pillar_from_height(upper_height, x_coordinate, offset);
      calculateCorners_topLeft();
    }

    void draw(int eraseMethod) {
      switch (eraseMethod) {
        case do_not_erase:
          break;

        case erase_screen: 
          tft.fillScreen(ST7735_BLACK);
          break;

        case erase_square:
          tft.drawRect(previous_position.x, previous_position.y, PILLAR_WIDTH, height, ST7735_BLACK);
          break;

        default:
          break; 
        }
      tft.fillRect(corner.topLeft.x, corner.topLeft.y, PILLAR_WIDTH, height, ST7735_GREEN);
    }
    
    void shift_left() {
      previous_position.x = corner.topLeft.x;
      previous_position.y = corner.topLeft.y;

      --corner.topLeft.x;
      --corner.topRight.x;
      --corner.bottomLeft.x;
      --corner.bottomRight.x;
    }

  }; // single_pillar

  struct two_pillars {
    single_pillar lower;
    single_pillar upper;
    bool score_counted; // If true, the player already got a Score for passing this pillars

    two_pillars () {
      score_counted = false;
      this->initialize();
    }

    void initialize() {
      // pull random numbers
      // random(minHeight, maxHeight);
      int nHeight;
      int nDistance;
      int border_size = 15;
      int x_coordinate = 159;

      do {
      randomSeed(analogRead(5));
      nHeight    = random(border_size, 90);
      nDistance  = random(50, 80);
      }
      while (nHeight + nDistance > 128 - border_size); // shuffle again if height+distance is too large

      lower.make_pillar_bottom(nHeight, x_coordinate);
      upper.make_pillar_top(nHeight, x_coordinate, nDistance);
    }

    void draw(int eraseMethod) {
      lower.draw(eraseMethod);
      upper.draw(eraseMethod);
    }

    void shift_left() {
      lower.shift_left();
      upper.shift_left();
    }

    bool check_if_outofbounds() {
      // Returns true if invisible
      if (lower.corner.topRight.x < 0) {
        return true;
      }
      else return false;
    }

    bool check_for_distance_between_pillars() {
      if (lower.corner.topRight.x < 160 - DISTANCE_BETWEEN_PILLARS) {
        return true;
      }
      else return false;
    }

    // Bird-Pillar collision is determined in gameloop, because bird and pillar do not have access to each other
    // Similarly, the Score is also determined in gameloop

    void set_score_counted(bool input) {
      this->score_counted = input;
    }

  }; // two_pillars

  // This class manages all pillars in the game. It spawns and destroys pillars as needed. Pillars should only be moved via this class

  std::vector<two_pillars *> pillarvector;

  void initialize_vector() {
    pillarvector.push_back(new two_pillars);
  }

  void clear_vector() {
    for (auto i : pillarvector) {
      delete i;
    }
    pillarvector.clear();
  }

  void shift_left() {
    for (auto i : pillarvector) {
      i->shift_left();
    }
  }

  void check_if_front_is_outofbounds() {
    if (pillarvector.front()->check_if_outofbounds()) {
      delete pillarvector.front();
      pillarvector.erase(pillarvector.begin());
    }
  }

  void check_if_new_pillar_is_needed() {
    if (pillarvector.back()->check_for_distance_between_pillars()) {
      pillarvector.push_back(new two_pillars);
      pillarvector.back()->initialize();
    }
  }

  void shift_left_and_check() {
    shift_left();
    check_if_front_is_outofbounds();
    check_if_new_pillar_is_needed();
  }

  void draw_all_pillars(int eraseMethod) {
    for (auto i : pillarvector) {
      i->draw(eraseMethod);
    }
  }

  
}; // pillars

struct gameloop {
  bird_class * bird;
  buttons_class * buttonsobj;
  pillars * pillarsobj;

  bool runLoop;
  bool pause;

  int score;

  gameloop() {
    bird = new bird_class;
    buttonsobj = new buttons_class;
    pillarsobj = new pillars;
    this->runLoop = false;
    this->pause = false;
    this->score = 0;
  }

  ~gameloop() {
    delete bird;
    delete buttonsobj;
    delete pillarsobj;
  }

  void loop() {
    unsigned long oldTimer = millis();
    unsigned long nDelay = 20;

    tft.fillScreen(ST7735_BLACK);
    pillarsobj->initialize_vector();

    while (runLoop) {
      buttonsobj->refreshAll();
      if (buttonsobj->D.isOn()) {
        this->bird->reset();
        this->switchPause();
        }

      while (!pause) {
        if (millis() >= oldTimer + nDelay) {
          buttonsobj->refreshAll();

          if (buttonsobj->A.isOn()) {
            this->bird->reset();
          }
          if (buttonsobj->B.isOn()) {this->bird->flap();}
          if (buttonsobj->C.isOn()) {
            this->bird->applyPhysics();
            this->bird->draw(erase_screen); 
          }
          if (buttonsobj->D.isOn()) {this->switchPause();}

          this->bird->applyPhysics();
          this->bird->draw(erase_square); 

          this->pillarsobj->shift_left_and_check();
          this->pillarsobj->draw_all_pillars(erase_square);

          this->check_score();
          this->print_score();

          if (this->check_bird_pillar_collision()) {
            this->switchPause();
          }

          if (this->bird->corner.bottomLeft.y >= 128) {
            this->switchPause();
          }

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

  bool check_bird_pillar_collision() {

    for (auto i : pillarsobj->pillarvector) {
      if (
            // X-Axis
          ( (bird->corner.topRight.x > i->upper.corner.topLeft.x && bird->corner.topRight.x < i->upper.corner.topRight.x)
            ||
            (bird->corner.topLeft.x > i->upper.corner.topLeft.x && bird->corner.topLeft.x < i->upper.corner.topRight.x) )

            &&

            // Y-Axis
          ( (bird->corner.topRight.y > i->upper.corner.topLeft.y && bird->corner.topRight.y < i->upper.corner.bottomLeft.y) // Upper Pillar
            ||
            (bird->corner.bottomRight.y > i->lower.corner.topLeft.y && bird->corner.bottomRight.y < i->lower.corner.bottomLeft.y) ) // Lower Pillar

        ) {
          return true;
        } // if
    } // for
    return false;
  }

  void check_score() {
    if ((pillarsobj->pillarvector.front()->lower.corner.topRight.x < bird->corner.topLeft.x) && pillarsobj->pillarvector.front()->score_counted == false) {
      score++;
      //pillarsobj->pillarvector.front()->score_counted == true; // Somehow doesnt work
      pillarsobj->pillarvector.front()->set_score_counted(true);
    }
  }

  void print_score() {
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setTextSize(1);
    tft.setTextWrap(true);

    tft.print("Score: ");
    tft.print(this->score);
  }

  void press_button_screen() {
    
  }

  void game_over_screen() {

  }



};

//buttons_class buttonsobj;
//bird b;
gameloop gameloop1;
//pillars::single_pillar sp_b;
//pillars::single_pillar sp_t;
//pillars::two_pillars tp1;
//pillars p1;

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
  
  //tft.drawRect(90, 60, 20, 20, ST7735_GREEN);
  //b.setButtonsRef(buttonsobj);
  gameloop1.setLoop(true);
  //p1.initialize_vector();
}

void loop() {

  gameloop1.loop();

  }
