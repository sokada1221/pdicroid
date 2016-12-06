#ifndef	__MULTIDEF_H
#define	__MULTIDEF_H

#if defined(SMALL)
#define	MAX_MULTIDIC	8
#else
#ifdef __ANDROID__
#define	MAX_MULTIDIC	30
#else
#define	MAX_MULTIDIC	100
#endif
#endif

#define	MD_PARA		0x0001		// 並列表示モード
#define	MD_MERGE	0x0002		// マージ表示モード
#define	MD_NOTMERGE	0x0004		// 非マージ表示モード
#define	MD_MERGE_MASK	(MD_PARA|MD_MERGE|MD_NOTMERGE)

#define	MD_DEFAULT	MD_NOTMERGE

#endif	// __MULTIDEF_H

