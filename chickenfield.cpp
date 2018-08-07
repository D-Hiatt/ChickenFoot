//
/// chickenfield.cpp
//
//  Created on: Jan 9, 2015
//	Updated on: Jan 23, 2015
//      Author: David Hiatt
//
///	CS202 - Program 2 - chickenfoot
/// The [Doublebone] class
//		This class is used internally by the [Field] class.
//		It tracks the last double played, it's location, and number of locked plays left
//
//
///	The [Field] class
//		This class manages the relationship between individual bones without the overhead
//		The bone class is converted into its two values and then discarded.
//		Only two (unsigned char) values are stored.
/// Call Summary:
//		Entry point		->	(PlayBone)
//		(PlayBone)		->	(Check)
//		(Check)			->	(Open, TryPlace, Place)
//		(TryPlace)		->	(Adjacent)
//		(Adjacent)		-> 	(Open)
//		(Place)			->	(UpdateCell)
//		(UpdateCell)	->	(UpdateCell+Overload, UpdatePerimeter)
///	Overview:
//		A bone (or value of a bone) is passed in via (PlayBone).
//		Those two values are then passed to the private function (Check)
//		The (Check) function searches the map for a valid placement.  This is the bulk of the class
//		Any time (Check) finds a cell that matches one of the two values, it calls (Open)
//		The (Open) function determines if the match has any open edges.
//			If (Open) returns true, (Check) will call next (TryPlace)
//			The (TryPlace) function validates that the bone's non-matching edge can fit near the match
//				If (TryPlace) returns true, then (Check) will terminate by calling (Place)
//				The (Place) function copies the values to the board and updates the map.
//
////

#include "chickenfield.h"

#include <sys/types.h>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "chickencoop.h"
#include "chickentemplate.h"



using namespace std;


/* [Doublebone] Start */

/// [Doublebone] Static Data (Index)
unsigned short int doublebone::index = 0;


/// [Doublebone] Constructor (Default)
//	doublebone::doublebone()
///
doublebone::doublebone() : value(0), left(0), loc1(-1), loc2(-1), id(index++)
{
}

/// [Doublebone] Constructor (Custom)
//	doublebone::doublebone(unsigned char, bool)
//		Creates a [Doublebone] with a specific value;
///	Arguments:
//		unsigned char @val	|	The value of the [Doublebone]
///
doublebone::doublebone(unsigned char val) : value(val), left(3 + (index == 0)), loc1(-1), loc2(-1), id(index++)
{
}

/// [Doublebone] Operator (Compare)
//	bool doublebone::operator > == >=(const node&) const
//		Compares this object to another by @id
///	Type Safety:
//		These are all overridden virtual function operators.
//		The first expression compares (const node&) to (const node*)
//			which calls (bool node::operator ==(const node*) const.
//			That function performs the RTTI typeid verification,
//
///
bool doublebone::operator >(const node &other) const
{ return other == this && id > ((doublebone&)other).id; }
bool doublebone::operator ==(const node &other) const
{ return other == this && id == ((doublebone&)other).id; }

/// [Doublebone] Function (Display)
//	doublebone::display() const
///	Purpose:
//		Displays the lock being enforced by this [Doublebone]
///
void doublebone::display() const
{
	std::cout << endl << "Any bone played must match " << (int)value << " for " << (int)left << " more turns." << std::endl;
}

/// [Doublebone] Function (IsMatch)
//	bool doublebone::ismatch(unsigned char) const
/// Purpose:
//		Compares the argument @value to the lock on this [Doublebone]
///
bool doublebone::ismatch(unsigned char val) const
{ return val == value;}
//bool doublebone::ismatch(unsigned char *val) const
//{ return ismatch(val[0]) || ismatch(val[1]);}
bool doublebone::ismatch(unsigned char v1, unsigned char v2) const
{ return ismatch(v1) || ismatch(v2); }


