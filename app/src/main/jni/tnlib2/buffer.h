#ifndef	__BUFFER_H
#define	__BUFFER_H

class Buffer {
protected:
	byte *buf;
	int size;
public:
	Buffer( int _size );
	Buffer( const void *data, int _size );
	// ï°êª
	Buffer( Buffer *buffer );
	~Buffer();
	operator char *()
		{ return (char*)buf; }
	operator byte *()
		{ return buf; }
//		char &operator[]( int i )
//			{ return buf[i]; }
	byte &operator[]( int i )
		{ return buf[i]; }
	int GetSize( )
		{ return size; }
};


#endif	// __BUFFER_H
