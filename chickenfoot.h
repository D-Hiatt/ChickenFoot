/*
 * chickenfoot.h
 *
 *	David Hiatt
 *	Created:	12 January 15
 *  Updated:	25 January 15
 *
 *	CS202 - Program 2 - chickenfoot
 *	Version 2
 *
 *	---------------------------------------------------------------------------------
 *	|	Class		|	Parents			|			Description						|
 *	---------------------------------------------------------------------------------
 *	Class player    | Node, Collection	| Manages a player's turn, name, and bones	|
 *	Class bone		| Node				| Holds two values between 1-9			  	|
 *	Class yard		| 					| Stores a set of bones not in use		  	|
 *	Class game		| Collection		| Stores all players; manages who's turn 	|
 *	---------------------------------------------------------------------------------
 *
 */

#ifndef CHICKENFOOT_H_
#define CHICKENFOOT_H_

#include "chickenbase.h"
#include "chickenfield.h"

///-----------------------------------------------------------
//class bone: Holds two values.
///-----------------------------------------------------------
class bone : public node
{
public:
	bone();
	bone(unsigned char, unsigned char);
	~bone();

//Virtuals
	virtual void display() const;						//virtual override; displays values
	virtual bool operator >(const node&) const;			//Overrides to find the largest double
	virtual bool operator ==(const node&) const;

//Non-Virtuals
	const unsigned char* getvalues() const;				//Returns @value.  No longer a cast operator
//	operator const unsigned char*() const;

	bool operator ==(const unsigned char*) const;
	bool operator >(const unsigned char*) const;
	bool operator !() const;							//Checks if a [Bone] is valid.  (@value > 0)

private:
	unsigned char value[2];
};



///-----------------------------------------------------------
//class player: Manages a player's turn and current deck of bones
//	Inheritance -> node & collection.
//		[Node] is needed by [Game] to allow for a collection of [Players]
//		[Collection] is used to handle the [Player]s hand
//	This class could be designed with 'has a' relationship with [Collection] for the hand,
//		but multiple inheritance allows all memory management to be centralized in the base classes
///-----------------------------------------------------------
class player : public node, public collection
{
public:
	player();
	player(const char*, bool (*o)(bone*&) = 0);			//Ctor to create a computer controlled player
//	player(const player&);
	~player();

	virtual void display() const;						//Pure virtual override; Displays name
	virtual bool turn(bone*&);							//Asks the player what they want to do.
	virtual bool operator >(const node&) const;			//Overrides to find the winner
	virtual bool operator ==(const node&) const;

	void reset();										//Deletes all the bones left in the [Player]'s hand
	void getbone(bone *&);								//Allows the player to recieve new bones.
	int highestdouble();
//	unsigned char count() const;					//Returns the number of bones in the hand

	const char* getname() const;						//Returns the [Player]'s name.  No longer a cast operator
//	operator const char*() const;

private:
	bool (*overload)(bone*&);							//A late bound function to be used instead of allowing the player to play a bone
	char *name;
};


///-----------------------------------------------------------
//class yard:	Has a linear array of pointers to all available bones.
//				It uses a little bit more memory than an array of pointer arrays,
//					but is far easier to shuffle.
///-----------------------------------------------------------
class yard
{
public:
	yard();
//	yard(const yard&);
	yard(unsigned char);
	~yard();
	bool active() const;					//True if there are any bones remaining
	bool killchicken(bone*&);				//Removes a bone from the set
	void display() const;					//DEBUG: Displays the entire set, used to check the shuffle method
	int count() const;						//Returns @remaining
	void shuffle();							//Creates a new deck and shuffles it

private:
	unsigned char max;			//1b		//Stores the max value of a bone
	unsigned char overflow;		//1b		//The number of unused backup decks
	short int remaining;		//1b		//The number of bones left in the set
	short int amount;			//1b		//The maximum number of bones in a set
	bone **skeletons;			//4b		//An array of bone pointers
};



///-----------------------------------------------------------
//class game: A list of the players
///-----------------------------------------------------------
//It is also a central object that controls most of the game.
//It deals bones to players, calls their 'turn()' function,
//	and acts as  messenger between players and the board.
//
//	Contains a [Yard] and [Field] object
///
class game : public collection
{
public:
	game();
	game(const char*);
	~game();

	virtual void display() const;			//Override, displays the current score

	void addplayer(const char*, bool (*o)(bone*&) = 0);
	void begin(int domsize = 9);			//Starts the game, iterates between each player's turn
	void deal();
private:
	void getplayer();						//Get at least 2 players.
	void fakeplayer();						//Auto-play and create a player.
	unsigned short bonesize;				//The max size of a bone (between 4 and 23)
	int round;
	yard *butcher;							//Stores all unplayed chicken bones
	field *farm;							//Holds the list of played bones
};

#endif /* CHICKENFOOT_H_ */