/// [Doublebone] Function (AddLink)
//	bool doublebone::addlink()
/// Purpose:
//		Decrements the lock
///
bool doublebone::addlink()
{
	return ((int)--left) <= 0 || value == 0;
}

/// [Doublebone] Function (SetLoc)
//	void doublebone::setloc(unsigned short int, unsigned short int)
/// Purpose:
//		Sets this object's location on the board
///
void doublebone::setloc(unsigned short int r1, unsigned short int r2)
{
	if(loc1 < 0)
		loc1 = r1;
	if(loc2 < 0)
		loc2 = r2;
}


/// [Doublebone] Function (GetIndex)
//	unsigned int doublebone::getindex() const
/// Purpose:
//		Returns both edge locations encoded as an integer
///
unsigned int doublebone::getindex() const
{
	return (loc1 << 12) | loc2;
}

/// [Doublebone] Destructor
doublebone::~doublebone()
{
	value = left = loc1 = loc2 = 0;
}

/* [Doublebone] End */



/* [Field] Public START */

/// [Field] Static Data (Cheat)
field* field::cheat = NULL;

//// [Field] Constructor (Sized)
//	 field::field(unsigned char)
///	Purpose:
//		Initializes the size of the map.
///	Variables:
//		@size is set to the argument [@max] multiplied by two
//		@map  is then dynamically allocated with [@size * @size + @size] elements
////
field::field(unsigned char max) : size(max << 1), topend(0), lowend(0), lend(0), rend(0), cheatmode(false)
{
	map = new int[size * (size + 1)];
	reset();
	cheat = this;
}


//// [Field] Function (Reset)
//	void field::reset()
///	Purpose:
//		Sets every value back to 0
//		Clears the board and resets the size of the map
///
void field::reset()
{
	if(map && size)
	{
	for(int i = 0; i < size * (size + 1); i++)
		map[i] = 0;
	}
	topend = lend = size + 1;
	lowend = rend = -1;
	cheatmode = false;
	clear();
}


//// [Field] Function (PlayBone)
//	bool field::playbone(const unsigned char*)
///	Purpose:
//		The only public entry point
//		This will be called by the [Game] class when a [Player] decides to play a bone
/// Note:
//		The bone is never actually received.  It is converted into its values as an argument
////
bool field::playbone(const unsigned char *values)
{
	if(!values)
		return false;
	unsigned char top = values[0];
	unsigned char bottom = values[1];
	return check(top, bottom);
}

/* [Field] Public END */



/* [Field] Private START */

//// [Field] Function (StoreLock)
//	void field::storelock(doublebone*&)
///	Purpose:
//		Adds a new lock to the list
///
void field::storelock(doublebone *&lock)
{
	enqueue(lock);
	sort();
	lock = NULL;
}


//// [Field] Function (CheckLocks)
//	bool field::checklocks(unsigned char, unsigned char, doublebone*&)
///	Purpose:
//		Checks if there is a lock in the list.
//			If there is, compare the two edge values for a match
//				Finally, the lock is returned if a match is found.
//				Otherwise, the lock is put back.
///
bool field::checklocks(unsigned char top, unsigned char bottom, doublebone *&lock)
{
	bool ret = false;
	if(dequeue(lock) && lock)
	{
			if(lock->ismatch(top, bottom))
				ret = true;
			else
			{
				storelock(lock);
			}
	}
	return ret || (count() < 1);
}


//// [Field] Function (Reset)
//	bool field::icheat(unsigned char, unsigned char)
///	Purpose:
//		Use by (Autoplay) to find a [Bone] to play as an automatic [Player]
///
bool field::icheck(unsigned char top, unsigned char bottom)
{
	cheat->cheatmode = true;
	bool ret = cheat->check(top, bottom);
	cheat->cheatmode = false;
	return ret;
}

