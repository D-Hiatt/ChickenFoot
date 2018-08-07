//
/// chickefoot.cpp	(Main)
//
//  Created on: Jan 15, 2015
//	Updated on: Jan 20, 2015
//      Author: David Hiatt
//
///	CS202 - Program 2 - chickenfoot
//
///	[Classes]
//		class bone		|	The basic domino unit
//		class player	|	A generic player
//		class yard		|	Contains the linear array of pointers to unplayed bones
//		class game		|	Manages which player's turn it is and how bones are dealt
////

#include "chickenfoot.h"

#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <typeinfo>

#include "chickencoop.h"
#include "chickentemplate.h"

using namespace std;


//// [Main] (Entry)
//	int main(int, char**)
///	Purpose:
//		Initial object construction
//		Handle program arguments
//
/// Variables:
//		int @count		|	This is set to any integer passed in as an argument of @argv
//							If set, it instructs the game to be built with a specific domino set size
//							It will be rounded to be between 1 - 24
//		char* @buffer	|	A static buffer to sanatize input arguments
//		game  @g		|	The game object
////
int main(int argc, char **argv)
{
	srand(time(0));						//The seeded @rand() generator is used to shuffle the deck
	game g;								//@g
	int count = 0;						//@count
	char buffer[256];					//@buffer
	for(int i = 1; i < argc; i++)			//Loop through @argv
	{
		long int c = strtol(argv[i], NULL, 10);	//TMP variable for comparison
		if(c > 0)								//If this argument was a number, assume its @count
		{
			if(count == 0)
				count = c;
		}
		else									//Otherwise, it's now a player's name!
		{
			memset(buffer, '\0', 255);
			if(strlen(argv[i]) > 255)				//More than 255 char program argument?
			{
				memmove(buffer, argv[i], 254);			//Copied over for manipulation
				buffer[255] = '\0';
			}
			else
				strcpy(buffer, argv[i]);
			if(getinput(buffer))				//Removes wierd symbols and such
			{
				g.addplayer(buffer);				//Add a player to the game
			}
		}
	}
	cout.fill('_');									//Set fill char to keep the game map somewhat in sync
	do
	{
		g.begin(count);								//The game starts here
		cout << endl << "Play again? (yes/[No])";		//And ends here.
		memset(buffer, '\0', 255);
		cin.getline(buffer, 256, '\n');
	}while(getinput(buffer) && buffer[0] == 'Y');
	memset(buffer, '\0', 255);						//Clean up and erase all data
	count = 0;
	return 0;
}


/* Class declarations ---> */

/* [Bone] Start */

/// [Bone] Constructor (Default)
//	bone::bone()
//		Shouldn't be called, but you get a random bone
///
bone::bone()
{
	value[0] = 1u + (rand() % 9u);
	value[1] = 1u + (rand() % 9u);
}

/// [Bone] Constructor (Set value)
//	bone::bone(unsigned char, unsigned char)
//		Creates a specific bone combination
///
bone::bone(unsigned char top, unsigned char bottom)
{
	value[0] = top;
	value[1] = bottom;
}


//// [Bone] Operator (unsigned char*, Overload)
//	 bone::operator const unsigned char *()
///	Purpose:
//		Quickly convert a bone to its two values
////
const unsigned char* bone::getvalues() const
{
	return value;
}

//// [Bone] Operator (unsigned char*, Overload)
//	 bone::operator const unsigned char *()
///	Purpose:
//		Quickly convert a bone to its two values
////
bool bone::operator >(const node &other) const
{
	return (typeid(*this) == typeid(other)) && (((value[0] == value[1]) * value[0]) > ((((bone&)other).value[0] == ((bone&)other).value[1]) * ((bone&)other).value[0]));
}

//// [Bone] Operator (unsigned char*, Overload)
//	 bone::operator const unsigned char *()
///	Purpose:
//		Quickly convert a bone to its two values
////
bool bone::operator ==(const node &other) const
{
	return (typeid(*this) == typeid(other)) && (value[0] == value[1] == ((bone&)other).value[0] == ((bone&)other).value[1]);
}
//bool bone::operator >(const unsigned char *other) const
//{
//	return (value[0] == value[1]) && (((bone&)other).value[0] != ((bone&)other).value[1] || value[0] > ((bone&)other).value[0]);
//}
//bool bone::operator ==(const unsigned char *other) const
//{
//	return (value[0] == value[1] == ((bone&)other).value[0] == ((bone&)other).value[1]);
//}
bool bone::operator !() const
{
	return !value[0] || !value[1];
}

