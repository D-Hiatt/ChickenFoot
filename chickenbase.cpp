//
/// chickebase.cpp
//
//  Created on: Jan 18, 2015
//      Author: David Hiatt
//
///	CS202 - Program 1 - chickenfoot
//	Contains only abstract base classes
///	[Classes]
//		class node		|	A generic object
//								Inhertited by:	[Player][Bone]
//		class collection|	A collection of nodes arranged as a queue
//								Inherited by:	[Player][Game]
////

#include "chickenbase.h"

#include <cstdio>
#include <typeinfo>

#include "chickencoop.h"

using namespace std;


/// [Node] Constructor (Default)
//	node::node()
///
node::node() : next(NULL), prev(NULL)
{}


/// [Node] Constructor (Copy)
//	node::node(const node&)
///
//node::node(const node &copy) : forward(NULL), back(NULL)
//{
//}


//// [Node] Function (Unlink)
//	void node::unlink()
///	Purpose:
//		Removes both pointers from this object
////
void node::unlink()
{
	next = prev = NULL;
}

//// [Node] Function (HasNext/Prev)
//	bool node::hasnext()
///	Purpose:
//		Returns true if the pointer is set and does not point to itself
////
bool node::hasprev() const { return prev != NULL && prev != this;}
bool node::hasnext() const { return next != NULL && next != this;}


//// [Node] Virtual Operator (Comparison, Overloads)
//	void node::operator ==,>()
///	Purpose:
//		They both return false unless they are overwritten
////
bool node::operator ==(const node &other) const
{
	return false;
}
bool node::operator >(const node &other) const
{
	return true;
}

//// [Node] Operator (Comparison, Overloads)
//	void node::operator <=!>()
///	Purpose:
//		All of these end up calling one of two virtual overloads above
////
bool node::operator ==(node const *other) const
{
	return other != NULL && typeid(this) == typeid(other);
}


bool node::operator !=(node const *other) const
{
	return other == NULL || typeid(this) != typeid(other) || !(*this == *other);
}

bool node::operator >=(node const *other) const
{
	return other == NULL || (typeid(this) == typeid(other) && ((*this == *other) || (*this > *other)));
}

bool node::operator <=(node const *other) const
{
	return other != NULL && typeid(this) == typeid(other) && ((*this == *other) || !(*this > *other));
}

bool node::operator <(node const *other) const
{
	return other != NULL && typeid(this) == typeid(other) && (!(*this == *other) && !(*this > *other));
}

bool node::operator >(node const *other) const
{
	return other == NULL ||  (typeid(this) == typeid(other) && (*this > *other));
}

bool node::operator !=(const node &other) const
{
	return typeid(this) != typeid(other) || !(*this == other);
}

bool node::operator >=(const node &other) const
{
	return typeid(this) == typeid(other) && ((*this == other) || (*this > other));
}

bool node::operator <=(const node &other) const
{
	return typeid(this) != typeid(other) || ((*this == other) || !(*this > other));
}

bool node::operator <(const node &other) const
{
	return typeid(this) == typeid(other) && (!(*this == other) && !(*this > other));
}
////-> End [Node] Operator (Comparison, Overloads)

/// [Node] Destructor
node::~node()
{
	node *tmp = next;
	if(prev && prev->next == this)
		prev->next = NULL;
	prev = NULL;
	next = NULL;
	if(tmp && tmp != this)
	{
		tmp->prev = NULL;
		if(tmp->next == this)
			tmp->next = NULL;
		delete tmp;
		tmp = NULL;
	}
	tmp = NULL;
}

/* [Node] End */



/* [Collection] Start */

/// [Collection] Constructor (Default)
collection::collection() : total(0), root(NULL)
{
}


//// [Collection] Template function (Sort)
//	void collection::sort()
///	Purpose:
//		Sorts a list using the [Node] class comparison operator overload
//		The actual sorting is performed by recursively swapping nodes one at a time.
//		The list is considered sorted when no swaps are performed
//
/// Locals:
//		bool @unsorted	|	Determines if the function needs to recurse again to sort the list
//		node* @tmp		|	First node to compare
//		node* @tmp		|	Second node to compare
//////
void collection::sort()
{
	node *tmp = root;
	if(tmp)
	{
		node *comp = NULL;
		tmp->goprev(comp);
		if(comp)
		{
			bool unsorted = false;
			do
			{
				if(comp && *comp > tmp)				//----> Check if [Node] is greater than a pointer to the previous [Node]
				{
					unsorted = (tmp != root);			//Set the @unsorted flag if neither [Node] is the @root
					node *swap = NULL;

					tmp->gonext(swap);
					if(swap)							//Set the @prev variable of the [Node] in front of @tmp to be @comp
						swap->setprev(comp);

					comp->setnext(swap);				//Swap the @next pointers
					tmp->setnext(comp);

					comp->goprev(swap);
					if(swap)							//Set the @next variable of the [Node] behind of @comp to be @tmp
						swap->setnext(tmp);

					tmp->setprev(swap);					//Swap the @prev pointers
					comp->setprev(tmp);
					if(tmp == root)
					{
						root = comp;					//Update the @root pointer
						comp = NULL;
					}
					swap = NULL;
				}
				else
					tmp = comp;
				tmp->goprev(comp);					//<---- Loop backwards until the @root is reached again
			}while(comp && comp != root);
			if(unsorted)
				sort();							//Recurse if unsorted
		}
	}

}

//// [Collection] Function (Display)
//	void collection::display()
///	Purpose:
//		Displays every node in the list in order
////
void collection::display() const
{
	node *tmp = root;
	do
	{
		if(tmp)
		{
			tmp->display();
			tmp->goprev(tmp);
		}
	}while(tmp && tmp != root);
	tmp = 0;
}


//// [Collection] Function (View)
//	void collection::view()
///	Purpose:
//		A simpel and quick way to display the first object in the list
////
void collection::view() const
{
	if(root)
		root->display();
}


//// [Collection] Function (Clear)
//	void collection::clear()
///	Purpose:
//		Removes all nodes from the list
////
void collection::clear()
{
	if(root)
		delete root;
	root = NULL;
	total = 0;
}


//// [Collection] Function (Count)
//	short int collection::count()
///	Purpose:
//		Returns the number of nodes in the collection
////
short int collection::count() const
{
	return total;
}


/// [Collection] Destructor
collection::~collection()
{
	if(root)
		delete root;
	root = NULL;
	total = 0;
}

/* [Collection] End */

/* chickenbase.cpp End */