//// [Field] Function (Check)
//	bool field::check(unsigned char, unsigned char)
///	Purpose:
//		Compares both [@top, @bottom] to every value on the board.
//		When a match is found, consider down the call hierarchy until a valid placement has been found
//		~See [Field] Overview
/// Variables:
//		@check		| A bitmask flag used to match the first value
//						A flag at |0001| indicates an opening to the (Right)
//						A flag at |0010| indicates an opening to the (Left)
//						A flag at |0100| indicates an opening		 (Above)
//						A flag at |1000| indicates an opening		 (Below)
//		@second		| A bitmask flag used to match the first value
//						Uses the same flag structure as @check
//
/// Extract standalone functions to break this guy up.
////
bool field::check(unsigned char top, unsigned char bottom)
{
	short match = -1;
	if(lowend == match)									//First move
	{
		if(top != bottom)
			return false;
		match = (size * (size + 1)) / 3;				//Place it in the center of the board
		if(cheatmode)
			return true;
		return place(match, match + 1, top, bottom);
	}
	doublebone *locker = NULL;
	if(!checklocks(top, bottom, locker))				//Check for any [Doublebone]s and matches
	{
		storelock(locker);
		locker = NULL;
		return false;
	}
	if(locker)				//----> Loop during a [Doublebone] lock
	{
		unsigned char x, y;
		unsigned int area = locker->getindex();
		getcell(area & 0xFFF, x, y);				//Get the location of the first edge
		if(checkmatch(x, y, (locker->ismatch(top)) ? top : bottom, (locker->ismatch(top)) ? bottom : top))
		{
			if(!locker->addlink())				//Match was found, decrement the lock
				storelock(locker);
			else
				delete locker;
			locker = NULL;
			return true;
		}
		getcell(area >> 12, x, y);					//Get the location of the second endge
		if(checkmatch(x, y, (locker->ismatch(top)) ? top : bottom, (locker->ismatch(top)) ? bottom : top))
		{
			if(!locker->addlink())
				storelock(locker);
			else
				delete locker;
			locker = NULL;
			return true;
		}
		storelock(locker);
		locker = NULL;
		return false;
	}

	for(short i = topend; i <= lowend; i++)				//Loop from top to bottom
	{
		for(short ii = lend; ii <= rend; ii++)			//And left to right
		{
			if((map[(i * size) + ii] == (int)top))		//Match found, check for adjacent openings
			{
				if(checkmatch(i, ii, top, bottom))
					return true;
			}
			if(top != bottom && (map[(i * size) + ii] == (int)bottom))		//Match found, check for adjacent openings
				{
					if(checkmatch(i, ii, bottom, top))
						return true;
				}
		}
	}
	return false;
}

//// [Field] Function (CheckMatch)
//	bool field::checkmatch(unsigned char, unsigned char, unsigned char, unsigned char)
///	Purpose:
//		Searches the region around the selected row and column for a match
//		It is called after one edge has been matched with an existed value on the board.
//			This method then determines if the second edge of @match will fit and be valid
//			for any alignment
//
///	Arguments:
//	unsigned char @row		|	The row of the previous @match
//	unsigned char @col		|	The column of the previous @match
//	unsigned char @match	|	A matched value on the board
//	unsigned char @nonmatch	|	The opposite edge of the match
///
bool field::checkmatch(unsigned char row, unsigned char col, unsigned char match, unsigned char nonmatch)
{
	signed char check = open(row, col);
	bool ret = false;
	if(check > 0)	//There is an opening, check for one more for the other side
	{
		short int loc = 0;
		short int base = (short int)row * size + col;					//Convert to a single integer location
		if((check & 1) && col < size)								//Indicates an opening to the right
		{
			loc = matchnear(row, col + 1, nonmatch, 0xFF ^ (check << 1));
		}
		if(!ret && loc !=0)
			ret = place(base + 1, base + 1 + loc, match, nonmatch);
		if(!ret && (check & 2) && col > 0)							//Indicates an opening to the left
		{
			loc = matchnear(row, col - 1, nonmatch, 0xFF ^ (check >> 1));
		}
		if(!ret && loc !=0)
			ret = place(base - 1, base - 1 + loc, match, nonmatch);
		if(!ret && (check & 4) && row > 0)							//Indicates an opening above
		{
			loc = matchnear(row - 1, col, nonmatch, 0xFF ^ (check << 1));
		}
		if(!ret && loc !=0)
			ret = place(base - size, base -size + loc, match, nonmatch);
		if(!ret && (check & 8) && row < size)						//Indicates an opening below
		{
			loc = matchnear(row + 1, col, nonmatch, 0xFF ^ (check >> 1));
		}
		if(!ret && loc !=0)
			ret = place(base + size, base +size + loc, match, nonmatch);
	}
	return ret;
}

