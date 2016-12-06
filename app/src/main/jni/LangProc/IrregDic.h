#ifndef __IrregDic_h
#define	__IrregDic_h

class TIrregDic {
protected:
	TIFile File;
	tnstr  FileName;
	__int64 LastModTime;
public:
	TIrregDic()
	{
		LastModTime = 0;
	}
	bool Read(const tchar *filename, tnstr &s);
//	bool IsOpened()
//		{ return File.is_open(); }
	bool IsModified();
protected:
	bool Open(const tchar *filename);
	void Close();
	__int64 GetFileTime();
};

#endif	/* __IrregDic_h */

