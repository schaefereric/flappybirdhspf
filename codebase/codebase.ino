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

void setup () {
  Serial.begin(9600);
  Serial.print("lol");
  Serial.println();

  int i = 0;
  for(i=2;i<18;i++)
  {
    pinMode(i, OUTPUT);
  }

  for(i=2;i<18;i++) {
    digitalWrite(i, LOW);
  }

  blank_screen();
}

void loop () {
  Serial.print("iteration"); 
  Serial.println();
  
  turnOn(0,0);
  //digitalWrite(row[0], HIGH);
  //digitalWrite(col[0], LOW);

  delay(1000); 
  
  //turn off all  
  //blank_screen();
  turnOff(0,0);
  
  delay(1000); 
}