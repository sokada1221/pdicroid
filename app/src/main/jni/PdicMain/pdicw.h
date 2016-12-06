#ifndef	__PDICW_H
#define	__PDICW_H

#define	SF_POPUPSRCH	0x0001

#ifndef SMALL
extern bool delwithfile;	// ’PŒêíœ‚ÌAfile link‚àˆê‚Éíœ‚·‚é
#endif

#ifndef SMALL
extern uint modc;
extern uint delc;
extern uint appc;
#endif

extern HINSTANCE hInstance;

#ifdef SMALL
#else	// !SMALL
extern bool fWindowsVista;
extern bool fWow64;
extern bool AutoViewMode;
#endif	// !SMALL

#endif	// __PDICW_H

