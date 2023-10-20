#include <ArduinoSTL.h>

#define display_array_size 8
// ascii 8x8 dot font
#define data_null 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // null char
#define data_ascii_A 0x02,0x0C,0x18,0x68,0x68,0x18,0x0C,0x02 /*"A",0*/
#define data_ascii_B 0x00,0x7E,0x52,0x52,0x52,0x52,0x2C,0x00 /*"B",1*/
#define data_ascii_C 0x00,0x3C,0x66,0x42,0x42,0x42,0x2C,0x00 /*"C",2*/
#define data_ascii_D 0x00,0x7E,0x42,0x42,0x42,0x66,0x3C,0x00 /*"D",3*/
#define data_ascii_E 0x00,0x7E,0x52,0x52,0x52,0x52,0x52,0x42 /*"E",4*/
#define data_ascii_F 0x00,0x7E,0x50,0x50,0x50,0x50,0x50,0x40 /*"F",5*/
#define data_ascii_G 0x00,0x3C,0x66,0x42,0x42,0x52,0x16,0x1E /*"G",6*/
#define data_ascii_H 0x00,0x7E,0x10,0x10,0x10,0x10,0x7E,0x00 /*"H",7*/
#define data_ascii_I 0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00 /*"I",8*/

byte data_ascii[][display_array_size] = {
  data_null,
  data_ascii_A, 
  data_ascii_B,
  data_ascii_C,
  data_ascii_D,
  data_ascii_E,
  data_ascii_F,
  data_ascii_G,
  data_ascii_H,
  data_ascii_I,
};

//the pin to control ROW
const int row[] = {2,3,4,5,17,16,15,14};
/*
const int row1 = 2; // the number of the row pin 24
const int row2 = 3; // the number of the row pin 23
const int row3 = 4; // the number of the row pin 22
const int row4 = 5; // the number of the row pin 21
const int row5 = 17; // the number of the row pin 4
const int row6 = 16; // the number of the row pin 3
const int row7 = 15; // the number of the row pin 2
const int row8 = 14; // the number of the row pin 1
*/
//the pin to control COl
const int col[] = {6,7,8,9,10,11,12,13};
/*
const int col1 = 6; // the number of the col pin 20
const int col2 = 7; // the number of the col pin 19
const int col3 = 8; // the number of the col pin 18
const int col4 = 9; // the number of the col pin 17
const int col5 = 10; // the number of the col pin 16
const int col6 = 11; // the number of the col pin 15
const int col7 = 12; // the number of the col pin 14
const int col8 = 13; // the number of the col pin 13
*/

// Forward Declarations
void turnOn(int input_row, int input_col);
void turnOff(int input_row, int input_col);
void blank_screen();

// Global Variables
String incomingString = String("empty");

int buttonState0 = 0; // Button A
int buttonState1 = 0; // Button B

enum shiftOptions {up, down, left, right};



struct dot {
  // Attributes
  int row;
  int col;

  // Constructors
  dot () {
    this->row = 0;
    this->col = 0;
  }
  dot (int input_row, int input_col) {
    this->row = input_row;
    this->col = input_col;
  }

  // Methods
  void setDot(int input_row, int input_col) {
    this->row = input_row;
    this->col = input_col;
  }

  void drawDot(bool blankScreen) {
    if (blankScreen) {blank_screen();}

    turnOn(this->row, this->col);
  }

  void hideDot() {
    turnOff(this->row, this->col);
  }

  void shift(shiftOptions option, int distance) {
    switch (option) {
      case up: 
        //if (this->row == 0) {return;}
        this->row = this->row - distance;
        break;

      case down:
        //if (this->row == 7) {return;}
        this->row = this->row + distance;
        break;

      case left:
        //if (this->col == 0) {return;}
        this->col = this->col - distance;
        break;

      case right:
        //if (this->col == 7) {return;}
        this->col = this->col + distance;
        break;

      default: break;
    } 
  }

  void moveDown() {
    if (this->row == 7) {return;}            // If maximum reached, do nothing

    turnOff(this->row, this->col);         // Turn off current dot
    this->row = this->row + 1;            // Increment row
    this->drawDot(false);             // Draw new dot
  }

