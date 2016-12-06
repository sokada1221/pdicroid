#ifndef __diclist_h
#define	__diclist_h

#include "multidef.h"

#pragma warn -8027

#if MAX_MULTIDIC <= 30
#define	DICLIST_ALL		0x7FFFFFFF
#define	DICLIST_EMPTY	0
#define	DICLIST_ERR		-1
struct _diclist_t {
	int list;
	_diclist_t(){ list = DICLIST_ALL; }
	_diclist_t(int _list){ list = _list; }
	inline int *array() { return &list; }
	inline int array_num() const { return 1; }
	inline void all()
	{
		list = DICLIST_ALL;
	}
	inline bool selected(int no)
	{
		return list & (1<<no);
	}
	inline void set(int no)
	{
		list |= (1<<no);
	}
	inline void reset(int no)
	{
		list &= ~(1<<no);
	}
	inline bool check(int no) const { return list & (1<<no) ? true : false; }
	inline bool is_empty() const { return list == 0; }
	inline bool is_all() const { return list == DICLIST_ALL; }
	inline bool is_error() const { return list == DICLIST_ERR; }
};
#define	USE_DL_STRUCT		0
#else	// 将来的には。。。
struct _diclist_t {
	unsigned list[(MAX_MULTIDIC+31)/32];
	unsigned char flag;
	enum { F_ERROR = 1 };

	_diclist_t()
	{
		all();
		flag = 0;
	}
	_diclist_t(unsigned val)
	{
		set_val(val);
		flag = 0;
	}
	_diclist_t(int dummy, bool error)
	{
		all();
		flag = error ? F_ERROR : 0;
	}
	inline int *array() const { return (int*)list; }
	inline int array_num() const { return (MAX_MULTIDIC+31)/32; }
	inline void all()
	{
		set_val(~0);
	}
	inline void set_val(unsigned val)
	{
		list[0] = 
#if MAX_MULTIDIC>32
		list[1] = 
#if MAX_MULTIDIC>64
		list[2] = 
#if MAX_MULTIDIC>96
		list[3] = 
#endif
#endif
#endif
			val;
	}
	inline void set_empty()
	{
		set_val(0);
	}
	inline bool selected(int no) const
	{
		return list[no>>5] & (1<<(no&0x1F));
	}
	inline void set(int no)
	{
		list[no>>5] |= (1<<(no&0x1F));
	}
	inline void reset(int no)
	{
		list[no>>5] &= ~(1<<(no&0x1F));
	}
	inline bool check(int no) const
	{
		return list[no>>5] & (1<<(no&0x1F)) ? true : false;
	}
	bool is_empty() const
	{
		for (int i=0;i<(MAX_MULTIDIC+31)/32;i++){
			if (list[i])
				return false;
		}
		return true;
	}
	bool is_all() const
	{
		for (int i=0;i<(MAX_MULTIDIC+31)/32;i++){
			if (list[i]!=~0)
				return false;
		}
		return true;
	}
	inline bool is_error() const { return flag & F_ERROR ? true : false; }
	bool is_selected(int numdics) const
	{
		for (int i=0;i<(numdics+31)/32;i++){
			if (list[i])
				return true;
		}
		return false;
	}
	_diclist_t operator = (const _diclist_t &o)
	{
		for (int i=0;i<(MAX_MULTIDIC+31)/32;i++){
			this->list[i] = o.list[i];
		}
		this->flag = o.flag;
		return *this;
	}
};
#define	USE_DL_STRUCT		1
extern _diclist_t dl_error_obj;
extern _diclist_t dl_all_obj;
extern _diclist_t dl_empty_obj;
#endif

#if USE_DL_STRUCT	// 構造体を利用する
typedef _diclist_t diclist_t;
#define	dl_def(list)			diclist_t list
#define	dl_def_empty(list)		diclist_t list(0)
#define	dl_array(list)			list.array()
#define	dl_num(list)			list.array_num()
#define	dl_val_all				dl_all_obj
#define	dl_val_empty			dl_empty_obj
#define	dl_val_err				dl_error_obj
#define	dl_init(list)
#define	dl_check(list, no)		list.check(no)
#define	dl_is_empty(list)		list.is_empty()
#define	dl_is_all(list)			list.is_all()
#define	dl_is_error(list)		list.is_error()
#define	dl_is_selected(list, numdics)	list.is_selected(numdics)
#define	dl_all(list)			list.all()
#define	dl_empty(list)			list.set_empty()
#define	dl_set(list, no)		list.set(no)
#else
typedef int diclist_t;
#define	dl_def(list)			diclist_t list = DICLIST_ALL
#define	dl_def_empty(list)		diclist_t list = DICLIST_EMPTY
#define	dl_array(list)			&list
#define	dl_num(list)			1
#define	dl_val_all				DICLIST_ALL
#define	dl_val_empty			DICLIST_EMPTY
#define	dl_val_err				DICLIST_ERR
#define	dl_init(list)			((list) = DICLIST_ALL)
#define	dl_check(list, no)		((list) & (1<<(no)) ? true : false)
#define	dl_is_empty(list)		((list)==0)
#define	dl_is_all(list)			((list)==DICLIST_ALL)
#define	dl_is_error(list)		((list)==DICLIST_ERR)
#define	dl_is_selected(list, numdics)	(((list) & ~(0xFFFFFFFF<<numdics)) ? true : false)
#define	dl_all(list)			((list) = DICLIST_ALL)
#define	dl_empty(list)			((list) = DICLIST_EMPTY)
#define	dl_set(list, no)		((list) |= (1<<(no)))
#endif

#endif

