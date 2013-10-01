#pragma once

template <class T>

class IndexedNode
{
public:
	template <class T> friend class IndexedList;

	IndexedNode(T * item, int index)
	{
		this->index = index;
		data = item;
	}

	~IndexedNode()
	{

	}

	IndexedNode<T> * getNext()
	{ return next; }

	IndexedNode<T> * getPrev()
	{ return prev; }

	T * getData()
	{ return data; }

	int getIndex()
	{ 
		return index; 
	}

private:
	int index;
	T * data;
	IndexedNode<T> * next;
	IndexedNode<T> * prev;
};