//// [Bone] Virtual (Display, Override)
//	void bone::display()
//		Displays the values in this bone.  It is called by the [Player] class
///
void bone::display() const
{
	cout << "{" << (ushort)value[0] << ", " << (ushort)value[1] << "}";
}


/// [Bone] Destructor
bone::~bone()
{
	value[0] = value[1] = 0u;
}
/* [Bone] End */




/* [Player] Start */

/// [Player] Constructor (Default)
//	player::player()
//	Default, not used
///
player::player() : overload(NULL), name(NULL)
{
}


/// [Player] Constructor (Name)
//	player::player(const char*)
//		Builds a player with the name @nm
///
player::player(const char *nm, bool (*o)(bone*&)) : overload(o)
{
	if(nm)
	{
		name = new char[strlen(nm) + 1];
		strcpy(name, nm);
	}
}


//// [Player] Function (GetName)
//	 const char* player::getname() const
///	Purpose:
//		Returns the players name.
////
const char* player::getname() const
{
	return name;
}


//// [Player] Virtual Operator (==, Override)
//	bool player::operator >(const node&)
///	Purpose:
//		Mostly an experiment, returns the player with the highest score
///
bool player::operator >(const node &other) const
{
	return (typeid(*this) == typeid(other)) && count() < ((player&)other).count();
}


//// [Player] Virtual Operator (>, Override)
//	bool player::operator >(const node&)
///	Purpose:
//		Mostly an experiment, returns true if two players have the same score
///
bool player::operator ==(const node &other) const
{
	return (typeid(*this) == typeid(other)) && count() == ((player&)other).count();
}


//// [Player] Function (Reset)
//	void player::reset()
///	Purpose:
//		Removes all [Bone]'s in the hand
////
void player::reset()
{
	clear();
}

//// [Player] Virtual (Display, Override)
//	void player::display()
///	Purpose:
//		Displays all bones in a player's hand
////
void player::display() const
{
	cout << name << ": " << (int)count() << endl;
}

//// [Player] Function (HighestDouble)
//	int player::highestdouble()
///	Purpose:
//		Sorts the [Bone]'s in the list by doubles and returns the highest
////
int player::highestdouble()
{
	sort();
	int ret = 0;
	const unsigned char *val = NULL;
	bone *tmp = NULL;
	if(peek(tmp))
	{
		val = tmp->getvalues();
		if(val[0] == val[1])
			ret = val[0];
	}
	val = NULL;
	tmp = NULL;
	return ret;


}

//// [Player] Function (GetBone)
//	void player::getbone(bone*&)
///	Purpose:
//		Takes the @piece bone and places it in the player's hand
////
void player::getbone(bone *&piece)
{
	enqueue(piece);
	piece = NULL;
}


