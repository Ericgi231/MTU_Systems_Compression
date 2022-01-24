#include "bitsy.h"
#include <unistd.h>

//read byte buffer
unsigned char rBuff = 0;
//write byte buffer
unsigned char wBuff = 0;
//available read bits
unsigned char rAvail = 0;
//available write bits
unsigned char wAvail = 8;
//unique id to represent eof
unsigned short eofID = 300;

/* readByte
 * Abstraction to read a byte.
 * Relys on readBit.
 */
unsigned short readByte(){ 
	//read 8 bits one at a time
	unsigned short res = 0;
	for (unsigned char i = 0; i < 8; i++) {
		res <<= 1;
		res = res | readBit();
	}

	//return res or eof
	//potential issue, if reading not byte alligned and reached eof, could lose bits
	return res > 255 ? eofID : res;
}

/* readBit
 * Abstraction to read a bit.
 * */
unsigned short readBit(){
	//read a new buffer if no bits available
	if (rAvail == 0) {
		//read in one byte to buffer, return on eof
		if (read(0, &rBuff, 1) == 0) {
			return eofID;
		}
		//set available bits to a full byte
		rAvail = 8;
	}

	//get first bit from buff
	unsigned short c = rBuff >> 7;
	rBuff <<= 1;
	rAvail--;

	return c;
}

/* writeByte
 * Abstraction to write a byte.
 */
void writeByte(unsigned char byt){
	unsigned char bit;
	for (unsigned char i = 0; i < 8; i++) {
		//remove highest order bit and write it
		bit = byt >> 7;
		byt <<= 1;
		writeBit(bit);
	}
}

/* writeBit
 * Abstraction to write a single bit.
 */
void writeBit(unsigned char bit){
	//read bit to end of buffer
	wBuff <<= 1;
	wBuff |= bit;
	wAvail--;

	//write on buffer full
	if (wAvail <= 0) {
		wAvail = 8;
		write(1, &wBuff, 1);
	}
}

/* flushWriteBuffer
 * Helper to write out remaining contents of a buffer after padding empty bits
 * with 1s.
 */
void flushWriteBuffer(){
	//if bits are available to write
	if (wAvail != 8) {
		//write ones until buffer fills
		while (wAvail != 8) {
			writeBit(1);
			// printf("\nFlushed 1\n");
		}
	}
}
