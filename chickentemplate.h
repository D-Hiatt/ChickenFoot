//
/// chickentemplate.h
//
//  Created on: Jan 22, 2015
//	Updated on: Jan 31, 2015
//      Author: David Hiatt
//
///	CS202 - Program 2 - chickenfoot
//
///	Contents:
//		Contains defintions for all template functions
//		*These are not actual declarations; the compiler will build those.
//		*Equivalent to prototypes?
///	Overview:
//		These templates remove the RTTI requirement from derived classes.
//		All RTTI can now be performed in one location.
//			(Each template is converted to an actual member function at compile time,
//			 with the signature [return] [class]::[function name]<[player|bone|etc]>([args]))
//		It is just quicker to have the compiler do it.
//
//		RTTI is used exclusively to compare any two nodes in a list and downcasting.
///
#ifndef CHICKENTEMPLATE_H_
#define CHICKENTEMPLATE_H_

#include "chickenbase.h"


/* [Node::<T>] Start */

//// [Node] Template Function (SetNext)
//	void node::setnext(T *&out)
///	Purpose:
//		Downcasts the argument and stores the address at @next
////
template<typename T>
void node::setnext(T *&out)
{
	if(out)
		next = dynamic_cast<node*>(out);
	else
		next = 0;
}

//// [Node] Template Function (SetPrev)
//	void node::setprev(T *&out)
///	Purpose:
//		Downcasts the argument and stores the address at @prev
////
template<typename T>
void node::setprev(T *&out)
{
	if(out)
		prev = dynamic_cast<node*>(out);
	else
		prev = 0;
}

//// [Node] Template Function (GoPrev)
//	void node::goprev(T *&out)
///	Purpose:
//		Returns @prev through the argument @out
//		I don't think this actually needs a dynamic_cast
//			if an overload were created for every derived class
//			and every derived class were known at compile time.
////
template<typename T>
bool node::goprev(T *&out)
{
	out = 0;
	if(prev && prev != this)
		out = dynamic_cast<T*>(prev);
	return out != 0;
}

//// [Node] Template Function (GoNext)
//	void node::gonext(T *&out)
///	Purpose:
//		Returns @next through the argument @out
//		I don't think this actually needs a dynamic_cast
//			if an overload were created for every derived class
//			and every derived class were known at compile time.
////
template<typename T>
bool node::gonext(T *&out)
{
	out = 0;
	if(next && next != this)
		out = dynamic_cast<T*>(next);
	return out != 0;
}

/* [Node::<T>] End */




/* [Collection::<T>] Start */

//// [Collection] Template function (Enqueue)
//	void collection::enqueue(T *&tmp)
///	Purpose:
//		Inserts a [Node] into the back of the queue
//		The argument @tmp will be zeroed before returning
////
template<typename T>
void collection::enqueue(T *&tmp)
{
	if(tmp)
	{
		node *safe = dynamic_cast<node*>(tmp);			//Try to downcast the argument @tmp
		if(safe)
		{
			++total;
			if(!root)
			{
				root = safe;
				root->setnext(root);
				root->setprev(root);
			}
			else
			{
				node *swapper = root;
				if(root->hasnext())
				{
					root->gonext(swapper);							//Get the old [last in line]
					swapper->setprev(safe);							//Tell them that there is a new follower
				}
				else
				{
					root->setprev(safe);							//@root.prev is #2 in line, which is only the new node if there's only two nodes
				}
					safe->setprev(root);							//The last node always follows @root
					safe->setnext(swapper);							//@safe.next will be @root when there's only two nodes
					root->setnext(safe);							//@root always has valid @next and @prev pointers
			}
		}
		safe = 0;
	}
	tmp = 0;
}

//// [Collection] Template function (Dequeue)
//	void collection::dequeue(T *&tmp)
///	Purpose:
//		Returns the next [Node] through the argument @caller
//		Decrements the total as needed
////
template<typename T>
bool collection::dequeue(T *&caller)
{
	caller = 0;									//Clear @caller to prevent false positives
	if(root)
	{
		node *current = root;					//@Root is always the front
		node *front = 0;						//@Front is a temporary swap variable

		root->gonext(front);					//Store the last node in @front
		root->goprev(root);						//Set @root to the previous node

		if(root)
			root->setnext(front);				//Attach the new @root to the end of the queue
		if(front)
			front->setprev(root);				//Attach the end of the queue to the new front

		current->unlink();						//Remove all links on the return

		--total;
		caller = dynamic_cast<T*>(current);		//Upcasting, if the type is known..
		current = 0;
	}

	return caller != 0;
}


//// [Collection] Template function (Peek)
//	void collection::peek(T *&tmp)
///	Purpose:
//		Returns the next [Node] through the argument @caller
//		Does not remove it from the queue
////
template<typename T>
bool collection::peek(T *&item) const
{
	if(root)
	{
		item = dynamic_cast<T*>(root);
	}
	return item != 0;
}


