#include	"tnlib.h"
#pragma	hdrstop
#include	"regs.h"

#ifdef USE_JRE
#define	CODE_JRE(c)		c
#else
#define	CODE_JRE(c)
#endif
#ifdef USE_REGEXPP
#define	CODE_REGEXPP(c)	c
#else
#define	CODE_REGEXPP(c)
#endif
#ifdef USE_BREGONIG
#define	CODE_BREGONIG(c)	c
#else
#define	CODE_BREGONIG(c)
#endif

Regs::Regs()
{
	CODE_JRE(jre = NULL;)
	CODE_REGEXPP(regexpp = NULL;)
	CODE_BREGONIG(bregonig = NULL;)

	next = NULL;
	connection = 0;
	_not = false;
}

Regs::~Regs()
{
	Close( );
	if (next)
		delete next;
}

template<class T>
T *open(T*obj)
{
	if (obj) return obj;
	obj = new T;
	if (obj->Open()) return obj;
	delete obj;
	return NULL;
}

void Regs::Open( )
{
	CODE_JRE( jre = open<JREObject>(jre); );
	CODE_REGEXPP( regexpp = open<TRegexpp>(regexpp); );
	CODE_BREGONIG( bregonig = open<TBregonig>(bregonig); );
}

void Regs::Close( )
{
	CODE_JRE( __delete(jre); );
	CODE_REGEXPP( __delete(regexpp); );
	CODE_BREGONIG( __delete(bregonig); );
}

void Regs::Discard( )
{
	CODE_JRE(jre = NULL;)
	CODE_REGEXPP(regexpp = NULL;)
	CODE_BREGONIG(bregonig = NULL;)
	if (next){
		next->Discard();
	}
}

bool Regs::IsOpened( )
{
#if defined(USE_JRE) || defined(USE_REGEXPP) || defined(USE_BREGONIG)
	return (
		CODE_JRE(jre != NULL)
		CODE_REGEXPP(regexpp!=NULL)
		CODE_BREGONIG(bregonig!=NULL)
		);
#else
	return false;
#endif
}

void Regs::Setup( UINT regtype, bool bUseRegular, bool bFuzzy, bool bIgnore, bool bWord )
{
#if 0
	regType = regtype;
	useRegular = bUseRegular;
	useFuzzy = bFuzzy;
	ignoreCase = bIgnore;
	wordAlign = bWord;
#endif

#ifdef USE_JRE
	if ( regtype != 0 || ( !bUseRegular && !bFuzzy ) ){
		__delete(jre);
	}
#endif
	if (!bUseRegular){
		CODE_REGEXPP( __delete(regexpp) );
		CODE_BREGONIG( __delete(bregonig) );
	}

#ifdef USE_JRE
	if ( jre ){
		jre->SetFlag( GRXP_FUZZY, bFuzzy );
		jre->SetFlag( GRXP_IGNORECASE, bIgnore );
	}
#endif
	CODE_REGEXPP( if (regexpp){ regexpp->SetFlag( GRXP_IGNORECASE, bIgnore ); } );
	CODE_BREGONIG( if (bregonig){ bregonig->SetFlag( GRXP_IGNORECASE, bIgnore ); } );
}

GenericRexp *Regs::GetRexp( )
{
	GenericRexp *rexp = NULL;
	CODE_JRE( if(jre) rexp = jre; );
	CODE_REGEXPP( if(regexpp) rexp = regexpp; );
	CODE_BREGONIG( if(bregonig) rexp = bregonig; );
	if (!rexp)
		return NULL;
	rexp->_not = _not;
	rexp->connection = connection;
	// copy link list //
	if (next){
		rexp->next = next->GetRexp();
	}
	return rexp;
}

#if 0	//TODO: ‚±‚±‚Å‚Í‚È‚­Atnlib2::GenericRexp‚Éimpl‚·‚×‚«‚¾‚Á‚½
// multithreadŒŸõ—p‚É•¡»‚ðs‚¤
Regs *Regs::Duplicate()
{
	Regs *regs = new Regs();
	regs->Open();
	if (!regs->IsOpened()){
		delete regs;
		return NULL;
	}
	regs->Setup( regType, useRegular, useFuzzy, ignoreCase, wordAlign );
	regs->SetPattern( GetPattern() );
	if (next){
		regs->next = next->Duplicate();
		if (!regs->next){
			delete regs;
			return NULL;
		}
		regs->connection = connection;
		regs->_not = _not;
	}
	return regs;
}
#endif

bool regb(const tchar *str, const tchar *pat)
{
	Regs regs;
	regs.Open();
	if (!regs.IsOpened())
		return false;
	regs.SetPattern(pat);
	GenericRexp *grexp = regs.GetRexp();
	if (!grexp)
		return false;
	if (!grexp->Compile())
		return false;
	return grexp->Compare(str);
}

tnstr_vec *reg(const tchar *str, const tchar *pat, bool case_ignore)
{
	Regs regs;
	regs.Open();
	if (!regs.IsOpened())
		return NULL;
	GenericRexp *grexp = regs.GetRexp();
	if (case_ignore){
		grexp->SetFlag( GRXP_IGNORECASE, true );
	}
	tnstr_vec *res = new tnstr_vec;
	if (!grexp->CompareEx(pat, str, *res)){
		delete res;
		return NULL;
	}
	return res;
}
