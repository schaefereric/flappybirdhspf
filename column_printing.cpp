#include <iostream>
#include <cstdio>

enum byteOrder {lsb, msb};
bool field[8][8]; 


void drawField() {
	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			if (field[row][column] == false) {
				std::cout << " O" ;
			}
			if (field[row][column] == true) {
				std::cout << " X" ;
			}
		}
		std::cout << std::endl;
	}
}

void clearField() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			field[i][j] = false;
		}
	}
}

int importColumn(uint8_t data, int column, byteOrder byteorder = msb) {

    if (byteorder == msb) {
      for (int row = 0; row < 8; row++) {             // MOST significant bit first
        bool isBitSet = (data & (1 << 8 - 1 - row)) != 0;
        
        if (isBitSet) {
          field[row][column] = true;
        } 
      }
      return 1;
    }

    else if (byteorder == lsb) {
       for (int row = 0; row < 8; row++) {          // LEAST significant bit first
        bool isBitSet = (data & (1 << row)) != 0;   // Die "1" wird "row"-mal nach links gebitshifted. Das Bit an der Stelle (restliches byte wird mit Nullen aufgefuellt)
                                                    // wird dann mit dem Byte verundet. Wenn die gepruefte Stelle 0 ist, kommt 0 raus. Sonst ungleich 0.
        if (isBitSet) {
          field[row][column] = true;
        } 
      } 
      return 1;
    }

    else {return 0;}
  }

void importColumnArray(uint8_t bytearray[8], byteOrder byteorder = msb) {
    for (int col = 0; col < 8; col++) {
      importColumn(bytearray[col], col, byteorder);
    }
  }


int main() {
	
	uint8_t byte = 0x58;
	uint8_t bytearray[8] = {0x00,0x7E,0x52,0x52,0x52,0x52,0x2C,0x00};
	
	uint8_t bytearray_A[8] = {0x02,0x0C,0x18,0x68,0x68,0x18,0x0C,0x02} ; 
	uint8_t bytearray_B[8] = {0x00,0x7E,0x52,0x52,0x52,0x52,0x2C,0x00} ; 
	uint8_t bytearray_C[8] = {0x00,0x3C,0x66,0x42,0x42,0x42,0x2C,0x00} ; 
	uint8_t bytearray_D[8] = {0x00,0x7E,0x42,0x42,0x42,0x66,0x3C,0x00} ; 
	uint8_t bytearray_E[8] = {0x00,0x7E,0x52,0x52,0x52,0x52,0x52,0x42} ; 
	uint8_t bytearray_F[8] = {0x00,0x7E,0x50,0x50,0x50,0x50,0x50,0x40} ; 
	uint8_t bytearray_G[8] = {0x00,0x3C,0x66,0x42,0x42,0x52,0x16,0x1E} ; 
	uint8_t bytearray_H[8] = {0x00,0x7E,0x10,0x10,0x10,0x10,0x7E,0x00} ; 
	uint8_t bytearray_I[8] = {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00} ; 
	
	clearField();
	//importColumn(byte, 7, msb);
	importColumnArray(bytearray_A, msb);
	drawField();
	
	system("pause");
	return 0;
}