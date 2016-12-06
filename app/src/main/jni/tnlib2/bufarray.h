#ifndef __BUFARRAY_H
#define	__BUFARRAY_H

#ifndef __BUFFER_H
#include "buffer.h"
#endif

class FlexBufferArray : public FlexObjectArray<Buffer> {
	public:
		FlexBufferArray( int slotsize=5 ){}
		int add( const void *buffer, int size )
			{ return FlexObjectArray<Buffer>::add( new Buffer( buffer, size ) ); }
		int add( Buffer *obj )
			{ return FlexObjectArray<Buffer>::add( obj ); }
};


#endif	// __BUFARRAY_H
