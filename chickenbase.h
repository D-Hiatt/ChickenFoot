//
// chickenbase.h
//
//  Created on: Jan 18, 2015
//	Updated on: Jan 30, 2015
//      Author: David Hiatt
//
///	CS202 - Program 2 - chickenfoot
//	Contains only abstract base classes
///	[Classes]
//		class node		|	A generic object
//								Inhertited by:	[Player][Bone][Doublebone]
//		class collection|	A collection of nodes arranged as a queue
//								Inherited by:	[Player][Game][Field]
////

#ifndef CHICKENBASE_H_
#define CHICKENBASE_H_


///-----------------------------------------------------------
// class node:	Base class for any data type
///-----------------------------------------------------------
//This class performs the standard doubly linked list node functions.
//Many of the functions are templated to isolate all downcasting within a single base class.
//This allows all other classes to insert and remove any derived type of node
///
class node
{
public:
	node();
	virtual ~node();

//Pure virtual
	virtual void display() const = 0;				//Force derived class to override display.

//Virtuals
	virtual bool operator >(const node&) const;		//Virtual comparison operators
	virtual bool operator ==(const node&) const;

//Templates
	template<typename T>
		bool gonext(T*&);							//Returns the next pointer
	template<typename T>
		bool goprev(T*&);							//Returns the previous pointer

	template<typename T>
		void setprev(T*&);							//Sets the next pointer
	template<typename T>
		void setnext(T*&);							//Sets the previous pointer

	template<typename T, typename V>
		void display(V, V (T::*)()) const;

//Non-Virtuals
	void unlink();									//This as calling $setnext(NULL) and $setprev(NULL)
	bool hasnext() const;							//True if next is a different node
	bool hasprev() const;							//True if previous is a different node

	bool operator <(const node&) const;				//Wrapper comparison operators.
	bool operator >=(const node&) const;			//They all end up calling the same two functions.
	bool operator <=(const node&) const;			//The above virtual comparison operators
	bool operator !=(const node&) const;			//Perform the actual comparison when overriden
	bool operator >(node const*) const;				//Comparison to a pointer uses RTTI to ensure
	bool operator <(node const*) const;				//The comparison is between two of the same type
	bool operator >=(node const*) const;
	bool operator <=(node const*) const;
	bool operator !=(node const*) const;
	bool operator ==(node const*) const;

private:
	node *next;
	node *prev;
};


///-----------------------------------------------------------
//class collection:		Base class for all lists
///-----------------------------------------------------------
class collection
{
public:
	collection();
	virtual ~collection();

//Virtuals
	virtual void display() const;				//Displays each node in queue

//Templates
	template<typename T>
		void enqueue(T*&);						//Inserts a node in the back of the list
	template<typename T>
		bool dequeue(T*&);						//Returns the next node in the list
	template<typename T>
		bool peek(T*&) const;					//Returns the next node in the list without removing it

	template<typename T>
		void swap(T*&);							//This is a wrapper, it calls the overloaded function with a default argument of -1
	template<typename T>
		void swap(T*&, signed char);			//Returns the node at a specific position

//Non-Virtuals
	short int count() const;					//Returns total

protected:
//Virtuals
	virtual void view() const;					//Calls $node::display() on the next node
	virtual void sort();						//Attempt to sort the queue

//Templates
	template<typename T>
		void sort(int (T::*)());				//Attempt to sort the queue using late binding
	template<typename T, typename V>
		void display(V, V (T::*)()) const;		//Displays an attribute of a node using late binding

//Non-Virtuals
	void clear();								//Deletes all nodes in the list

private:
	short int total;							//The number of nodes in queue
	node *root;
};

#include "chickentemplate.h"					//Template signatures
#endif /* CHICKENBASE_H_ */

