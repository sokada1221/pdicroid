#ifndef	__ENV_H
#define	__ENV_H

//	コード生成制御フラグ


#if !defined(DOS) || defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__) || defined(WIN32)
//#define FARPTR
#define	USE_INLINE	0		// まだ、FARバージョンのインラインがうまく行かない(prevWord()でUAE)
#else
#define	USE_INLINE	1				//インラインアセンブラにする場合
#endif

#define	PUSH_SIDI
#define	POP_SIDI



#endif	// __ENV_H