//// [Player] Function (Turn)
//	bool player::turn(bone*&)
///	Purpose:	Handles the player's entire turn.
//		It will cycle through each bone in the player's hand until a selection is made
//		The options are [y|n|d|?|#|q]
//			'Y'	-> Yes. 	Indicates the player wants to play the current bone.
//			'N'	-> Next.	Cycles to the next bone.  This is the default.
//			'D'	-> Draw.	The player needs to draw a bone from the pile.
//			'?'	-> ???		Displays the entire hand with numbered indexes.
//			'#'	-> Num.		Selects the bone at position @# for play.
//			'Q'	-> Quit.	Ends the game
///Return:	Places the result into the input argument [@play]
////
bool player::turn(bone *&play)
{
//	highlight(name, 1);			//Removed, terminal dependant
	//cout << endl;
	display();							//Display the player's name
	cout << endl;
	bone *current = NULL;
	if(overload && peek(current))
	{
		if(overload(current) && dequeue(play))
		{
			while(play != current)
				swap(play);
			current = NULL;
//			bone *front = NULL;
//			bone *back = NULL;
//			play->gonext(front);
//			play->goprev(back);
//			if(front)
//				front->setprev(back);
//			if(back)
//				back->setnext(front);
//			play->unlink();
			return play != NULL;

		}
		return false;
	}
	dequeue(current);			//Gets the next [Bone] in the player's hand
	char buffer[256];
	memset(buffer, '\0', 256);

	while(current)
	{
		cout << "\r\tUse chicken bone? (y|N|d|v|1";
		if(count()  > 1)
			cout << '-' << count();
		cout << "|?|q)\t| ";
		current->display();									//Displays the [Bone]
		cout << " |" << std::flush;
		buffer[0] = '\0';									//I only really char about index 0, but we need a larger size for input
		cin.getline(buffer, 256, '\n');
		getinput(buffer);
		switch(buffer[0])
		{
			case 'Q': enqueue(current);						//Quit the game.
					current = new bone(0,0);
			case 'Y':										//Play the bone
					play = current;
					current = NULL;							//Unset @current to break the loop
			case 'D':enqueue(current);						//Give up and draw a new bone (current will be set to NULL)
				break;
			case 'V':
				{
					this->collection::display();							//Display the player's entire hand.  Scope operator calls collection over node
					cout << endl;
					char a = cout.fill();
					cout.fill(' ');
					for(int i = 1; i < count() + 1; ++i)
						{
						cout.width(5 + (i > 1));
						cout << i;
						}
					cout.fill(a);
					cout << endl;
				}
				break;
			case '?':
					cout << "\r\tOptions: (Yes | No | Draw | View all | [0-9]*:Play # bone | ?:Help | Quit)" << endl;
					cout << "\t\tOnly the first letter is required, but feel free to type all you want" << endl;
				break;
			default:
			{
				int item = strtoul(buffer, NULL, 10);		//If they put in a number, select the bone in that position
				if(item > 0 && item <= count())
				{
					if(item > 1)
					swap(current, (signed char)item);
					play = current;
					current = NULL;
				}
				else
					swap(current);					//Otherwise, just goto the next one
			}

		}
	}
	enqueue(current);								//If there is still a [Bone] out, put it away
	return play != NULL;
}


/// [Player] Destructor
player::~player()
{
	if(name)
		delete[] name;
	name = NULL;
	overload = NULL;
}

/* [Player] End */





/* [Yard] Start */

/// [Yard] Constructor (Default)
//	yard::yard()
//		Not used, hopefully
///
yard::yard() : max(0), remaining(0), amount(0), skeletons(NULL)
{
}

/// [Yard] Constructor (Copy)
//	yard::yard(const yard&)
//		Not used, hopefully
///
//yard::yard(const yard &copy) : max(copy.max), remaining(copy.remaining), amount(copy.amount), skeletons(NULL)
//{
//	if(amount)
//	{
//		skeletons = new bone*[amount];
//		for(int i = 0; i < amount; i++)
//		{
//			skeletons[i] = NULL;
//			if(copy.skeletons[i])
//				skeletons[i] = new bone(*copy.skeletons[i]);
//		}
//	}
//}


/// [Yard] Constructor (Size)
//	yard::yard(unsigned char)
///	Argument:	 @dsize is taken as an integer between 1 and 23
//		The number passed in becomes the size of the domino set to build.
//		The total number of dominos is calculated and stored in @amount
///
yard::yard(unsigned char dsize) : max(dsize), remaining(0), amount(((ushort)dsize * ((ushort)dsize + 1u)) >> 1u), skeletons(NULL)
{
	skeletons = new bone*[amount];
	for(int i = 0; i < amount; ++i)
		skeletons[i] = NULL;
	shuffle();
}


//// [Yard] Function (Count)
//	int yard::count()
///	Purpose:
//		Returns the number of bones left in the deck
////
int yard::count() const
{
	return remaining;
}

//// [Yard] Function (Shuffle)
//	void yard::shuffle()
///	Purpose:
//		Used to both build and shuffle the set of bones.
//	Expects @max to already be set to an appropriate value in the @ctor
/// Algorithm:
//		A single pass Yates-Fisher-ish shuffle.
//		The @ctor precalculated and allocated an array of pointers, this just fills it.
////
void yard::shuffle()// { sort(); }
{
	remaining = 0;
	for(int i = (ushort)max; i > 0; i--)				//Loop from the max digit (usually 9) to 0
	{
		for(int j = i; j > 0; j--)						//Loop from [@i] to 0
		{												//This results with [9,9-1],[8,8-1],etc
			int row = rand() % (remaining + 1);
			if(row != remaining)
				skeletons[remaining] = skeletons[row];				//Legal since were dealing with pointers!
			else if(skeletons[row])		//Clear out any leftover from the last round
				delete skeletons[row];
			skeletons[row] = new bone((unsigned char)i,(unsigned char)j);
			remaining++;
		}
	}
	remaining--;							//One last decrement to align it with the index
}

