//
/// chickencoop.cpp
//
//  Created on: Jan 15, 2015
//      Author: David Hiatt
///	CS202 - Program 1 - chickenfoot
//
// Contains misc functions not related to any particular class.
///	[Non-class]
//		bool getinput(char*)			Sanitize user input
//		short int maxcell(short int*)	Returns max value
//		short int maxcell(short int*)	""
//		void highlight(int data)		Prints an escape code to highlight @data in white
//		void highlight(const char data)	""
////

#include "chickencoop.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "chickenfield.h"
#include "chickentemplate.h"


using namespace std;


//// [GetInput] Function
//	bool getinput(char*)
///	Purpose:
//		Returns @name with all symbols removed
//
///	Locals:
//	static char @buffer	|	Volatile memory to be manipulated
//	char @namelike		|	A small incrementer to prevent wasting resources on junk
//	char @pos			|	The current position in the buffer
//	int @size			|	The original size of the string
//	bool @space			|	A flag indicating that the previous character was a space
//
////
bool getinput(char *name)
{
	int namelike = 0;
	static char buffer[256];							//This method is called a lot, so keep the memory open
	if(name)
	{
		memset(buffer, '\0', 255);
		int size = strlen(name);
		if(size > 0)
		{
			memmove(buffer, name, 254);					//Copy most of the string over to work with.
			char *pos = buffer;
			bool space = true;
			for(; pos < buffer + size; ++pos)
			{
				if(*pos > 0x60 && *pos < 0x7B)			//----> Lowercase ASCII letters
				{
					if(space)
						*pos -= 0x20;
					space = false;
					++namelike;

				}
				else if (*pos > 0x40 && *pos < 0x5B)	//----> Capital ASCII letters
				{
					if(!space)
						*pos += 0x20;
					space = false;
					++namelike;
				}
				else if(*pos == 0x20 || *pos == ' ')	//----> Space ASCII letter
				{
					if(space)
						memmove(pos + 1, pos, strlen(pos));
					space = true;
				}
				else									//----> All other ASCII letters
				{
					--namelike;
                    if(*pos == 0x3F)
                    {
                        namelike = -1;
                        buffer[0] = '?';
                        buffer[1] = '\0';
                    }
                    else
                    {
				    	char *number =  NULL;
				    	if(strtoul(pos, &number, 10))				//If the string begins with a number, return the number
				    	{
					    	if(namelike < 0 && pos < number)
						    	memset(number, '\0', strlen(number));
					    	else
					    	{
					    		memmove(pos, number, number - pos);
					    		--pos;
					    	}

					    }
					    else
						    memmove(pos, pos + 1, strlen(pos) - 1);
					    number = NULL;
                    }

				}
				if(namelike < 0)
                {
                    pos = NULL;
					break;
                }

			}
            size = 0;
		}
		memset(name, '\0', strlen(name));
		strcpy(name, buffer);
	}
	return namelike > 0;
}

//void compnodes(node *&high, node *&low)
//{
//	if(high && low && high != low)
//	{
//		node *tmp = low;
//		if(compare(high, tmp) < 0)
//		{
//
//		}
//
//		low->gonext(tmp);
//		node *comp = low;
//		if(compare(comp, high))
//		{
//
//		}
//	}
//}
//void sortnodes(node *&high, node *&low)
//{
//	node *tmp = NULL;
//	if(high && low && high != low)
//	{
//		high->goprev(tmp);
//		if(tmp != low)
//		{
//
//		}
//	}
//}
//Turns (second<->first into first<->second)
//void reverse(node *&second, node *&first)
//{
//	if(first && second)
//	{
//		node *tmp = NULL;
//		first->gonext(tmp);
//
//		second->setnext(tmp);
//		first->setnext(second);
//
//		second->goprev(tmp);
//
//		first->setprev(tmp);
//		second->setprev(first);
//		tmp = NULL;
//	}
//}

//int compare(node *obj, node *comp)
//{
//	if(obj && !comp)
//		return 1;
//	if(comp && !comp)
//		return -1;
//	if(obj && comp)
//	{
//		if(*obj > *comp)
//			return 1;
//		if(*comp > *obj)
//			return -1;
//	}
//	return 0;
//}

//// [] Function (Autoplay)
//	void ::autoplay(bone*&)
///	Purpose:
//		Wrapper function for (Autoplay)
////
bool autoplay(bone *&card)
{
	node *tmp = dynamic_cast<node*>(card);
	return autoplay(tmp);
}

//// [] Function (Autoplay)
//	void ::autoplay(node*&)
///	Purpose:
//		Automatically selects and plays a card from a player's hand.
////
bool autoplay(node *&card)
{
	if(card)
	{
		static field *obj = field::cheat;						//Retrieve a pointer to the current [Field] object
		bone *start = NULL;
		card->gonext(start);
		if(start)										//-----> Executed only when there is more than one [Bone] to choose from
		{
			bone *tmp = start;
			if(obj)
			{
				if(obj->count())								//-----> This loop is executed only if the [Field] is locked by a double
				{
					doublebone *key = NULL;
					obj->peek(key);									//Retrieve the @key
					do
					{
						const unsigned char *arr = tmp->getvalues();
						if(key->ismatch(arr[0], arr[1]))			//Compare each [Bone] in the [Player]'s hand with the @key
						{
							card = tmp;
							tmp = NULL;								//Clear @tmp to exit the loop when a match is found
						}
						arr = NULL;
					}while(tmp && tmp != start);				//<----- Loop stops when the beginning is reached, or a match is found
					key = NULL;
				}
				else											//-----> [Field] was not locked
				{
					do
					{
						const unsigned char *arr = tmp->getvalues();
						if(field::icheck(arr[0], arr[1]))			//Call the internal [Field] checker to find matches.
						{
							card = tmp;
//							card->gonext(tmp);
//							card->goprev(start);
//
//							tmp->setprev(start);
//							start->setnext(tmp);
							tmp = start = NULL;

						}
						else
							tmp->gonext(tmp);
						arr = NULL;
					}while(tmp && tmp != start);
					tmp = NULL;
				}												//<----- End of ([Field] was not locked)
			}
		start = NULL;
		}
	}
	return card != NULL;
}

//// [MaxCell] Function
//	short int maxcell(short int*)
///	Purpose:
//		Returns the max of four values
////
short int maxcell(short int *array)
{
	return maxcell(maxcell(maxcell(array[0], array[1]), array[2]), array[3]);
}

short int maxcell(short int one, short int two)
{
	return (one > two ? one : two);
}


//// [Highlight] Function
//	void highlight(int)
///	Purpose:
//		Not used.  This will display a value with a bold font to highlight cells on DEC VT100-compatible terminals
/// Arguments:
//		@INT data	|	Displays a cell in (White) if it is greater than 0, otherwise (Red)
//		@CHAR data	|	Displays a singe character in (White)
//				Used to mark edges of the board
////
void highlight(int data)
{
	cout << "\e[0;" << (data > 0 ? 1 : 31) << "m" << abs(data) << "\e[0m";

}

void highlight(const char data)
{
	cout << "\e[01;m" << data << "\e[0m";
}

/* chickencoop.cpp END */
