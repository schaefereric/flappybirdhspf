#include <ArduinoSTL.h>
#include <list>

//the pin to control ROW
const int row[] = {2,3,4,5,17,16,15,14};

const int col[] = {6,7,8,9,10,11,12,13};

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

// Global Variables
String incomingString = String("empty");

#define buttonpinA 18
#define buttonpinB 19

int buttonState0 = 0; // Button A
int buttonState1 = 0; // Button B

enum shiftOptions {up, down, left, right};
enum byteOrder {lsb, msb};


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

  ~dot() {turnOff(this->row, this->col);}

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

  void move(shiftOptions option, int distance) {
    switch (option) {
    case down : 
      //if (this->row == 7) {return;}                // If maximum reached, do nothing
      turnOff(this->row, this->col);                // Turn off current dot
      this->row = this->row + distance;            // Increment row
      this->drawDot(false);                       // Draw new dot
      break;
    
    case up:
      //if (this->row == 0) {return;}            // If maximum reached, do nothing
      turnOff(this->row, this->col);            // Turn off current dot
      this->row = this->row - distance;        // Increment row
      this->drawDot(false);                   // Draw new dot
      break;

    case left:
      //if (this->col == 0) {return;}            // If maximum reached, do nothing
      turnOff(this->row, this->col);            // Turn off current dot
      this->col = this->col - distance;        // Increment row
      this->drawDot(false);                   // Draw new dot
      break;

    case right:
      //if (this->col == 7) {return;}            // If maximum reached, do nothing
      turnOff(this->row, this->col);            // Turn off current dot
      this->col = this->col + distance;        // Increment row
      this->drawDot(false);                   // Draw new dot
      break;
    }
  }
};

struct pattern {
  std::vector<dot *> dotvec;
  //dot dots[64];
  //int dots_used = 0;

  pattern () {}
  pattern (int input_row, int input_col) {
    dotvec.push_back(new dot(input_row, input_col));
  }
  ~pattern () {}

  void pushDot(int input_row, int input_col) {
    dotvec.push_back(new dot(input_row, input_col));
  }

  void popDot() {           // Pop last dot
    delete dotvec.back();
    dotvec.pop_back();
  }

  void drawPattern(bool blankScreen) {
    if (blankScreen) {blank_screen();}

    for (auto & i : dotvec) {
      i->drawDot(false);
    }
  }

  void hidePattern() {
    for (auto & i : dotvec) {
      i->hideDot();
    }
  }

  virtual void shift(shiftOptions option, int distance) {
    for (auto & i : dotvec) {
      i->shift(option, distance);
    }
  }

  virtual void move(shiftOptions option, int distance) {
    for (auto & i : dotvec) {
      i->move(option, distance);
    }
  }

  int importColumn(byte data, int column, byteOrder byteorder = msb) {
    if (byteorder == msb) {
      for (int row = 0; row < 8; row++) {             // MOST significant byte first
        bool isBitSet = (data & (1 << 8 - 1 - row)) != 0;
        
        if (isBitSet) {
          this->pushDot(row, column);
        } 
      }
      return 1;
    }

    else if (byteorder == msb) {
       for (int row = 0; row < 8; row++) {          // LEAST significant byte first
        bool isBitSet = (data & (1 << row)) != 0;   // Die "1" wird "row"-mal nach links gebitshifted. Das Bit an der Stelle (restliches byte wird mit Nullen aufgefuellt)
                                                    // wird dann mit dem Byte verundet. Wenn die gepruefte Stelle 0 ist, kommt 0 raus. Sonst ungleich 0.
        if (isBitSet) {
          this->pushDot(row, column);
        } 
      } 
      return 1;
    }

    else {return 0;}
  }

  void importPattern(byte bytearray[8], byteOrder byteorder = lsb) {
    for (int col = 0; col < 8; col++) {
      importColumn(bytearray[col], col, byteorder);
    }
  }

  void patternDebug() {
    int counter = 0;
    int address = 0;

    Serial.print("dotvec size: "); Serial.println(dotvec.size());

    for (auto i : dotvec) {
      address = (int)i;
      Serial.print("dot ");         Serial.print(counter); Serial.print(": ");
      Serial.print("row: ");        Serial.print(i->row);
      Serial.print("  col: ");      Serial.print(i->col);
      Serial.println();
      Serial.print("address: ");    Serial.print(address);
      Serial.println();
      counter++;
    }
    /* for (auto i : dotvec) {
      address = (int)i;

      Serial.print("dot " + counter + ": ");        
      Serial.print("row: " + i->row);
      Serial.print("  col: " + i->col);
      Serial.println();
      Serial.print("address: " + address);
      Serial.println();
      counter++;
    } */

  }

};


struct pillar : pattern {   // Double Pillar Implementation
  int spawnColumn;          // Column where the pillar appears 
  int holeSize;             // Size of Hole (vertical distance between pillars)
  int holeRow;              // Row where the hole begins
  int upperPillar_height;
  int lowerPillar_height;

  pillar(shiftOptions direction, int iHoleSize = 2, int iSpawnColumn = 7) {
    randomSeed(analogRead(5));
    this->spawnColumn = iSpawnColumn;

    if (direction == down) {

    }

    do { this->calculatePillarHeights(); }
    while (this->upperPillar_height + this->lowerPillar_height > 5 );

    //this->upperPillar_height = generateRandomPillarHeight();
    //this->lowerPillar_height = generateRandomPillarHeight();

    for (int i = 0; i < upperPillar_height; i++) {
      this->pushDot(i, this->spawnColumn);
    }

    for (int i = 0; i < upperPillar_height; i++) {
      this->pushDot(7 - i, this->spawnColumn);
    }

  }
  ~pillar () {}

