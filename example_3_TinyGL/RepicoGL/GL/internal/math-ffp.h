#ifndef MATH_ffp
#define MATH_ffp

#include "GL/internal/math-sll.h"

// #ifdef USE_FIXED_POINT

typedef float ffp;

#define int2ffp(X)	((float)(X))
#define ffp2int(X)	((int)(X))
#define ffpneg(X)	(-(X))
#define sll2ffp(X)	((float)sll2dbl(X))
#define ffp2sll(X)	(dbl2sll((double)(X)))
#define ffpadd(X,Y)	((X)+(Y))
#define ffpsub(X,Y)	((X)-(Y))
#define ffpmul(X,Y)	((X)*(Y))
#define ffpdiv(X,Y)	((X)/(Y))
#define ffp2dbl(X)	((double)(X))

/*
static inline ffp int2ffp(int a)
{
	return ((ffp)a);
}

static inline int ffp2int(ffp a)
{
	return ((int)a);
}

static inline ffp ffpneg(ffp a)
{
	return -a;
}

static inline ffp sll2ffp(sll a)
{
	return ((ffp)sll2dbl(a));
}

static inline sll ffp2sll(ffp a)
{
	return (dbl2sll((double)a));
}


static inline ffp dbl2ffp(double a)
{
	return ((ffp)a);
}

static inline double ffp2dbl(ffp a)
{
	return ((double)a);
}

static inline ffp ffpadd(ffp a, ffp b)
{
	return (a+b);
}

static inline ffp ffpsub(ffp a, ffp b)
{
	return (a-b);
}

static inline ffp ffpmul(ffp a, ffp b)
{
	return (a*b);
}

static inline ffp ffpdiv(ffp a, ffp b)
{
	return (a/b);
}

*/


/*
typedef struct{
	long long value;
	double dvalue;
	int ivalue;
}ffp;


static inline ffp int2ffp(int a)
{
	ffp ret;

	ret.value=(long long)a;
	ret.dvalue=(double)a;
	ret.ivalue=a;

	return ret;
}

static inline int ffp2int(ffp a)
{
	return ((int)a.value);
}

static inline ffp ffpneg(ffp a)
{
	ffp ret;

	ret.value=-a.value;
	ret.ivalue=-a.ivalue;
	ret.ivalue=-a.ivalue;

	return ret;
}

static inline ffp sll2ffp(sll a)
{
	ffp ret;

	ret.value=(long long)sll2int(a);
	ret.ivalue=sll2int(a);
	ret.dvalue=sll2dbl(a);

	return ret;
}

static inline sll ffp2sll(ffp a)
{
//	sll ret=int2sll((int)a.value);
	sll ret=dbl2sll(a.dvalue);
	return ret;
}

static inline ffp dbl2ffp(double a)
{
	ffp ret;
	ret.value=(long long)a;
	ret.ivalue=(int)a;
	ret.dvalue=a;
	return ret;
}

static inline double ffp2dbl(ffp a)
{
	double ret;
	ret=(double)a.value;
	return (ret);
}


static inline ffp ffpadd(ffp a, ffp b)
{
	ffp ret;
	ret.value=a.value+b.value;
	ret.ivalue=a.ivalue+b.ivalue;
	ret.dvalue=a.dvalue+b.dvalue;
	return ret;
}

static inline ffp ffpsub(ffp a, ffp b)
{
	ffp ret;

	ret.value=a.value-b.value;
	ret.ivalue=a.ivalue-b.ivalue;
	ret.dvalue=a.dvalue-b.dvalue;

	return ret;
}

static inline ffp ffpmul(ffp a, ffp b)
{
	ffp ret;

	ret.value=a.value*b.value;
	ret.ivalue=a.ivalue*b.ivalue;
	ret.dvalue=a.dvalue*b.dvalue;

	return ret;
}

static inline ffp ffpdiv(ffp a, ffp b)
{
	ffp ret;

	if ((b.value) && (b.ivalue) && (b.dvalue!=0.0))
	{
		ret.value=a.value/b.value;
		ret.ivalue=a.ivalue/b.ivalue;
		ret.dvalue=a.dvalue/b.dvalue;
	}
	else
	{
		ret.value=0;
		ret.ivalue=0;
		ret.dvalue=0;
	}

	return ret;
}
*/



