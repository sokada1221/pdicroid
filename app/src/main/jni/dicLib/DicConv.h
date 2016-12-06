//---------------------------------------------------------------------------

#ifndef dicconvH
#define dicconvH
//---------------------------------------------------------------------------

// PDICé´èëïœä∑
class TDicConverter {
protected:
	class Perd12 *txDic;
	class Pdic2 *pDic;	// ref.
public:
	TDicConverter();
	~TDicConverter();
	bool ConvertToPdic(const tchar *textname, const tchar *dicname, int dictype);
	bool ConvertToPdic(const tchar *textname, class Pdic *dic, int dictype);
//	bool ConvertFromPdic(const tchar *dicname, const tchar *textname);
	bool ConvertFromPdic(Pdic *dic, const tchar *textname, int dictype);
	static bool SupportedType(const tchar *filename);
	static Perd12 *CreateText(const tchar *filename, int dictype);
};

#endif

