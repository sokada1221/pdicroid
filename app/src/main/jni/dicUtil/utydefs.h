#ifndef __UTYDEFS_H
#define	__UTYDEFS_H

#define	FOM_READONLY		1
#define	FOM_CREATE			2
#define	FOM_WRITE			3
#define	FOM_OPENMODEMASK	3
#define	FOM_MERGE			8		// Windows版でCanOpen()の問合わせ無しの指示
#define	FOM_NOTREADOLE		0x10	// OLEデータは転送しない
#define	FOM_NOTREADFILE		0x20	// ファイルリンクデータは転送しない
#define	FOM_NOTREADOBJ		0x30	// リンクオブジェクトデータは転送しない
#define	FOM_FILE			0x100	// File
#define	FOM_CLIPBOARD		0x200	// Clipboard
#define	FOM_EDITCONTROL		0x400	// Edit Control
#define	FOM_CHAR			0x800	// tnstr型へ
#define	FOM_REFMODE			0x1000	// 辞書オブジェクトをリファレンスでオープンする(現在Pdic2のみ)
									// リファレンスモードでオープンするとDictionaryクラスでClose()しても
                                    // Closeされない！！
#define	FOM_RICHEDITCONTROL	0x2000	// Rich Edit Control

#define STR_PDIC	"PDIC V5形式"
#define STR_PERD12	"PDICﾃｷｽﾄ形式"
#define STR_WX2		"ＷＸII(+)(ﾃｷｽﾄ形式)"
#define STR_DOSDIC	"DosDic辞書"
#define	STR_WLEVEL	"単語レベルリスト"
#define	STR_EXTPDICTEXT	"1行ﾃｷｽﾄ形式"
#define	STR_FENG5	"FENG V5標準形式"
#define	STR_RTF		"RTF(RichTextFormat)"
#define	STR_USER	"ﾕｰｻﾞ定義"

// Profileでも使用するため数値の変更は不可(最大0x8000まで - TRS_とダブる)
#define	CF_DISTINCT		0x0001		// 日本語訳 / 用例を区別して読み取る
#define	CF_REMOVEPART	0x0002		// 見出語部の品詞を削除
#define	CF_PARTLABELING	0x0004		// 見出語部の品詞は訳のﾗﾍﾞﾘﾝｸﾞへ
#define	CF_DIVWORD		0x0008		// セミコロンで区切られた英単語は分ける
#define	CF_CONVDELIM	0x0010		// 日本語訳中の◆は、or,に変換
#define	CF_ZEN2HAN		0x0020		// 全角>>半角変換(FENGでのみ影響がある - 通常はMergeで変換を行う)
#define	CF_COMP1		0x0040		// 格納しきれない場合のみ圧縮
#define	CF_COMP2		0x0080		// なるべく圧縮する
#define	CF_COMPEXP		0x0100		// 用例も圧縮
//#define	CF_TRANSOBJ		0x0200		// オブジェクトも転送（削除）
#define	CF_CONVWAVE		0x0200		// 【音声】をファイルリンクに変換
#define	CF_CONVLEVEL	0x0400		// 【レベル】数値 をレベルに変換
#define	CF_CONVPRON		0x0800		// 【発音】を発音記号に登録
#define	CF_REMOVEATMARK	0x1000		// 【＠】
#define	CF_ELIMKANA		0x2000		// {ひらがな}を削除
#define	CF_LINEFEED		0x4000		// 品詞で改行する
#define	CF_CONVCRLF		0x8000		// ■は改行に変換

// Profileでも使用するため数値の変更は不可(32bit利用可能)
#define	CF2_FASTDB_CREATE	0x00000001	// FastDBを新規作成

// CF2_xxxの一時使用 (値の変更は確定するまで可能）
#define	CF2_ELIM_ILLCHAR	0x00000010	// \tなど不正文字を削除


#ifndef DEF_MERGEMODE
#define	DEF_MERGEMODE
enum MergeMode {
	MRG_MERGE,
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

// VCLのmrXXXXとぶつからないように設定
enum MergeSelect {
	mrMergeJapa = 0x200,
	mrNext,
	mrAuto,
	mrReplace,
	mrMerge,
//	mrAdvanced,
	mrLast
};

#endif	// __UTYDEFS_H
