#include "tnlib.h"
#pragma hdrstop
#include <sys/stat.h>
#include "IrregDic.h"

bool TIrregDic::Read(const tchar *filename, tnstr &s)
{
	if (filename){
		Close();
		if (!Open(filename)){
			return false;
		}
		FileName = filename;
		LastModTime = GetFileTime();
	}
	if (File.getline(s)>=0){
		return true;
	}
	Close();
	return false;
}
bool TIrregDic::Open(const tchar *filename)
{
	if (File.is_open()){
		return false;	// Already opened.
	}
	
	if (File.open(filename)!=0){
		return false;	// Open failure.
	}
	return true;
}
void TIrregDic::Close()
{
	File.close();
}
bool TIrregDic::IsModified()
{
	if (FileName.IsEmpty())
		return false;
	__int64 t = GetFileTime();
	return LastModTime!=t;
}

__int64 TIrregDic::GetFileTime()
{
	return FileTime(FileName);
#if 0	// old version
	if (File.is_open()){
		struct stat st;
		if (fstat(File.get_fd(), &st)==0){
			return st.st_mtime;
		}
	} else {
#if __BORLANDC__==0x0550 || __BORLANDC__==0x0551
		struct stat st;		//TODO: VCL bug struct _stat‚ğg‚¤‚Æalignment‚ª‚®‚¿‚á‚®‚¿‚á
#else
		// ‚Ù‚©‚Ìˆ—Œn‚ÍH
#endif
		if (_tstat(FileName, (struct _stat*)&st)==0){
			return st.st_mtime;
		}
	}
	return -1;	// error
#endif
}

