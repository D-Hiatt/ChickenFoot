/*
 * chickenfield.h
 *
 *  Created on: Jan 9, 2015
 *  Updated on: Jan 23, 2015
 *      Author: David Hiatt
 *
 *      CS202 - Program 2 - chickenfoot
 *  -----------------------------------------
 *  The [Doublebone] class
 *  -----------------------------------------
 *  	This class is used internally by the [Field] class.
 *  	It is initialized using a single value from a double char array
 *  Type Hierarchy:
 * 		Inherits class [Node].
 *
 *	-----------------------------------------
 *	The [Field] class:	A linear matrix
 *	-----------------------------------------
 *		This class manages the relationship between individual bones without the overhead
 *		The bone class is converted into its two values and then discarded.
 *		Only two (unsigned char) values are stored.
 *
 * Type Hierarchy:
 * 		Inherits [Collection]
 *
 * Call Summary:
 *		Entry point		->	(PlayBone)
 *		(PlayBone)		->	(Check)
 *		(Check)			->	(Open, TryPlace, Place)
 *		(TryPlace)		->	(Adjacent)
 *		(Adjacent)		-> 	(Open)
 *		(Place)			->	(UpdateCell)
 *		(UpdateCell)	->	(UpdateCell+Overload, UpdatePerimeter)
 *
 *
 *	Overview:
 *		A bone (or value of a bone) is passed in via (PlayBone).
 *		Those two values are then passed to the private function (Check)
 *		The (Check) function searches the map for a valid placement.  This is the bulk of the class
 *		Any time (Check) finds a cell that matches one of the two values, it calls (Open)
 *		The (Open) function determines if the match has any open edges.
 *			If (Open) returns true, (Check) will call next (TryPlace)
 *			The (TryPlace) function validates that the bone's non-matching edge can fit near the match
 *				If (TryPlace) returns true, then (Check) will terminate by calling (Place)
 *				The (Place) function copies the values to the board and updates the map.
 *
 *	Doubles:
 *		A [Bone] passed in as a double will follow the same call hierachy described above.
 *		During the final stage (Place), a new object of [Doublebone] is initialized and stored
 *			in the [Field]'s inherited [Collection]
 *		The (PlayBone) will check if any [Doublebone] has been played, and whether it's argument
 *			matches the value of the restriction.
 *			After that, the (Check) function will only search the open tiles directly adjacent
 *				to the locked [Doublebone].
 *		The [Doublebone] is removed and deleted as soon as it's counter reaches zero.
 *
 */



#ifndef CHICKENFIELD_H_
#define CHICKENFIELD_H_

#include "chickenbase.h"


///-----------------------------------------------------------
//class doublebone : Stores information relating to any double value requirements
///-----------------------------------------------------------
class doublebone : public node
{
public:
    doublebone();
    doublebone(unsigned char);	//Custom constructor.
    ~doublebone();

    virtual void display() const;						//Overridden pure virtual.  Announces what value is locked, and for how long
    virtual bool operator >(const node&) const;    		//Overridden virtuals
    virtual bool operator ==(const node&) const;		//They are used to sort [Doublebone]'s based on their @index values

    bool ismatch(unsigned char) const;					//Returns true if the argument matches the value of this bone
    bool ismatch(unsigned char, unsigned char) const;	//Overloaded function
    bool ismatch(unsigned char*) const;					//Overloaded function

    bool addlink();										//Indicates that a [Bone] has been played on this [Doublebone].  The lock decrements

    void setloc(unsigned short int, unsigned short int);//Finalizes this bone's location on the game map
    int getloc() const;									//Returns both locations using 24 of 32 bits

    unsigned int getindex() const;

private:
    static unsigned short int index;	//2b	//In order increment used for sorting base on creation time
    unsigned char value : 8;			//2b	//The value this bone represents
    signed char left : 4;				//0.5b	//The number of required plays left
    short int loc1 : 12;				//1.5b	//The absolute location of one half of the bone

    short int loc2 : 12;				//1.5b	//The absolute location of one half of the bone
    //Padding : 4						//0.5b
    const unsigned short int id;		//2b	//The automatically assigned integer used for sorting
};	//Alignment: 2 bytes?



///-----------------------------------------------------------
//class field: Contains the entire list of played nodes, as well as a list of locked doubles
///-----------------------------------------------------------
//Alternatively, this could be set up as a directed graph of edges.
//I chose this design to track both edge to edge connections as well as relative proximity.
//	|1|1|2|_|		|1|1|2|2|		|1|1|2|2|
//	|5|_|_|_|	->	|5|_|_|4|	->	|5|_|_|4|	+ |x|4| at the bottom right is difficult to detect with a graph,
//	|_|_|_|_|		|_|_|_|_|		|5|6|_|_|			since 4 still has 3 open edges
class field : public collection
{
public:
	field();
	~field();
	field(unsigned char);

	bool playbone(const unsigned char *v);										//Tries to place a bone -> calls check(x,y);
	void reset();																//Rests the board
	virtual void display() const;												//Overide -> Displays the board

private:
	void storelock(doublebone*&);												//Enqueus a [Doublebone]
	bool checklocks(unsigned char, unsigned char, doublebone*&);				//Compares arguments to a [Doublebone] in the list.  If a match is found, that [Doublebone] is returned through the third argument

	bool check(unsigned char v1, unsigned char v2);				//Iterates through the usable board for a match to either value
	bool tryplace(signed char, unsigned char, unsigned char, unsigned char) const;	//The last check before placing a bone
	bool place(short loc1, short loc2, unsigned char v1, unsigned char v2);		//Places bone, calls update, stores doubles, and exits

	bool checkmatch(unsigned char, unsigned char, unsigned char, unsigned char);
	bool adjacent(unsigned char, unsigned char, short int, unsigned char) const;//Check for adjacent cell conflicts
	signed char open(unsigned char row, unsigned char col) const;				//Check if any cells are open near a match

	void updatecell(short int loc);									//Updatecell -> Entry point.  It will call all other update functions
	void updatecell(unsigned char row, unsigned char col);					//Determines if a single cell is still playable
	void updateperimeter(unsigned char, unsigned char);						//Adjusts the size of the map as needed

	void getcell(short int, unsigned char&, unsigned char&) const;			//Utility function to convert (row, column) into an index value

	short int matchnear(unsigned char, unsigned char, unsigned char, signed char) const;
	void displayloc(unsigned char, unsigned char) const;					//Displays an individual cell
	void displaymap() const;												//Displays the entire map


	friend bool autoplay(node*&);									//Just for fun -> friend a function to automatically play for a [Player]
	static bool icheck(unsigned char, unsigned char);				//A static function specifically designed to allow (Autoplay) to cheat

	static field *cheat;		//4b									//A backdoor allowing (Autoplay) to have an unfair advantage

	const unsigned char size;	//1b								//Total size of the bone set.  This will be double the maximum face value.

	short int topend : 8;		//1b								//The row of the highest tile
	short int lowend : 8;		//1b								//The row of the lowest tile
	short int lend : 8;			//1b								//The column of the furthest left tile

	short int rend : 8;			//1b								//The column of the furthest right tile
	bool cheatmode;				//1b
	//Padding : 16				//2b

	int *map;					//4b								//The linear matrix. map[size * size + size] max 2^16
};//Alignment: 4b ?

#endif /* CHICKENFIELD_H_ */
