//
// pdc600とPDIC本体共通の定義
#ifndef __PDCDEFS_H
#define	__PDCDEFS_H

// Dictionary Check Parameters //
enum DCP_CODE {
	DCP_NONE,
	DCP_ERR_CHECK,
	DCP_UPDATECOUNT,
	DCP_UPDATESTATUS,
	DCP_UPDATESTATUS_WAIT,
	DCP_UPDATEWORDCOUNT,
	DCP_UPDATEPERCENT,
	DCP_STOPCHECK,
	DCP_ERR_READ,
	DCP_ERR_DISKACCESS,
	DCP_QUERYRECOVER,
	DCP_NORMALEND,
	DCP_ABORT,

	DCP_OPTIMIZE_SUBFUNCTION,
};

// Optimize Sub Function
enum DCP_OPTIMIZE {
	DCPO_TEMPREMAIN,
	DCPO_EMPTYBLOCKREMAIN,
	DCPO_DISKERROR,
};

// [O] - optimizeで使用するparameter
typedef int (*FNDicCheckCallBack)( int code, struct PDDICCHECKPARAM *dcp );
struct PDDICCHECKPARAM {
	FNDicCheckCallBack CallBack;	// [O]
	unsigned long Count;			// DCP_UPDATECOUNT
	unsigned long WordCount;		// DCP_UPDATEWORDCOUNT
	unsigned long Percent;			// [O] DCP_UPDATECOUNT
	int ErrorCode;					// 
	LPCTSTR Message;				// DCP_ERR_CHECK, DCP_UPDATESTATUS, 
	LPCTSTR PosMessage;				// DCP_ERR_CHECK
	UINT UserData;
	int SubFunction;				// [O]
};

// Optimization //
// 必須member  : CallBack
// 使用message : DCP_UPDATEPERCENT, DCP_NORMALEND, DCP_ABORT, DCP_OPTIMIZE_FUNCTION, DCP_STOPCHECK
// DCP_OPTIMIZE_FUNCTION messageは、
// SubFunctionに、DCPO_xxxをセットされる
// 注意
// PDC600.DLL では、内部で DCP_OPTIMIZE_FUNCTIONのSubFunctionをMessageに変換し、DCP_UPDATESTATUSを発生させている

typedef struct _PDCOLEDATAPARAMA {
	char ServerName[18];
	char reserved1[2];
	RECT Rect;
	int ObjectType;
	short nMag;
	short nAspect;
	int DataLength;
} PDCOLEDATAPARAMA;

typedef struct _PDCOLEDATAPARAMW {
	wchar_t ServerName[18];
//	char reserved1[2];
	RECT Rect;
	int ObjectType;
	short nMag;
	short nAspect;
	int DataLength;
} PDCOLEDATAPARAMW;

#if defined(_UNICODE) || (defined(MCHAR) && sizeof(MCHAR)==2)
#define	PDCOLEDATAPARAM		PDCOLEDATAPARAMW
#define	_PDCOLEDATAPARAM	_PDCOLEDATAPARAMW
#else
#define	PDCOLEDATAPARAM		PDCOLEDATAPARAMA
#define	_PDCOLEDATAPARAM	_PDCOLEDATAPARAMA
#endif

// PDGetClearNetError()
// [I] Inet Dic
// [L] LAN Dic
#define	NDERR_NONE			0
#define	NDERR_NOFILE		1	// [L] Network上の辞書がない
#define	NDERR_NETWORK		2	// [I] Network retry error

#define	NDERR_OPEN_REMOTE	3	// [L] open remote
#define	NDERR_READ_REMOTE	4	// [L] read error
#define	NDERR_WRITE_REMOTE	5	// [L]
#define	NDERR_SEEK_REMOTE	6	// [L]

#define	NDERR_OPEN_LOCAL	7	// [L?]
#define	NDERR_READ_LOCAL	8	// [L?] write error
#define	NDERR_WRITE_LOCAL	9	// [L?] write error

#define	NDERR_NOTOPEN		10	// [L]

#define	NDERR_OPEN_HISTORY	11	// [L]

#define	NDERR_WRITELOCK		12	// [L] ほかのapplicationから、更新されているためupdateできない
								// ReadLockが成功してからもう一度行う

#define	NDERR_USERID		13	// [I] useridが間違っている/anonymous not supported
#define	NDERR_CHALLENGE		14	// [I] serverからのchallenge keyが誤っている（パスワード変更された？）
#define	NDERR_AUTH			15	// [I] 権限エラー
#define	NDERR_SERVER		16	// [I] server error
#define	NDERR_PERROR		17	// [I] parameter error（恐らくserverの間違い）
#define	NDERR_BUSY			18	// [I] server busy
#define	NDERR_LASTID		19	// [I] LastID不整合エラー local LastID > network LastID
#define	NDERR_INETDICDLL	20	// [I] MSXML3.0がインストールされていない、InetDic.DLLが見つからない
#define	NDERR_NOTEMP		21	// [I] Temporary network file open error
#define	NDERR_NOURL			22	// [I] No URL
#define	NDERR_FULL			23	// [I]server側の辞書が一杯 2008.11.4 added
#define	NDERR_CONFLICT		24	// [I]衝突した 2008.11.7 added
#define	NDERR_EXIST			25	// [I]すでに同じ単語が存在する(=CONFLICT) 2008.11.7 added
#define	NDERR_NOANONYMOUS	26	// [I] 匿名接続は対応していない
#define	NDERR_VERSION		27	// [I]server rejected because the client is old.
#define	NDERR_SERVER_VER	28	// [I]server version is not acceptable
#define	NDERR_LIMIT			29	// [I]the length of the item is limited.
#define	NDERR_NOT_SERVER	30	// not PDIC server
#define	NDERR_CRYPT_REQ		31	// [I] 暗号化パスワード必要

#ifndef DEF_MERGEMODE
#define	DEF_MERGEMODE
enum MergeMode {
	MRG_LONG,
	MRG_IGN,
	MRG_REP,
	MRG_APP,
	MRG_APPCR,
	MRG_INQ,
	MRG_LEVEL,
	MRG_ANET,		// WLevelでの出力形式
	MRG_USER		// USER.DLLでマージを行なう
};
#endif


#endif
