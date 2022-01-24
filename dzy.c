#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions

extern unsigned short eofID;
//buff stores 8 bytes
unsigned long buff = 0;

// Writes a byte to the standard out and pushes to buffer
//
void writeByteWithBuffer(unsigned short byt) {
	//write byte to out
	writeByte(byt);
	//shift top byte off buffer
	buff <<= 8;
	//add new byte to end of buffer
	buff |= byt;
}

// Reads three bits into an unsigned short and returns
//
unsigned short readThreeBit(){
	unsigned short psn = 0;
	unsigned short bit;
	for (unsigned char i = 0; i < 3; i++) {
		bit = readBit();
		if (bit == eofID) {return eofID;}
		psn |= bit;
		if (i != 2) {psn <<= 1;}
	}
	return psn;
}

/* main - dzy de-compression implementation
 * This program decompresses a compressed stream directed at its standard input 
 * and writes decompressed data to its standard output.
 *
 * To compile dzy: make dzy
 * To execute: ./dzy < somefile.encoded > somefile_decoded.txt
 */
int main(int argc, char *argv[]){
	unsigned short bit = 0;
	unsigned short byt = 0;
	//also used to store count
	unsigned short psn = 0;
	while (bit != eofID) {
		//read bit
		bit = readBit();
		if (bit == eofID) {break;}

		//read flag bit, check if unique
		if (bit == 1) {
			//read and write byte
			byt = readByte();
			if (byt == eofID) {break;}
			writeByteWithBuffer(byt);
		}
		//is recent, read flag bit, check if is not repeating 
		else {
			//read in bit
			bit = readBit();
			if (bit == eofID) {break;}

			//read in psn
			psn = readThreeBit();
			if (psn == eofID) {break;}

			//look back psn into buffer, then write byte
			byt = (buff>>(psn*8))&0xff;
			writeByteWithBuffer(byt);

			//is repeating, print character repeating
			if (bit == 1) {
				psn = readThreeBit();
				if (psn == eofID) {break;}

				for (unsigned char i = 0; i <= psn; i++) {
					writeByteWithBuffer(byt);
				}
			}
		}
	}
	
	flushWriteBuffer();
	return 0; //exit status. success=0, error=-1
}