//// [Yard] Virtual (Display, Override, Debugging)
//	void yard::display()
///	Purpose:
//		Debugging.  It prints every bone in the order they would be called.
//		Validates the @Shuffle() method.
////
void yard::display() const
{
	for(int i = 0; i < amount; i++)
		skeletons[i]->display();
}


//// [Yard] Function (Active)
//	bool yard::active()
///	Purpose:
//		Returns true as long as there are bones left to draw
////
bool yard::active() const
{
	return remaining > 0;
}

//// [Yard] Function (Draw)
//	bool yard::killchicken(bone*&)
///	Purpose:
//		Attempts to draw the next chicken bone from the yard.
//		(You can't get a chicken bone from a living chicken..)
////
bool yard::killchicken(bone *&consumer)
{
	consumer = NULL;
	if(remaining >= 0)
	{
		consumer = skeletons[remaining];				//Preshuffled list allows simple incremental selection
		skeletons[remaining--] = NULL;
	}
	return consumer != NULL;
}


/// [Yard] Destructor
yard::~yard()
{
	if(skeletons)
	{
		for(int i = 0; i < amount; i++)
		{
			if(skeletons[i])
				delete skeletons[i];
			skeletons[i] = NULL;
		}
		delete[] skeletons;
	}
	skeletons = NULL;
	amount = remaining = 0;
}

/* [Yard] End */


/* [Game] Start */

/// [Game] Constructor (Default)
//	game::game()
///
game::game() : bonesize(0), round(0), butcher(NULL), farm(NULL)
{
}


/// [Game] Constructor
//	game::game(const char*)
//		Initializes by building a new player
//		It is identical to (AddPlayer)
///
game::game(const char *nm) : bonesize(0), round(0), butcher(NULL), farm(NULL)
{
	player *tmp =new player(nm);
	enqueue(tmp);
	tmp = NULL;
}


//// [Game] Function (Begin)
//	void game::begin(int)
///	Purpose:
//		Starts the game.
/// Arguments:	@domsize can be used to set the domino set size.  Once.
////
void game::begin(int domsize)
{
	//With a minimum of two players, there needs to be at least 14 + 7 bones to play the game.
	//Let bonesize = n: (n*(n+1)) / 2 >= 20 then n >= 6
	//The maximum that I feel like supporting is 255 or _UCHAR_MAX.  n <= 22
	if(!bonesize)
		bonesize = (domsize > 5 && domsize < 23) ? domsize : 9;
	player *current = NULL;
	deal();
	if(count() < 2)
		return;
	int round = 1;
	dequeue(current);
	player *start = current;					//Store the starting player, to increment the round.
	cout << "Round " << round << "\t\t" << "Bones remaining: " << butcher->count() << endl;
	while(butcher->active()) 					//While the yard has cards..
	{

		bone *move = NULL;
		//cout << "Current player: ";
		farm->display();						//This displays the current board, if any
		if(!current->turn(move))				//Give the player a NULL bone to fill, or not
		{
			if(butcher->killchicken(move))		//Player needs to (Draw), so give them a new bone
			{
				cout << "\rPlayer " << current->getname() << " draws a bone" << endl;
				current->getbone(move);
			}
			else
			{
				break;							//The yard has run out of chickens.  The game is over
			}
		}
		else
		{
			if(move && !(*move))
			{
				delete move;
				move = NULL;
				cout << "\rPlayer " << current->getname() << " has quit the game" << endl;
				break;
			}
			if(farm->playbone(move->getvalues()))			//Try to play the bone on the board
			{
				cout << current->getname() << " has played ";
				move->display();
				cout << endl;
				delete move;					//The board has stored the piece, so delete it
				move = NULL;
			}
			else
			{
				cout << "\tNo valid placement found.  The bone has been returned." << endl;
				current->getbone(move);						//Give the bone back to the player
				if(butcher->killchicken(move))				//And draw a new bone for them
				{
					cout << "Player " << current->getname() << " draws a bone" << endl;
					current->getbone(move);
				}
				else
				{
					break;				//The yard has run out of chickens.  The game is over
				}
			}
		}
		swap(current);					//Goto the next player
		if(start == current)			//If this is the starting player, increment the round
		{
			cout << "Round " << ++round << "\t\t" << "Bones remaining: " << butcher->count() << endl;
		}

	}
	enqueue(current);					//Put away any player's still out
	if(round > 0)
	{
		cout << "Game over" << endl << endl;
		this->sort();
		cout << "End of game scores" << endl;
		display();							//Display results
	}
}


