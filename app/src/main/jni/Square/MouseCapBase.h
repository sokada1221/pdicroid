#ifndef __MouseCapBase_h
#define	__MouseCapBase_h

//	ヒットテスト項目・結果
#define	HT_NONE			0
#define	HT_WORDITEM		0x0100		// 英単語の選択
									// 下位８ビットが項目インデックス
#define	HT_MEMORY		0x8000u		// "暗"
#define	HT_MODIFY		0x4000		// "修"
#define	HT_OUT			0x0080		// 暗、修のとき、マウスの位置がはずれた場合
// 注：ヒットテスト項目にHT_WORDITEMを指定するとHT_MEMORYかHT_MODIFYが
// 返ってくることがある。そのときはHT_WORDITEMは付かない。
// また、下位８ビットの意味は変わらない

//#define	HT_TITLEBAR		0x0200		// タイトルバー
//#define	HT_TITLEBORDER	0x0400		// タイトルバーの境界

#define	HT_UPPER		0x0800		// 表示領域より上の方
#define	HT_LOWER		0x1000		// 表示領域より下の方
//#define	HT_MSGBAR		0x2000		// メッセージバー

#define	HT_MASK			0xff00

class TMouseViewIFBase {
public:
	virtual void SetHintText(const tchar *msg){}
};

class TMouseCaptureBase {
protected:
	TMouseViewIFBase &View;
	int fCapturing;
public:
	TMouseCaptureBase(TMouseViewIFBase &view);
	TMouseViewIFBase &GetView() const
		{ return View; }
	bool IsCapturing() const
		{ return fCapturing?true:false; }
	void SetCapturing(int capturing)	// wsobj.cpp専用 -> 削除したい
		{ fCapturing = capturing; }
};

#endif	// __MouseCapBase_h