//// [Collection] Template function (Swap)
//	void collection::swap(T *&old, signed char id)
///	Purpose:
//		A shortcut for (Dequeue && Enqueue] in one call
//		The optional argument allows the caller to request a [Node] at a specific position in the queue
/// Arguments:
//		@old	|	Passed in as a node to be stored, returned as a node in the list
//		@id		|	An integer below 127 indicating the requested [Node] number
//					If @id is less than 0, assume the next node is requested
////
template<typename T>
void  collection::swap(T *&old, signed char id)
{
	node *tmp = 0;
	enqueue(old);							//Store the original node.  It is inserted at the end and should not affect index count
	if(id >= 0 && id <= total)
	{
		tmp = root;
		while(tmp->hasprev() && --id > 0)	//Traverse the specified number of nodes.
			tmp->goprev(tmp);
		if(tmp)
		{
			--total;
			node *back = 0;					//Store the [Node] behind this one
			node *front = 0;				//Also store the one in front
			tmp->goprev(back);
			tmp->gonext(front);
			tmp->unlink();					//Remove the target [Node] from the list
			old = dynamic_cast<T*>(tmp);	//Place it in the return variable, @old
			if(back)
				back->setnext(front);		//Reconnect the list
			if(front)
				front->setprev(back);		//In both directions
			back = front = 0;
		}
	}
	else
	{
			dequeue(old);					//If no index was specified, just return the next one
	}
	tmp = 0;
}


//// [Collection] Template function (Swap)
//	void collection::swap(T *&old)
///	Purpose:
//		A wrapper function to [Enqueue] and [Dequeue] for the next node
/// Arguments:
//		@old	|	The original value is stored in queue, then is set to the next node in queue
//////
template<typename T>
void collection::swap(T *&old)
{
	swap(old, -1);
}


//// [Collection] Template function (Sort)
//	void collection::sort(int (T::*)())
///	Purpose:
//		Sorts a list using a custom member function call for comparison
//		The actual sorting is performed by recursively swapping nodes one at a time.
//		The list is considered sorted when no swaps are performed
//
/// Arguments:
//		@compare	|	A late bind to a member function of class T.
//						The function must return an #int and take no arguments.
/// Locals:
//		bool @unsorted	|	Determines if the function needs to recurse again to sort the list
//		node* @tmpobj	|	First node to compare
//		node* @tmpcomp	|	Second node to compare
//		T* @obj			|	Downcasted type from @tmpobj
//							This is used to be able to call the @compare function on the object
//		T* @comp		|	Downcasted type from @tmpcomp
//							This is used to be able to call the @compare function on the object
//////
template<typename T>
void collection::sort(int (T::*compare)())
{
	node *tmpobj = root;
	if(tmpobj && total > 1)
	{
		bool unsorted = false;
		node *tmpcomp = 0;
		tmpobj->goprev(tmpcomp);											//Set @tmpobj to the [Node] behind the @root
		if(tmpcomp)
		{
			do
			{
				T *obj = dynamic_cast<T*>(tmpobj);							//Downcast from type [Node] to type [T:Derived]
				T *comp = dynamic_cast<T*>(tmpcomp);						//$comp is the node behind $obj
				if(obj && comp)
				{
					if(((comp->*compare)() > (obj->*compare)()))			//Call the @compare member function pointer on the derived typed objects
					{														//Always compare the previous [Node] to the one in front
						unsorted = (tmpobj != root);						//Only flag recursion if any [Node] other than the @root was switched
						node *swap = 0;
						tmpobj->gonext(swap);								//Store the [Node] in front of the current first [Node]
						if(swap)
							swap->setprev(tmpcomp);							//If @swap exists, reset @swap's previous pointer to the second [Node]

																			//@Swap can be NULL and @next will still be set
						tmpcomp->setnext(swap);								//Set the second [Node]'s @next pointer to the [Node] in front of the first.
						tmpobj->setnext(tmpcomp);							//Swap the first and second [Node]

						tmpcomp->goprev(swap);								//Store the [Node] behind the second [Node]
						if(swap)
							swap->setnext(tmpobj);							//If @swap exists, set it's @next pointer to the current first [Node]

						tmpobj->setprev(swap);								//Set the first [Node]'s @prev pointer to the [Node] behind the second.
						tmpcomp->setprev(tmpobj);							//Swap the second and the first [Node]

						if(tmpobj == root)
						{
							root = tmpcomp;									//Update @root if needed
							tmpcomp = 0;
						}
						swap = 0;
					}
					else
						tmpobj = tmpcomp;									//Perform the loop again, but start with @tmpcomp in the front
				}
				tmpobj->goprev(tmpcomp);									//Now compare the @prev [Node] with it's @prev
				obj = comp = 0;
			}while(tmpcomp && tmpcomp != root);								//If we are comparing to @root again, then we have reached the end.
			tmpcomp =  0;
			if(unsorted)
				sort(compare);												//Recurse if any [Node] was switched
		}
	}
	tmpobj = 0;
}

/* [Collection::<T>] End */

#endif /* CHICKENTEMPLATE_H_ */
