#ifndef __TEXTEXT_H
#define	__TEXTEXT_H

class TextExtension {
protected:
	tnstr_vec exts;
public:
	TextExtension(const tchar *text_exts);
	~TextExtension( );
//	void LoadProfile( );
//	void SaveProfile( );
	void Set( const tchar *str );
	void Get( tnstr &str );
	bool IsText( const tchar *ext );
	static const tchar *DefTextExts;
};

extern TextExtension textext;


#endif	// __TEXTEXT_H