//// [Field] Function (MatchNear)
//	short int field::checkmatch(unsigned char, unsigned char, unsigned char, unsigned char)
///	Purpose:
//		This method only checks for conflicting values.
//		Any surrounding value that does not equal @match and is not open indicates a blocked cell
//
///	Arguments:
//	unsigned char @row		|	The row of the previous @match
//	unsigned char @col		|	The column of the previous @match
//	unsigned char @match	|	A matched value on the board
//	unsigned char @mask		|	A bitmask array of adjacent values matching @match
///
short int field::matchnear(unsigned char row, unsigned char col, unsigned char match, signed char mask) const
{
	signed char check = open(row, col) & mask;
	if(check > 0)
	{
		if(col < size && tryplace((check & 1), row, col, match))
		{
			return 1;
		}
		if(col < size && tryplace((check & 2), row, col, match))
		{
			return -1;
		}
		if(row > 0 && tryplace((check & 4), row, col, match))
		{
				return -size;
		}
		if(row < size && tryplace((check & 8), row, col, match))
		{
			return size;
		}
	}
	return 0;
}

//// [Field] Function (Open)
//	signed char ::field::open(unsigned char, unsigned char)
///	Purpose:
//		Checks all adjacent cells for an opening
/// Returns:	@side as a bitmask
//		A flag at |0001| indicates an opening to the (Right)
//		A flag at |0010| indicates an opening to the (Left)
//		A flag at |0100| indicates an opening		 (Above)
//		A flag at |1000| indicates an opening		 (Below)
////
signed char field::open(unsigned char row, unsigned char col) const
{
	short loc = (row * size) + col;
	signed char side = -((row | col) > size || map[loc] < 0);
		if(col < size)									//Check right
		side |= ((map[loc + 1] == 0));
		if(col > 0)										//Check left
		side |= ((map[loc - 1] == 0) << 1);
		if(row > 0)										//Check above
		side |= ((map[loc - size] == 0) << 2);
		if(row < size)									//Check below
		side |= ((map[loc + size] == 0) << 3);
		return side;
}



//// [Field] Function (TryPlace)
//	bool ::field::tryplace(signed char, unsigned char, unsigned char, unsigned char)
///	Purpose:
//		This is called after a location for the first value was found with a nearby open cell
//			(TryPlace) calls (Adjacent) to ensure that there are no conflicts next to the open cell
/// Arguments:
//		@type		|	The direction of a nearby open cell to validate
//		@row, @col	|	The location of the matched bone value
//		@value		|	The non-matched bone value
////
bool field::tryplace(signed char type, unsigned char row, unsigned char col, unsigned char value) const
{
	bool valid = false;
	switch(type)
	{
		case 1:		valid = adjacent(row, col + 1, -1, value);//right
			break;
		case 2:		valid = adjacent(row, col - 1, 1, value);//left
			break;
		case 4:		valid = adjacent(row - 1, col, size, value);//up
			break;
		case 8:		valid = adjacent(row + 1, col, -size, value);//down
			break;
		default:	break;
	}
	return valid;
}