  void calculatePillarHeights(int minHeight = 1, int maxHeight = 6, int minDistance = 2) {
    this->upperPillar_height = random(minHeight, maxHeight + 1);
    this->lowerPillar_height = random(minHeight, maxHeight - minDistance + 1);
  }

  int generateRandomPillarHeight(int minHeight = 1, int maxHeight = 6) {
    return random(minHeight, maxHeight); 
  }

  void randomizePillar() {
    dotvec.clear();

    this->upperPillar_height = generateRandomPillarHeight();
    this->lowerPillar_height = generateRandomPillarHeight();

    for (int i = 0; i < upperPillar_height; i++) {
      this->pushDot(i, this->spawnColumn);
    }

    for (int i = 0; i < upperPillar_height; i++) {
      this->pushDot(7 - i, this->spawnColumn);
    }
  }

  void pillarDebug(bool alsoDebugPattern = false) {
    Serial.print("upperpillarheight: ");
    Serial.print(this->upperPillar_height);
    Serial.print("  lowerpillarheight: ");
    Serial.print(this->lowerPillar_height);
    Serial.println();
    Serial.print("random num:  ");
    Serial.print(generateRandomPillarHeight());
    Serial.println();
    if (alsoDebugPattern = false) {
      patternDebug();
    }
    Serial.println();
  }
  

};

struct pillarframe {
  std::list<pillar *> pillarList;
  int horizontalDistance;           // Distance between Pillars

  pillarframe () {
    pillarList.push_back(new pillar(down));
    this->horizontalDistance = 3;
  }

  pillarframe (int iHorizontalDistance) {
    pillarList.push_back(new pillar(down));
    this->horizontalDistance = iHorizontalDistance;
  }

  void drawFrame(bool blankScreen) {
    if (blankScreen) {blank_screen();}

    for (auto & i : pillarList) {
      i->drawPattern(false);
    }
  }

  void hideFrame() {
    for (auto & i : pillarList) {
      i->hidePattern();
    }
  }

  void spawnPillar() {
    pillarList.push_back(new pillar(down));
  }

  void checkPillars() {
    if (pillarList.front()->dotvec[0]->col < 0 ) {                                // Check if left most pillar is out of bounds
      delete pillarList.front();                                                 // Deallocate heap memory
      pillarList.pop_front();                                                   // If so, despawn
    }

    if (pillarList.back()->dotvec[0]->col <= (7 - this->horizontalDistance)) {    // If horizontalDistance has been reached,
      spawnPillar();                                                            // spawn new pillar
    }
  }

  void shift(shiftOptions option, int distance) {
    for (auto & i : pillarList) {
      i->shift(option, distance);
    }
  }

  void scroll(shiftOptions option, int distance) {
    shift(option, distance);
    checkPillars();
    drawFrame(true);  
  }

};

struct gameloop {
  bool breakRequest;
  int circledot_start_time;
  int circledot_state;
  dot dot1(2,2);

  gameloop() : breakRequest(false), circledot_start_time(0), circledot_state(0) {}
  ~gameloop() {}

  bool checkTimePassed(int lastrecordedtime, int timePassed) {
    if ( (millis() - lastrecordedtime) >= timePassed ) {return true;}
    else {return false;}
  }

  void circledot(dot & input_dot, int delayMillis) {
    

    //if (checkTimePassed(start_time, 400)) {
      if (circledot_state == 0 && checkTimePassed(circledot_start_time, delayMillis)) {
        input_dot.move(down, 1);
        circledot_start_time = millis();
        circledot_state++;
        return;
      }
      else if (circledot_state == 1 && checkTimePassed(circledot_start_time, delayMillis)) {
        input_dot.move(left, 1);
        circledot_start_time = millis();
        circledot_state++;
        return;
      }
      else if (circledot_state == 2 && checkTimePassed(circledot_start_time, delayMillis)) {
        input_dot.move(up, 1);
        circledot_start_time = millis();
        circledot_state++;
        return;
      }
      else if (circledot_state == 3 && checkTimePassed(circledot_start_time, delayMillis)) {
        input_dot.move(right, 1);
        circledot_start_time = millis();
        circledot_state = 0;
        return;
      }
    //}
    else {return;}
  }

  void run() {
    while (!breakRequest) {
      circledot(dot1, 500);

      
 
    }
  }


};

struct bird {
  dot bird_dot;

  bird () {
    bird_dot.setDot(2, 2);

  }

  ~bird() {}
};


void readButtons() {
  // Read Buttons
  buttonState0 = digitalRead(buttonpinA);
  buttonState1 = digitalRead(buttonpinB);

  if (buttonState0 == HIGH) {
    turnOn(0,7);

    // Sticky Button
    while (buttonState0 == HIGH) {
      buttonState0 = digitalRead(buttonpinA);
      Serial.println("holding button A");
    }
    turnOff(0,7);
    }

    if (buttonState1 == HIGH) {

    // Sticky Button
      while (buttonState1 == HIGH) {
      buttonState1 = digitalRead(buttonpinB);
      Serial.println("holding button B");
    }
  }
}

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

  // Initialize Button A & B
  pinMode(buttonpinA, INPUT);
  pinMode(buttonpinB, INPUT);

  blank_screen();

  
  
  
  

}




void loop () {

  

  
}