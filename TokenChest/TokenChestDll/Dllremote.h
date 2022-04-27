/*
Written by r57shell from Blizzhackers
http://www.blizzhackers.cc/viewtopic.php?f=182&t=499536
*/


#ifndef REMOTE_TEMPLATE_H
#define REMOTE_TEMPLATE_H

#include <psapi.h>
#include <map>
#include <memory.h>

template<class reader>
struct RemoteData
{
	typedef typename std::map<unsigned int, void *> Map;
	typedef typename Map::iterator iterator;
	static Map m;

	static void * read(unsigned int ptr, size_t size)
	{
		iterator f = m.find(ptr);
		if (f != m.end())
			return f->second;
		unsigned int* p = (unsigned int*)malloc(size + sizeof(unsigned int));
		if (!p)
			throw 42;
		p[0] = size;
		reader::read(&p[1], ptr, size);
		m[ptr] = &p[1];
		return &p[1];
	}

	static void Release()
	{
		iterator i;
		for (i = m.begin(); i!=m.end(); ++i)
			free((unsigned int*)i->second-1);
		m.clear();
	}
};

template<class reader> std::map<unsigned int, void *> RemoteData<reader>::m;

template<typename T, class reader>
struct Remote
{
	unsigned int ptr;
	Remote()
	{}
	Remote(unsigned int addr)
	{
		ptr = addr;
	}
	const T & operator *()
	{
		return *(T*)RemoteData<reader>::read(ptr,sizeof(T));
	}
};

#endif
