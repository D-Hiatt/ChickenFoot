//
// chickencoop.h
//
//  Created on: Jan 16, 2015
//	Update on: Jan 30, 2015
//      Author: David Hiatt
//
///	CS202 - Program 2 - chickenfoot
//
// Contains misc functions not related to any particular class.
///	[Non-class]
//		bool autoplay(node*&)			Automatic play a bone for a non-user player
//		bool autoplay(bone*&)			Wrapper function
//		bool getinput(char*)			Sanitize user input
//		short int maxcell(short int*)	Returns max value
//		short int maxcell(short int*)	""
//		void highlight(int data)		Prints an escape code to highlight @data in white
//		void highlight(const char data)	""
////

#ifndef CHICKENCOOP_H_OLD_
#define CHICKENCOOP_H_OLD_
#include "chickenfoot.h"

bool autoplay(node*&);
bool autoplay(bone*&);
bool getinput(char*);
short int maxcell(short int*);
short int maxcell(short int, short int);
void highlight(const char);
void highlight(int);

#endif /* CHICKENCOOP_H_OLD_ */
