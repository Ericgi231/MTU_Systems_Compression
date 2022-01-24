#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions

//unique id to represent eof
extern unsigned short eofID;

/* main - czy compression implementation
 * Develop a program called czy which compresses the data stream directed 
 * at its standard input and writes the compressed stream to its standard 
 * output.
 *
 * The compression algorithm reads the input one symbol (i.e., byte) at a 
 * time and compares it with each of the 8 bytes previously seen.  It also 
 * checks to see if the following n characters are the same as the current 
 * symbol.  If the byte has been previously seen, it outputs the position of 
 * the previous byte relative to the current position as well as the number 
 * of times it occurs using the format described below. Otherwise, the symbol 
 * is output as is by prefixing it with a binary one.
 *
 * To compile czy: make czy
 * To execute: ./czy < somefile.txt > somefile.encoded
 */
int main(int argc, char *argv[]){
	//misc vars
	unsigned char i = 0;
	unsigned short c = 0;
	
	//used to hold psn between loops
	unsigned short psnHold = eofID;
	//holds count of repeat characters
	unsigned short count = eofID;

	//buff stores 8 bytes
	unsigned long buff;
	//buffSize used to handle initial boundry condition
	unsigned char buffSize = 0;

	//match stores 8 bits, representing matches between c and buff
	unsigned char match = 0;

	while (c != eofID) {
		//read a byte
		c = readByte();

		//compare byte to last 8
		match = 0;
		//printf("Checking %c:\n", c);
		for (i = buffSize; i > 0; i--) {
			//shift match
			match <<= 1;
			//printf("%c", (unsigned char)(buff >> (8*(i-1))));
			//if byte checked is equal
			if (c == (unsigned char)(buff >> (8*(i-1)))) {
				//mark as first match
				match |= 1;
			}
		}
		//printf("\n%d\n", match);

		//increase buffer size up to 8
		if (buffSize < 8) {
			buffSize++;
		}

		//shift top byte off buffer
		buff <<= 8;
		//add new byte to end of buffer
		buff |= c;

		//handle situation where character is being held waiting to see if repeating
		if (psnHold != eofID) {
			//character matches previous, increment counter and move to next character
			if ((match & 1) == 1 && match < 7) {
				count == eofID ? count = 0 : count++;
			}
			//character does not match previous, wrap up hold then continue
			else {
				//write flag for repeating
				count == eofID ? writeBit(0) : writeBit(1);
				// count == eofID ? printf("0 ") : printf("1 ");

				//write psn
				writeBit((psnHold>>2)&1);
				writeBit((psnHold>>1)&1);
				writeBit((psnHold)&1);
				// printf("%d", (psnHold>>2)&1);
				// printf("%d", (psnHold>>1)&1);
				// printf("%d ", (psnHold)&1);
				// printf("\n");

				//repeating found
				if (count != eofID){
					writeBit((count>>2)&1);
					writeBit((count>>1)&1);
					writeBit((count)&1);
					// printf("%d", (count>>2)&1);
					// printf("%d", (count>>1)&1);
					// printf("%d", (count)&1);
				}
				//printf("\n");

				//reset count and psnHold
				count = eofID;
				psnHold = eofID;
			}
		}

		//break on end of file after solving any holds.
		if (c == eofID) {
			break;
		}

		// Infrequent - character does not match any in buffer
		if (match == 0) {
			//write bit representing recent, followed by character
			writeBit(1);
			writeByte(c);
			// printf("1 ");
			// printf("%c\n", c);
		}
		// Recent - character has match in buffer
		// not currently holding from a previous character
		else if (psnHold == eofID) {
			//write bit representing recent
			writeBit(0);
			//printf("0 ");
			//place a psnHold, marking that a character is waiting to see if it repeats
			//printf("%c\n", c);
			for (i = 0; i < 8; i++) {
				//printf("%d\n", (match & 1));
				if ((match & 1) == 1) {
					psnHold  = i;
					break;
				}
				match >>= 1;
			}
		}
	}

	flushWriteBuffer();
	return 0;
}
