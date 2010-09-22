

// template_pseudo_haskell.cpp 
// Author: "rumzeus" 
// This code is from here - 
// http://www.refactory.org/s/
// template_haskell_with_list_processing_integer_
// factorization_and_rational_numbers/view/latest  
// *********************************************
// As the Refactory page states, this code is 
// released to the public domain.  

 
#include<stdio.h>
 
namespace haskell
{
 
struct Null {};
 
template<int First_,typename Rest_>
struct List
{
    enum { First=First_ };
    typedef Rest_ Rest;
};
 
template<bool cond,typename THEN,typename ELSE>
struct IF
{
    typedef ELSE val;
};
 
template<typename THEN,typename ELSE>
struct IF<true,THEN,ELSE>
{
    typedef THEN val;
};
 
template<int min,int max,int step=1>
struct enumFromTo
{
    template<bool cond, int min,int max,int step>
    struct enumFromToHelper
    {
        typedef List<min, typename enumFromToHelper< (min+step<=max), min+step,max,step>::val> val;
    };
 
    template<int min,int max,int step>
    struct enumFromToHelper<false,min,max,step>
    {
        typedef Null val;
    };
 
    typedef typename enumFromToHelper<(min<=max),min,max,step>::val val;
};
 
template<typename L>
struct length
{
    enum { val=length<L::Rest>::val+1 };
};
 
template<>
struct length<Null>
{
    enum { val=0 };
};
 
//map f []=[]
//map f (x:s)=(f x) : (map f s)
template<template <int x> class f,typename l >
struct map
{
    typedef List<f<l::First>::val, typename map<f, typename l::Rest>::val > val;
};
 
template<template <int x> class f>
struct map<f, Null>
{
    typedef Null val;
};
 
//foldr f n [] =  n
//foldr f n (x:s) = f x (foldr f n s)
template<template <int x, int y> class f, int n, typename l>
struct foldr
{
    enum { val = f<l::First, foldr<f, n, typename l::Rest>::val >::val };
};
 
template<template <int x, int y> class f, int n>
struct foldr<f, n, Null>
{
    enum { val = n };   
};
 
//foldl f n [] =  n
//foldl f n (x:s) = foldr f (f n x) s
template<template <int x, int y> class f, int n, typename l>
struct foldl
{
    enum { val = foldr<f, f<n, l::First>::val, typename l::Rest>::val };
};
 
template<template <int x, int y> class f, int n>
struct foldl<f, n, Null>
{
    enum { val = n };   
};
 
//zipWith f [] _=  []
//zipWith f _ []=  []
//zipWith f (x:s) (y:t)=(f x y):(zipWidth f s t)
template<template <int x, int y> class f, typename l1, typename l2>
struct zipWith
{
    typedef List<f<l1::First,l2::First>::val, typename zipWith<f,typename l1::Rest,typename l2::Rest>::val> val;
};
 
template<template <int x, int y> class f, typename l1>
struct zipWith<f,l1,Null>
{
    typedef Null val;
};
 
template<template <int x, int y> class f, typename l2>
struct zipWith<f,Null,l2>
{
    typedef Null val;
};
 
template<template <int x, int y> class f>
struct zipWith<f,Null,Null>
{
    typedef Null val;
};
//append [] l2 = l2
//append (x:s) l2 = x:(append s l2)
template<typename l1, typename l2>
struct append
{
    typedef List<l1::First,typename append<typename l1::Rest, typename l2>::val > val;
};
 
template<typename l2>
struct append<Null, l2>
{
    typedef typename l2 val;
};
 
//take 0 _ = []
//take _ []= []
//take n (x:s) = x:(take (n-1) s)
template<int n, typename l>
struct take
{
    typedef List<l::First, typename take<n-1, typename l::Rest>::val> val;
};
 
template<typename l>
struct take<0, l>
{
    typedef Null val;
};
 
template<int n>
struct take<n, Null>
{
    typedef Null val;
};
 
//reverse [] h=h
//reverse x:s h= Reverse s (x:h)
template<typename L, typename Temp=Null>
struct reverse
{
    typedef typename reverse<typename L::Rest, List<L::First,Temp> >::val val;
};
 
template<typename Temp>
struct reverse<Null,Temp>
{
    typedef Temp val;
};
 
//filter p [] = []
//filter p (x:s)
//  | p x = x:(filter p s)
//  | otherwise = filter p s
template<template <int x> class p, typename l>
struct filter
{
    typedef typename IF<p<l::First>::val, List<l::First, typename filter<p,typename l::Rest>::val >,
                          typename filter<p,typename l::Rest>::val >::val val;
};
 
template<template <int x> class p>
struct filter<p, Null>
{
    typedef Null val;
};
 
//remove p [] = []
//remove p (x:s)
//  | p x = remove p s
//  | otherwise = x:(remove p s)
template<template <int x> class p, typename l>
struct remove
{
    typedef typename IF<p<l::First>::val,typename remove<p,typename l::Rest>::val,
        List<l::First, typename remove<p,typename l::Rest>::val > >::val val;
};
 
template<template <int x> class p>
struct remove<p, Null>
{
    typedef Null val;
};
 
 
// inc x = x+1
template<int x>
struct inc
{
    enum { val=x+1 };
};
 
// add x y= x+y
template<int x, int y>
struct add
{
    enum { val=x+y };
};
 
//even x = x `mod' 2 ==0
template<int x>
struct even
{
    enum { val=(x%2==0) };
};
 
//abs a=if a<0 then -a else a
template<int a>
struct abs
{
    enum { val=(a<0)? -a : a };
};
 
//sign 0=0
//sign a=if a<0 then -1 else 1
template<int a>
struct sign
{
    enum { val=(a<0)? -1 : 1 };
};
 
template<>
struct sign<0>
{
    enum { val=0 };
};
 
//gcd x y=gcd_ (abs x) (abs y)
//          where
//              gcd_ x 0 = x
//              gcd_ x y = gcd_ y (x `rem` y)
template<int a,int b>
struct gcd
{
    template<int a,int b>
    struct gcd_
    {
        typedef typename IF<(a<b), gcd_<b,a>, gcd_<a%b,b> >::val gcd_t;
        enum { val=gcd_t::val };
    };
 