//// [Game] Function (Deal)
//	void game::deal()
///	Purpose:
//		Deals cards to the players in the initial round
////
void game::deal()
{
	if(count() < 2)
		getplayer();
	if(count() < 2)
		return;
	if(!farm)
		farm = new field(bonesize);
	else
		farm->reset();
	if(!butcher)
		butcher = new yard(bonesize);
	else
		butcher->shuffle();
	player *tmp = NULL;
	dequeue(tmp);						//Get a player
	player *start = tmp;
	while(tmp)
	{
		tmp->reset();
		cout << "Dealing " <<   7 - tmp->count() << " bones to " << tmp->getname() << endl;
		bone *wing = NULL;
		for(int i = tmp->count(); i < 7; i++)
		{
			if(!butcher->killchicken(wing))		//The yard has run out of chickens!
				return;							//FIXME: Too many players, not enough bones

			tmp->getbone(wing);					//Give the player the new bone
			wing = NULL;						//Clear the old reference, its address is no longer in scope
		}
		swap(tmp);						//Get the next player
		if(!tmp || tmp == start)
			break;
	}
	enqueue(tmp);
	sort(&player::highestdouble);				//Sort the list based on the player with the highest double
	tmp = start = NULL;
}


//// Function (GetPlayer)
//	void game::getplayer()
///	Purpose:
//		Prompts the user for the name of a player.
//		This could be moved to the default @ctor of the player class,
//			but input/output operations don't seem appropriate
////
void game::getplayer()
{
	char buffer[256];
	cout << endl << "(Type 'quit' to cancel the game)" << endl;
//	while(count() < 2)
//	{
	memset(buffer, '\0', 255);
	cout << "This game is more fun with players. Who wants to play?" << endl;
	cout << "Name:";
	cin.getline(buffer, 256, '\n');
	if(getinput(buffer))
		addplayer(buffer);
	else
		cout << "\r" << "Sorry, I couldn't understand that name." << endl << endl;
	memset(buffer, '\0', 255);
	cout << "Would you like to add another player? ";
	if(count() == 1)
		cout << "(y|n|C|?|q): ";
	else if (count() < 1)
		cout << "(Y|n|c|?|q): ";
	else
		cout << "(y|N|c|?|q): ";
	cin.getline(buffer, 256, '\n');

	if(getinput(buffer))
	{
		switch(buffer[0])
		{
			case 'Y':
				getplayer();
				break;
			case 'N':
				if(count() != 1)
					break;
			case 'C':
					fakeplayer();
					if(count() == 1)
						getplayer();
			case 'Q':
				break;
			default:
				if(count() < 1)
					getplayer();
				else if(count() == 1)
					fakeplayer();
				break;
		}
	}
	else
	{
		if(buffer[0] == '?')
		{
			cout << "\r\tOptions: [ Yes | No | Computer player | Help | Quit ]" << endl;
			getplayer();
		}
		else if(count() < 1)
			getplayer();
		else if(count() == 1)
			fakeplayer();
	}
}


//// [Game] Virtual (Display, Override)
//	void game::display()
///	Purpose:
//		Prints each player in score order.  Lower is better
////
void game::display() const
{
	if(count() > 0)
	{
		cout << "Final scores" << endl;
		this->collection::display();
	}
}


//// [Game] Function (AddPlayer)
//	void game::addplayer(const char*)
///	Purpose:
//		Builds & stores a new player with the name @nm
///	Arguments:
//		The second argument is a function pointer.  It is used to create a computer player.
////
void game::addplayer(const char *nm, bool (*o)(bone*&))
{
	if(nm != NULL)
	{
		player *human = new player(nm, o);
		enqueue(human);
		cout << "\r" << nm << " has been placed in the chicken farm" << endl;
	}
}


//// [Game] Function (FakePlayer)
//	void game::fakeplayer()
///	Purpose:
//		Makes a generic, automated player.
////
void game::fakeplayer()
{
	addplayer("Farmer John", autoplay);
}

//field* game::cheat() const
//{return farm;
//}

/// [Game] Destructor
game::~game()
{
	if(farm)
		delete farm;
	farm = NULL;
	if(butcher)
		delete butcher;
	butcher = NULL;
	round = 0;
	bonesize = 0;
}

/* [Game] End */
/* chickenfoot.cpp END */