  void moveUp() {
    if (this->row == 0) {return;}            // If maximum reached, do nothing

    turnOff(this->row, this->col);         // Turn off current dot
    this->row = this->row - 1;            // Increment row
    this->drawDot(false);             // Draw new dot
  }

  void moveLeft() {
    if (this->col == 0) {return;}            // If maximum reached, do nothing

    turnOff(this->row, this->col);         // Turn off current dot
    this->col = this->col - 1;            // Increment row
    this->drawDot(false);             // Draw new dot
  }

  void moveRight() {
    if (this->col == 7) {return;}            // If maximum reached, do nothing

    turnOff(this->row, this->col);         // Turn off current dot
    this->col = this->col + 1;            // Increment row
    this->drawDot(false);             // Draw new dot
  }
};

struct pattern {
  std::vector<dot> dotvec;

  pattern () {}
  pattern (int input_row, int input_col) {
    dotvec.push_back((struct dot) {.row = input_row, .col = input_col});
  }

  void pushDot(int input_row, int input_col) {
    dotvec.push_back((struct dot) {.row = input_row, .col = input_col});
  }

  void popDot() {
    dotvec.pop_back();
  }

  void drawPattern(bool blankScreen) {
    if (blankScreen) {blank_screen();}

    for (auto & i : dotvec) {
      i.drawDot(false);
    }
  }

  void hidePattern() {
    for (auto & i : dotvec) {
      i.hideDot();
    }
  }

  virtual void shift(shiftOptions option, int distance) {
    for (auto & i : dotvec) {
      i.shift(option, distance);
    }
  }

};

// Global Drawing Functions
void turnOn(int input_row, int input_col) {
  digitalWrite(row[input_row], HIGH);
  digitalWrite(col[input_col], LOW);
}

void turnOff(int input_row, int input_col) {
  digitalWrite(row[input_row], LOW);
  digitalWrite(col[input_col], HIGH);
}

void blank_screen() {
  /*
  for (int i=2; i<18; i++) { 
    digitalWrite(i, LOW);
  }*/

  for (int i = 0; i <= 7; i++) { 
    digitalWrite(row[i], LOW);
    digitalWrite(col[i], HIGH);
  }
}

dot dot1(3,3);
pattern p1(0,3);

void setup () {
  // Setup Serial Communication
  Serial.begin(9600);
  Serial.print("setup");
  Serial.println();

  // Initialize Pinouts
  int i = 0;
  for(i=2;i<18;i++)
  {
    pinMode(i, OUTPUT);
  }

  for(i=2;i<18;i++) {
    digitalWrite(i, LOW);
  }

  blank_screen();

  // Initialize Button A & B
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  dot1.drawDot(true);

  p1.pushDot(1, 3);
  p1.pushDot(2, 3);
  p1.drawPattern(true);

}



void loop () {
  
  // Read and Store Serial Input
  if (Serial.available() > 0) {
    incomingString = Serial.readString();

    if (incomingString != "empty") {
      // Feedback
      Serial.print("Received: ");
      Serial.println(incomingString);

      if (incomingString == "tp") { // Traverse Pattern
        for (auto i : p1.dotvec) {
          Serial.print("row: ");
          Serial.print(i.row);
          Serial.print("  col: ");
          Serial.print(i.col);
          Serial.println();
        }
      }

    incomingString = "empty";
    }
  }

  // Read Buttons
  buttonState0 = digitalRead(A4);
  buttonState1 = digitalRead(A5);
  
  if (buttonState0 == HIGH) {
    p1.shift(down, 1);
    p1.drawPattern(true);

    while (buttonState0 == HIGH) {
      buttonState0 = digitalRead(A4);
      Serial.println("holding button A");
    }
  }

  if (buttonState1 == HIGH) {
    p1.shift(up, 1);
    p1.drawPattern(true);

    while (buttonState1 == HIGH) {
      buttonState1 = digitalRead(A5);
      Serial.println("holding button B");
    }
  }

  

  

  

}