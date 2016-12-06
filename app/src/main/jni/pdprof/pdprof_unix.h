#ifndef __pdprof_unix_h
#define	__pdprof_unix_h

class PdProfile
{
public:
	// Global Configurations //
	bool IsTextFileBOM() { return true; }
	int GetTextFileCode() { return TFM_DEFAULT; }
};

#endif