    template<int a>
    struct gcd_<a,0>
    {
        enum { val=a };
    };
 
    enum { val=gcd_<abs<a>::val, abs<b>::val>::val };
};
 
//lcm _ 0=0
//lcm 0 _=0
//lcm 0 0=0
//lcm x y=abs ((x `quot` gcd x y) * y)
template<int a,int b>
struct lcm
{
    enum { val=abs<(a/gcd<a,b>::val)*b >::val };
};
 
template<int a>
struct lcm<a,0>
{
    enum { val=0 };
};
 
template<int b>
struct lcm<0,b>
{
    enum { val=0 };
};
 
template<>
struct lcm<0,0>
{
    enum { val=0 };
};
 
//factorize 0=[]
//factorize 1=[1]
//factorize n=if n<0 then (-first:rest) else (fac_from n 2)
//  where
//      (first:rest)=factorize (-n)
//      max=abs n
//      fac_from 1 _=[]
//      fac_from n s=if (s*s)>max then [n]
//                      else if (n `mod` s)==0 then (s:(fac_from (n `quot` s) s))
//                                  else fac_from n (s+1)
template<int n>
struct factorize
{
    enum { max=abs<n>::val };
 
    template<int n,int s>
    struct fac_from
    {
        template<bool with,int n, int s>
        struct fac_from_with
        {
            typedef List<s, typename fac_from<(n/s), s>::val> val;
        };
        template<int n, int s>
        struct fac_from_with<false,n,s>
        {
            typedef typename fac_from<n, s+1>::val val;
        };
 
        typedef typename IF<(s*s>max), typename enumFromTo<n,n>::val, typename fac_from_with<n%s==0, n, s>::val >::val val;
    };
 
    template<int s>
    struct fac_from<1,s>
    {
        typedef Null val;
    };
 
    template<bool cond,int n>
    struct pos_fac
    {
        typedef typename fac_from<n,2>::val val;
    };
 
    template<int n>
    struct pos_fac<false,n>
    {
        typedef typename factorize<-n>::val val_;
        typedef List<-val_::First,typename val_::Rest> val;
    };
 
    typedef typename pos_fac<(n>0), n>::val val;
};
 
template<>
struct factorize<0>
{
    typedef Null val;
};
 
template<>
struct factorize<1>
{
    typedef enumFromTo<1,1>::val val;
};
 
namespace meta_rational
{
 
template<int P,int Q>
struct rational
{
    template<int P,int Q>
    struct CanonicalRational
    {
        enum { Numerator=P, Denumerator=Q };
    };
    enum { s=sign<P>::val*sign<Q>::val };
    enum { gcd_=gcd<P,Q>::val };
    typedef typename CanonicalRational<s*abs<P>::val/gcd_,abs<Q>::val/gcd_> val;
};
 
template<int n>
struct to_rational
{
    typedef typename rational<n,1>::val val;
};
 
template<typename r>
struct floor
{
    enum { val=r::Numerator / r::Denumerator };
};
 
template<typename a>
struct neg
{
    typedef typename rational<-a::Numerator,a::Denumerator>::val val;
};
 
template<typename a,typename b>
struct add
{
    typedef typename rational<a::Numerator*b::Denumerator+a::Denumerator*b::Numerator, a::Denumerator*b::Denumerator>::val val;
};
 
template<typename a,typename b>
struct sub
{
    typedef typename rational<a::Numerator*b::Denumerator-a::Denumerator*b::Numerator, a::Denumerator*b::Denumerator>::val val;
};
 
template<typename a,typename b>
struct mul
{
    typedef typename rational<a::Numerator*b::Numerator, a::Denumerator*b::Denumerator>::val val;
};
 
template<typename a,typename b>
struct div
{
    typedef typename rational<a::Numerator*b::Denumerator, a::Denumerator*b::Numerator>::val val;
};
 
}//namespace meta_rational
 
}//namespace haskell
 