//// [Field] Function (Adjacent)
//	bool field::adjacent(unsigned char, unsigned char, short int, unsigned char)
///	Purpose:
//		Checks that every cell bordering the cell at (@row * @size + @col)
//			is either open (0) or a matching value
/// Arguments:
//		@row, @col	|	These represent the requested placement location
//		@offset		|	This number indicates the direction of the matching value triggering the placement
//						(Adding the offset to the absolute location gives the location of the match)
//						Left == -1
//						Right == 1
//						Up == -@size (< -1)
//						Down == @size (> 1)
/// Variables:
//		@side		|	A bitmask listing open edges with either a 1 or 0.
////
bool field::adjacent(unsigned char row, unsigned char col, short int offset, unsigned char match) const
{
	signed char side = open(row, col);				//Get a list of each open edge
	if(col < size)									//Check the right side.  It must either
		side |= ((map[(row * size) + col + 1] == match) || offset == 1 || (map[(row * size) + col + 1] < 0));
	//left
	if(col > 0)
		side |= (((map[(row * size) + col - 1] == match)  || offset == -1 || (map[(row * size) + col - 1] < 0)) << 1);
	//up
	if(row > 0)
		side |= (((map[((row - 1) * size) + col] == match) || offset < -1 || (map[((row - 1) * size) + col] < 0)) << 2);
	//down
	if(row < size)
		side |= (((map[((row + 1) * size) + col] == match) || offset > 1 || (map[((row + 1) * size) + col] < 0)) << 3);
	return side == 0xF;
}


//// [Field] Function (Place)
//	bool field::place(short int, short int, unsigned char, unsigned char)
///	Purpose:
//		Copies the values into the board and updates the map.
//		The location and values are assumed to be valid at this point, no checking is done.
////
bool field::place(short int base, short int other, unsigned char top, unsigned char bottom)
{
	if((base | other) > size * (size + 1) || (base | other) < 0 || (map[base] | map[other]) != 0)
		return false;
	if(cheatmode)
		return true;
	doublebone *current = NULL;
	if(top == bottom)					//If this item was a double, store it
		current = new doublebone(top);
	map[base] = top;
	updatecell(base);
	map[other] = bottom;
	updatecell(other);
	if(current)
	{
		current->setloc(base, other);
		storelock(current);
	}
	current = NULL;
	return true;
}


//// [Field] Function (UpdateCell)
//	void field::updatecell(short int)
///	Purpose:
//		This will be called immediately after placing a bone.
//		It will then call the overloaded (UpdateCell(row, col)) on every adjacent cell
//		Finally, it calls (UpdatePerimeter) to adjust the size of the map
////
void field::updatecell(short int loc)
{
	unsigned char row = 0u;
	unsigned char col = 0u;
	getcell(loc, row, col);							//Convert the cell value into a row and column
	if(col < size)									//These statements are needed because [@map] has a fixed size
		updatecell(row, col + 1);
	if(col > 0)
		updatecell(row, col - 1);
	if(row < size)
		updatecell(row + 1, col);
	if(row > 0)
		updatecell(row - 1, col);
	updateperimeter(row, col);
}


//// [Field] Function (UpdateCell, Overload)
//	void field::updatecell(unsigned char, unsigned char)
///	Purpose:
//		Called sequentially after placing a bone.
//		The cell [@row * @size + @col] should be a cell directly adjacent the bone.
//			If this cell is open (0 value), then every cell next to it must either
//			be open (0 value) OR the value of bone.
//		If a cell borders two DIFFERENT values, then nothing could ever fit there.
/// Variables:
//		@value	|	An array of values immediately adjacent to the current cell.
//			value[0] is set to the cell (Left) of current
//			value[1] is set to the cell (Right) of current
//			value[2] is set to the cell (Above) current
//			value[3] is set to the cell (Below) current
//		@maxval	|	The maximum value of the above array
////
void field::updatecell(unsigned char row, unsigned char col)
{
	short int loc = (row * size) + col;								//Convert row and column to a single value
	if(map[loc] != 0)
		return;

	short int value[4] = {-1, -1, -1, -1};
	value[0] =  (col > 0) 			? map[row * size + (col - 1)] : 0;		//0  Left side
	value[1] = (col < size) 	? map[row * size + (col + 1)] : 0;			//1  Right side
	value[2] = (row > 0) 			? map[(row - 1) * size + col] : 0;		//2  Up side
	value[3] = (row < size) 	? map[(row + 1) * size + col] : 0;			//3  Down side

	unsigned short int maxval = maxcell(value);								//Get the max

	for(int i = 0; i < 4; i++)									//Consider all open cells to be the same as the max value
	{
		if(value[i] <= 0)
			value[i] = maxval;
	}
	for(int i = 1; i < 4; i++)									//If anything does not now equal the maxval, the cell is dead
	{
		map[loc] -= (value[i] != value[i - 1]);
	}
}


