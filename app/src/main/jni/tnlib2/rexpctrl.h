#ifndef	__REXPCTRL_H
#define	__REXPCTRL_H

#include	"rexpgen.h"

#include	"rexpdll.h"

//	RexpObjectのインスタンスは複数存在しても構わない（？？JRE.DLLで対応しているかどうかも問題であるが）
//	このオブジェクトはなるべく寿命を短くした方が良い（JRE.DLLの仕様による？）

class RexpObject : public GenericRexp {
	protected:
		static HINSTANCE hinstRexp;		// REXPDLLインスタンスハンドル
		static int nInstance;			// このオブジェクトのインスタンスカウンター
		static LPRECOMP lpfnReComp;
		static LPREEXEC lpfnReExec;
		static LPREIGNORECASE lpfnReIgnoreCase;
		static LPREERRNO lpfnReErrno;

		int error;
	public:
		RexpObject( );
		~RexpObject( );

		WORD GetVersion( )
			{ return 0; }

		// 必ずOpen-Closeの対で使用する事
		BOOL Open( );
		void Close( );
		virtual BOOL CanUse( )
			{ return hinstRexp != NULL; }

		virtual BOOL CompileImpl( const TCHAR *patern );
		virtual BOOL CompareImpl( const TCHAR *str );

		virtual BOOL GetMatchImpl( int &len, int &loc )	// len : マッチ長 loc : マッチ位置
		{
			return FALSE;
		}

		virtual int GetErrorCode( );

		void SetIgnoreCase( BOOL f )
		{
			SetFlag( GRXP_IGNORECASE, f );
		}
		static int AbleFlag( )
			{ return GRXP_IGNORECASE; }
};

#endif	// __REXPCTRL_H
