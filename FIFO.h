// FIFO.h
// Runs on any LM3Sxxx
// Provide functions that initialize a FIFO, put data in, get data out,
// and return the current size.  The file includes a transmit FIFO
// using index implementation and a receive FIFO using pointer
// implementation.  Other index or pointer implementation FIFOs can be
// created using the macros supplied at the end of the file.
// Daniel Valvano
// June 16, 2011

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
      Programs 3.7, 3.8., 3.9 and 3.10 in Section 3.7

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#ifndef __FIFO_H__
#define __FIFO_H__
#include <stdint.h>

#define FIFOSIZE 32  // maximum storage is FIFO_SIZE-1 elements 
class Queue{ 
private:
	char Buf[FIFOSIZE];
	int PutI; // index to an empty place, next place to put
  int GetI; // index to oldest data, next to get

public:
  Queue();            // initialize queue
  bool IsEmpty(void); // true if empty
  bool IsFull(void);  // true if full
  bool Put(char x);   // enter data into queue
  bool Get(char *pt); // remove data from queue
  void Print(void);   // display element of queue on LCD
};


#endif //  __FIFO_H__
