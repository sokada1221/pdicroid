#ifndef __PRONTBL_H
#define	__PRONTBL_H

typedef struct {
	tnstr key;
	tnstr code;
} PRONTABLE;

class PronTable : public FlexObjectArray<PRONTABLE> {
	public:
		PronTable();
		~PronTable();
		int bsearch( const tchar *key );
		int search( const tchar *key );
		int add( const tchar *key, const tchar *code );
		const tchar *find( const tchar *key, tchar **next=NULL );
};

PronTable *OpenPronTable( const tchar *path );

#endif	// __PRONTBL_H