/*
#else


typedef long long ffp;

#define int2ffp(X)	(((ffp) (X)) << 16)
#define ffpvalue(X)	(X)
#define ffp2int(X)	((int) ((X) >> 16))
#define ffp_abs(X)	((X) & 0xeffpffpffpffpffpLL)
#define ffpint(X)	((X) & 0xffpffpffpffp0000LL)
#define ffpfrac(X)	((X) & 0x000000000000ffpfLL)
#define ffpneg(X)	(-(X))
#define ffpadd(X,Y)	((X) + (Y))
#define ffpsub(X,Y)	((X) - (Y))
#define ffp2sll(X)	((sll) (X<<16))
#define sll2ffp(X)	((ffp) (X>>16))


// Constants (converted from double) 
#define FFP_CONST_0		0x0000000000000000LL
#define FFP_CONST_1		0x0000000000010000LL
#define FFP_CONST_2		0x0000000000020000LL
#define FFP_CONST_3		0x0000000000030000LL
#define FFP_CONST_4		0x0000000000040000LL
#define FFP_CONST_10		0x00000000000a0000LL
#define FFP_CONST_1_2		0x0000000000008000LL
#define FFP_CONST_1_3		0x0000000000005555LL
#define FFP_CONST_1_4		0x0000000000004000LL
#define FFP_CONST_1_5		0x0000000000003333LL
#define FFP_CONST_1_6		0x0000000000002aaaLL
#define FFP_CONST_1_7		0x0000000000002492LL
#define FFP_CONST_1_8		0x0000000000002000LL
#define FFP_CONST_1_9		0x0000000000001c71LL
#define FFP_CONST_1_10		0x0000000000001999LL
#define FFP_CONST_1_11		0x0000000000001745LL
#define FFP_CONST_1_12		0x0000000000001555LL
#define FFP_CONST_1_20		0x0000000000000cccLL
#define FFP_CONST_1_30		0x0000000000000888LL
#define FFP_CONST_1_42		0x0000000000000618LL
#define FFP_CONST_1_56		0x0000000000000492LL
#define FFP_CONST_1_72		0x000000000000038eLL
#define FFP_CONST_1_90		0x00000000000002d8LL
#define FFP_CONST_1_110		0x0000000000000253LL
#define FFP_CONST_1_132		0x00000000000001f0LL
#define FFP_CONST_1_156		0x00000000000001a4LL
#define FFP_CONST_E		0x000000000002b7e1LL
#define FFP_CONST_1_Ea		0x0000000000005e2dLL
#define FFP_CONST_SQRTE		0x000000000001a612LL
#define FFP_CONST_1_SQRTE	0x0000000000009b45LL
#define FFP_CONST_LOG2_E	0x0000000000017154LL
#define FFP_CONST_LOG10_E	0x0000000000006f2dLL
#define FFP_CONST_LN2		0x000000000000b172LL
#define FFP_CONST_LN10		0x0000000000024d76LL
#define FFP_CONST_PI		0x000000000003243fLL
#define FFP_CONST_PI_2		0x000000000001921fLL
#define FFP_CONST_PI_4		0x000000000000c90fLL
#define FFP_CONST_1_PI		0x000000000000517cLL
#define FFP_CONST_2_PI		0x000000000000a2f9LL
#define FFP_CONST_2_SQRTPI	0x00000000000120ddLL
#define FFP_CONST_SQRT2		0x0000000000016a09LL
#define FFP_CONST_1_SQRT2	0x000000000000b504LL


static __inline__ ffp dbl2ffp(double a)
{
	sll x;
	sll ret;
	if (a>0.0)
	{
		x=(a/4294967296.0);
		ret=dbl2sll(a - (((double)x)*4294967296.0));
		return (x<<48)|sll2ffp(ret);
	}

	x=((-a)/4294967296.0);
	ret=dbl2sll((-a) - (((double)x)*4294967296.0));
	return -((x<<48)|sll2ffp(ret));
}

static __inline double ffp2dbl(ffp a)
{
	sll x;
	double ret;
	double sum;
	if (a&0x8000000000000000)
	{
		x=ffp2sll(a);
		if (x>0)
			ret=sll2dbl(-x);
		else
			ret=sll2dbl(x);
		sum=-((-a)>>48);
		return (ret+(sum*4294967296.0));
	}
	
	x=ffp2sll((a&0x7FFFFFFFFFFF));
	ret=sll2dbl(x);
	sum=a>>47;
	return ret+(sum*2147483648.0);

}

static __inline__ ffp ffpmul(ffp a, ffp b)
{
	unsigned short a_lo, b_lo;
	sll a_hi, b_hi;
	sll x;

	a_lo = a; // & 0xFFPF;
	a_hi = (sll) a >> 16;
	b_lo = b; // & 0xFFPF;
	b_hi = (sll) b >> 16;

	x = ((ull) (a_hi * b_hi) << 16)
	  + (((ull) a_lo * b_lo) >> 16)
	  + (sll) a_lo * b_hi
	  + (sll) b_lo * a_hi;

	return (ffp)x;
}


#define ffpinv(X) (sll2ffp(sllinv(ffp2sll(X))))

static __inline__ ffp ffpdiv(ffp left, ffp right)
{
	return ffpmul(left, ffpinv(right));
}

#endif

*/

#endif