//----
template<typename L>
void PrintList()
{
    printf("%i ",L::First);
    PrintList<L::Rest>();
}
template<>
void PrintList<haskell::Null>()
{
    printf("\n");
}
 
template<typename R>
void PrintRational()
{
    printf("%i / %i\n",R::Numerator,R::Denumerator);
}
 
int main(void)
{
    {
        using namespace haskell;
 
        PrintList<List<2,List<1,Null> > >();
        PrintList<enumFromTo<1,10>::val >();
        PrintList<reverse<enumFromTo<1,10>::val >::val >();
        PrintList<reverse<enumFromTo<1,10,3>::val >::val >();
 
        PrintList<map<inc, enumFromTo<1,10>::val >::val >();
 
        PrintList<filter<even, enumFromTo<1,10>::val >::val >();
        PrintList<haskell::remove<even, enumFromTo<1,10>::val >::val >();
 
        PrintList<append<enumFromTo<1,10>::val,enumFromTo<1,10>::val>::val >();
 
        PrintList<zipWith<add,enumFromTo<1,10>::val,enumFromTo<1,10>::val>::val >();
 
        PrintList<take<3, enumFromTo<1,10>::val>::val >();
 
        int len=length<enumFromTo<1,10>::val >::val;
        printf("length = %i\n",len);
 
        printf("foldr add 0 [1..10]=%i\n",foldl<add, 0, enumFromTo<1,10>::val >::val);
 
        printf("gcd 1 3 = %i\n",gcd<1,3>::val);
        printf("gcd 2 3 = %i\n",gcd<2,3>::val);
        printf("gcd 5 3 = %i\n",gcd<5,3>::val);
        printf("gcd 6 18 = %i\n",gcd<6,18>::val);
        printf("gcd 18 6 = %i\n",gcd<18,6>::val);
        printf("gcd -30 36 = %i\n",gcd<-30,36>::val);
        printf("gcd 30 -36 = %i\n",gcd<30,-36>::val);
        printf("gcd 0 0 = %i\n",gcd<0,0>::val);
 
 
        printf("lcm 1 3 = %i\n",lcm<1,3>::val);
        printf("lcm 2 3 = %i\n",lcm<2,3>::val);
        printf("lcm 5 3 = %i\n",lcm<5,3>::val);
        printf("lcm 6 18 = %i\n",lcm<6,18>::val);
        printf("lcm 18 6 = %i\n",lcm<18,6>::val);
        printf("lcm -30 36 = %i\n",lcm<-30,36>::val);
        printf("lcm 30 -36 = %i\n",lcm<30,-36>::val);
        printf("lcm 0 0 = %i\n",lcm<0,0>::val);
 
        PrintList<factorize<-7>::val >();
        PrintList<factorize<-1>::val >();
        PrintList<factorize<0>::val >();
        PrintList<factorize<1>::val >();
        PrintList<factorize<7>::val >();
 
        PrintList<factorize<1*2*3*4*5*6*7>::val >();
        PrintList<factorize<-1*2*3*4*5*6*7>::val >();
    }
    {
        using namespace haskell::meta_rational;
 
        PrintRational<rational<0,2>::val >();
        PrintRational<rational<0,10>::val >();
 
        PrintRational<rational<2,10>::val >();
        PrintRational<rational<-2,10>::val >();
        PrintRational<rational<2,-10>::val >();
        PrintRational<rational<-2,-10>::val >();
        PrintRational<neg<rational<1,2>::val>::val >();
        PrintRational<add<rational<1,10>::val, rational<1,3>::val >::val >();
        PrintRational<sub<rational<1,10>::val, rational<1,3>::val >::val >();
 
        PrintRational<mul<rational<1,2>::val, rational<1,3>::val >::val >();
        PrintRational<haskell::meta_rational::div<rational<1,2>::val, rational<1,3>::val >::val >();
        PrintRational<to_rational<7>::val >();
        printf("floor(1/1)=%i\n",floor<rational<1,1>::val>::val );
        printf("floor(1/2)=%i\n",floor<rational<1,2>::val>::val );
        printf("floor(-1/2)=%i\n",floor<rational<-1,1>::val>::val );
 
        printf("floor(1099/10)=%i\n",floor<rational<1099,10>::val>::val );
    }
    return 0;
}