//// [Field] Function (UpdatePerimeter)
//	void field::updateperimeter(unsigned char, unsigned char)
///	Purpose:
//		Adjusts the size of the field to fit the row and column sizes of a newly placed bone
////
void field::updateperimeter(unsigned char row, unsigned char col)
{
	unsigned short int x = (ushort)row;				//Converted to int due to comparison between literals and char
	unsigned short int y = (ushort)col;
	if(lend >= y)									//Left side is decreased if needed
		lend = y - ((y > 0) ? 1 : 0);
	if(rend <= y)									//Right side is increased if needed
		rend = y + ((y < size) ? 1 : 0);
	if(topend >= x)									//The top is decreased if needed
		topend = x - ((x > 0) ? 1 : 0);
	if(lowend <= x)									//The bottom is increased if needed
		lowend = x + ((x < size) ? 1 : 0);
}


//// [Field] Function (GetCell)
//	void field::getcell(short int, unsigned char&, unsigned char&)
///	Purpose:
//		A stub to quickly convert the actual index of an element into a row and column.
////
void field::getcell(short int loc, unsigned char &row, unsigned char &col) const
{
	if(loc > 0 && loc < (size * (1+size)))
	{
		row = (unsigned char)(((int)floor(loc / size)) & 0xFF);
		col = (unsigned char)(((int)(loc % size)) & 0xFF);
	}
}


//// [Field] Function (DisplayLoc)
//	void field::displayloc(unsigned char, unsigned char)
///	Purpose:
//		Displays a cell location if it has a value, otherwise it will display a placeholder
////
void field::displayloc(unsigned char row, unsigned char col) const
{
	if(col < 1)									//Should mark the final edges of the board
		highlight('|');
	else
		cout << '|';
	if(map[(row * size) + col] != 0)
		highlight(map[(row * size) + col]);
	else if(row >= size)
		highlight('_');
	else
		cout << '_';
	if(col >= size)								//Should mark the final edges of the board
		highlight('|');
	else
		cout << '|';
}

//// [Field] Function (Display)
//	void field::getcell() const
///	Purpose:
//		Displays the current lock or map.
////
void field::display() const
{
	if(count() > 0)
		view();
	else
		displaymap();
}

//// [Field] Function (Display)
//	void field::display()
///	Purpose:
//		Displays the current field of all bones
///
void field::displaymap() const
{
	if(topend < 0)
		cout << "The game has not started yet" << endl;
	else
	{
		cout << " ";
		for(int i = lend; i <= rend; i++)						//@lend and @right represent the horizontal edges of the field
			cout << " " << i << " ";					//This loop displays the column number above each column
		for(int i = topend; i <= lowend; i++)					//@topend and @lowend represent the vertical edges
		{
			cout << endl << i;
			for(int ii = lend; ii <= rend; ii++)
				displayloc(i, ii);
			cout << std::flush;
		}
	}
}


/// [Field] Destructor
field::~field()
{
	if(map)
	{
		if(topend >= 0)
		{
		for(int i = topend; i < lowend; i++)
			for(int ii = lend; ii < rend; ii++)
				map[(i * size) + ii] = 0;
		}
		delete[] map;
	}
	map = NULL;
	lend = rend = lowend = topend = 0;
}

/* [Field] End */

/* chickenfield.cpp End */
