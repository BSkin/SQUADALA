#pragma once

#include "IndexedNode.h"

template <class T>

class IndexedList
{
public:
	inline IndexedList() 
	{
		first = 0;
		last = 0;
		size = 0;
	}
	
	inline ~IndexedList()
	{
		while (size > 0) popFront();
	}

	IndexedNode<T> * getFront(void) 
	{ return first; }

	IndexedNode<T> * getBack(void) 
	{ return last; }

	IndexedNode<T> * getNode(int index)
	{
		if (index >= size) index = size-1;
		if (index < 0) index = 0;
	
		IndexedNode<T> * tempNode;
	
		if (index < size/2)
		{
			tempNode = first;
			while (tempNode->getIndex() != index) tempNode = tempNode->getNext();
		}
		else
		{
			tempNode = last;
			while (tempNode->getIndex() != index) tempNode = tempNode->getPrev();
		}
	
		return tempNode;
	}

	int getSize(void) 
	{ return size; }

	void addItem(T * item)
	{
		IndexedNode<T> * tempNode = new IndexedNode<T>(item, size);

		if (size == 0)
		{
			first = tempNode;
			last = tempNode;
		}
		else
		{
			tempNode->prev = last;
			last->next = tempNode;
			last = tempNode;
		}
		size++;
	}

	void addItem(T *, int index)
	{
		if (index > size) index = size;
	
		if (index == size) 
		{
			addItem(item);
			return;
		}

		IndexedNode<T> * tempNode = new IndexedNode<T>(item, index);
		IndexedNode<T> * startNode = getNode(index);

		if (startNode->index != 0)
		tempNode->prev = startNode->prev;

		tempNode->next = startNode;
		startNode->prev = tempNode;
	
		while (startNode != 0)
		{
			startNode->index++;
			startNode = startNode->next;
		}

		size++;
	}

	void popFront()
	{
		IndexedNode<T> * frontNode = first;

		if (size == 1)
		{
			first = 0; 
			last = 0;
		}
		else if (size > 1)
		{
			frontNode->next->prev = 0;
			first = frontNode->next;
		}
	
		delete frontNode;

		IndexedNode<T> * tempNode = first;
		while (tempNode != 0);
		{
			tempNode->index--;
			tempNode = tempNode->next;
		}

		size--;
	}

	void popBack()
	{
		IndexedNode<T> * backNode = last;

		if (size == 1)
		{
			first = 0;
			last = 0;
		}
		else if (size > 1)
		{
			backNode->prev->next = 0;
			last = backNode->prev;
		}

		delete backNode;

		size--;
	}

	void popNode(int index)
	{
		if (size == 0) return;
		if (tempNode->index == size-1)
		{
			popBack();
			return;
		}
		else if (tempNode->index == 0) 
		{
			popFront();
			return;
		}
		else 
		{
			IndexedNode<T> * tempNode = getNode(index);
			tempNode->prev->next = tempNode->next;
			tempNode->next->prev = tempNode->prev;

			IndexedNode<T> * startNode = tempNode->next;

			delete tempNode;

			while (startNode != 0)
			{
				startNode->index--;
				startNode = startNode->next;
			}	
		}

		size--;
	}

private:
	IndexedNode<T> * first;
	IndexedNode<T> * last;
	int size;
};