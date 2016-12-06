//---------------------------------------------------------------------------

#ifndef DicAttrH
#define DicAttrH
//---------------------------------------------------------------------------

struct TDicAttr {
	int DicType;
	int RecItem;
	int TypeNameId;
};

// RecItem
// äÓñ{ìIÇ…SQM_Ç∆çáÇÌÇπÇƒÇ¢ÇÈ
#define	RI_JAPA		0x0001
#define	RI_EXP		0x0002
#define	RI_LEVEL	0x0004
#define	RI_PRON		0x0008
#define	RI_WORD		0x0010
//#define	RI_TITLE 0x0020
//#define	RI_FILENAME 0x0040
#define	RI_MODIFY	0x0080
//#define	RI_EPWING 0x0100
#define	RI_MEMORY	0x0200
#define	RI_FILELINK	0x1000

#endif
