//---------------------------------------------------------------------------

#ifndef TextResourceH
#define TextResourceH
//---------------------------------------------------------------------------

bool InitTextResource(const tchar *filename, int orgIndex, int preAllocSize);
void CleanupTextResource();
const tchar *GetTextResource(int id);
//bool GetTextResource(int id, TNChar &str);
bool GetTextResource(int id, class __tnstr &str);
void GetTextResourceArray(tnstr_vec &array, int id, const tchar *delim);
int SetTextResource(const tchar *text, int id=-1);
int SetTextTempResource(const tchar *text);

#endif	/* TextResourceH */

