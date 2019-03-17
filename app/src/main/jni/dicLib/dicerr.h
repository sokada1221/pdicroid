#ifndef __DICERR_H
#define	__DICERR_H

#define	DICERR_OPEN_CREATE		1
#define	DICERR_INDEXMEMFULL		2
#define	DICERR_INDEXFULL		3
#define	DICERR_DATAFULL			4
#define	DICERR_READWRITE		5
#define	DICERR_DICVERSION		6
#define	DICERR_NOT_READY		7	// ネット共有辞書の準備中
#define	DICERR_ALREADY_OPEN		8
#define	DICERR_READONLY			9
#define	DICERR_TOOMANYOPEN		10
#define	DICERR_NOMEMORY			11
//#define DICERR_DUPDIC			12	// 同じ辞書を２つ以上指定している
#define	DICERR_DIFFVERSION		13
#define	DICERR_CREATE			14
//#define DICERR_DUALMULTI		15	// 分割辞書とマルチ辞書は併用できない
#define	DICERR_EXPIRED			15	// 使用期限が切れている
#define	DICERR_OPEN_NOCREATE	16
#define	DICERR_OPEN_SHARE		17
//#define DICERR_DISK			18	// disk error
#define	DICERR_NOTPDIC			19
#define	DICERR_NETWORK			20
//#define DICERR_OLEINIT		21	// OLE initialize error
#define DICERR_NOT_OPENED		22	// dictionary is not opened.(programming error)
#define	DICERR_READONLY_WRITE	23

#define	DICERR_WRITE_OLD		27
#define	DICERR_NOT_SUPPORT		28
#define	DICERR_TOOLONG			29

#define	DICERR_INCOMPTYPE		30		// incompatible type
#define	DICERR_CORRUPTED		31		// dictionary may be corrupted.

#define	DICERR_NO_DEPW			33
#define	DICERR_OPEN_EPWING		34

#define	DICERR_NOTEMP			36
#define DICERR_LONGWORD			37
#define	DICERR_HEADERSUM		38
#define	DICERR_NOLANGPROC		39

#define	DICERR_NODLL			41
#define	DICERR_NOURL			42
#define	DICERR_FORMATERROR		43		// for utydic (TSV/CSV)
#define	DICERR_GEN_CHANGED		44

#define	DICERR_PARAMETER		98

			//1:Openエラー（新規作成可能）
			//2:インデクス部・データバッファ用のメインメモリ容量不足エラー
			//3:インデクス部のディスク容量不足エラー,インデックス数不足エラー
			//4:データブロックのディスク容量不足エラー（seekエラーから判断）
			//5:読み・書きエラー
			//6:辞書のバージョンが違う
			//7:ドライブの準備が出来ていない
			//8:既にオープンされている
			//9:リードオンリイ属性のファイルをオープンしようとした
			//10:オープンファイルが多すぎる
			//11:メモリ容量不足
			//12:辞書が重複使用
			//13:タイプの異なる辞書を併用している
			//14:新規作成できませんでした
			//15:分割辞書とマルチ辞書は併用できません
			//16:オープンエラー（新規作成不可能）
			//17:共有違反、ロック違反
			//18:ディスクが読めない
			//19:PDIC用の辞書ではない
			//20:ネットワークエラーまたはドライブ名が間違っている
			//21:OLE2の初期化に失敗しました
			//22:同上（現在未使用）
			//23:リードオンリィのファイルに書き込もうとした
			//24:他で使用しているため、オープン出来ない（新規作成不可能）
			//25:現在他で処理しているため実行できない（再試行可能）＝LockRecord()あるいはLockFile()されている
			//26:25の拡張のために予約
			//27:古いバージョンに書き込めない(NEWDIC2)
			//28:サポートされていない辞書タイプ(NEWDIC2)
			//29:登録データが長すぎる(NEWDIC2)
			//30:異なる辞書タイプを同時に利用できない！
			//31:辞書が壊れている可能性がある
			//32:32bitインデックスは読み書きできない
			//33:DEPW.DLLが見つからない(EPWING用)
			//34:EPWINGファイルがオープンできない(EPwing用)
			//35:ファイル名に誤りがある
			//36:テンポラリファイルが作成できない(Network用)
			//37:英単語の長さが長すぎる
			//38:ヘッダのチェックサムが正しくない
			//39:対応するLangProcが見つからない
			//40:古い辞書(NEWDIC2以前)であるため実行できない
			//41:DLLが見つからない(InetDic.dll)
			//42:ネット共有辞書用のURLがありません
			//43:対応できないフォーマット
			//44:検索中にgenerationが変わった
			//98:不正オペレーションエラー（パラメータエラー）
			//99:未定義エラー（起こり得ないエラー？）
			//100:エラーでないエラー
			// 256～511 : JLINK関係のエラー

#endif	/* __DICERR_H */

