/*
 * lulesh.h
 *
 *  Created on: Apr 8, 2013
 *      Author: lagunaperalt1
 */

#ifndef LULESH_H_
#define LULESH_H_

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <math.h>

#include <vector>
#include <string>
#include <iostream>
#include <set>


//#ifdef NEW_STD
//#include <atomic>
//#endif

using namespace std;

#define LULESH_SHOW_PROGRESS 1

/*
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * ilaguna: recovery structures
 */
//#ifdef NEW_STD
//atomic<bool> _fault_ = false;
//#else
static bool _fault_ = false;
//#endif

//pthread_mutex_t _faultMutex_ = PTHREAD_MUTEX_INITIALIZER;

#define TRY bool _success_ = false;\
	        while(!_success_) { \
	        	try {

#define RECOVER _success_=true;\
			} catch (exception &_e_) {} }

template <class T, class P>
void _copy_array_(const T * const src, T * const dst, P size)
{
	P i;
	for (i = 0; i < size; ++i) {
		dst[i] = src[i];
	}
}

void _setFault_(int sign)
{
//#ifndef NEW_STD
//	assert(pthread_mutex_lock( &_faultMutex_) == 0);
//#endif
	_fault_ = true;
//#ifndef NEW_STD
//	assert(pthread_mutex_unlock( &_faultMutex_) == 0);
//#endif
}

void _unsetFault_(int sign)
{
//#ifndef NEW_STD
//	assert(pthread_mutex_lock( &_faultMutex_) == 0);
//#endif
	_fault_ = false;
//#ifndef NEW_STD
//	assert(pthread_mutex_unlock( &_faultMutex_) == 0);
//#endif
}

bool _thereIsAFault_()
{
	bool ret = false;
//#ifndef NEW_STD
//	assert(pthread_mutex_lock( &_faultMutex_) == 0);
//#endif
	ret = _fault_;
//#ifndef NEW_STD
//	assert(pthread_mutex_unlock( &_faultMutex_) == 0);
//#endif
	return ret;
}

static inline
void _checkFault_() {
	if (_thereIsAFault_()) {
		cout << "X";
		cout.flush();
		throw exception();
	}
}

/*
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 */

enum { VolumeError = -1, QStopError = -2 } ;

/****************************************************/
/* Allow flexibility for arithmetic representations */
/****************************************************/

/* Could also support fixed point and interval arithmetic types */
typedef float        real4 ;
typedef double       real8 ;
typedef long double  real10 ;  /* 10 bytes on x86 */

typedef int    Index_t ; /* array subscript and loop index */
typedef real8  Real_t ;  /* floating point representation */
typedef int    Int_t ;   /* integer representation */

inline real4  SQRT(real4  arg) { return sqrtf(arg) ; }
inline real8  SQRT(real8  arg) { return sqrt(arg) ; }
inline real10 SQRT(real10 arg) { return sqrtl(arg) ; }

inline real4  CBRT(real4  arg) { return cbrtf(arg) ; }
inline real8  CBRT(real8  arg) { return cbrt(arg) ; }
inline real10 CBRT(real10 arg) { return cbrtl(arg) ; }

inline real4  FABS(real4  arg) { return fabsf(arg) ; }
inline real8  FABS(real8  arg) { return fabs(arg) ; }
inline real10 FABS(real10 arg) { return fabsl(arg) ; }


/************************************************************/
/* Allow for flexible data layout experiments by separating */
/* array interface from underlying implementation.          */
/************************************************************/

typedef struct _Domain_ {

/* This first implementation allows for runnable code */
/* and is not meant to be optimal. Final implementation */
/* should separate declaration and allocation phases */
/* so that allocation can be scheduled in a cache conscious */
/* manner. */

public:

   /**************/
   /* Allocation */
   /**************/

   void AllocateNodalPersistent(size_t size)
   {
      m_x.resize(size) ;
      m_y.resize(size) ;
      m_z.resize(size) ;

      m_xd.resize(size, Real_t(0.)) ;
      m_yd.resize(size, Real_t(0.)) ;
      m_zd.resize(size, Real_t(0.)) ;

      m_xdd.resize(size, Real_t(0.)) ;
      m_ydd.resize(size, Real_t(0.)) ;
      m_zdd.resize(size, Real_t(0.)) ;

      m_fx.resize(size) ;
      m_fy.resize(size) ;
      m_fz.resize(size) ;

      m_nodalMass.resize(size, Real_t(0.)) ;
   }

   void AllocateElemPersistent(size_t size)
   {
      m_matElemlist.resize(size) ;
      m_nodelist.resize(8*size) ;

      m_lxim.resize(size) ;
      m_lxip.resize(size) ;
      m_letam.resize(size) ;
      m_letap.resize(size) ;
      m_lzetam.resize(size) ;
      m_lzetap.resize(size) ;

      m_elemBC.resize(size) ;

      m_e.resize(size, Real_t(0.)) ;

      m_p.resize(size, Real_t(0.)) ;
      m_q.resize(size) ;
      m_ql.resize(size) ;
      m_qq.resize(size) ;

      m_v.resize(size, 1.0) ;
      m_volo.resize(size) ;
      m_delv.resize(size) ;
      m_vdov.resize(size) ;

      m_arealg.resize(size) ;

      m_ss.resize(size) ;

      m_elemMass.resize(size) ;
   }

   /* Temporaries should not be initialized in bulk but */
   /* this is a runnable placeholder for now */
   void AllocateElemTemporary(size_t size)
   {
      m_dxx.resize(size) ;
      m_dyy.resize(size) ;
      m_dzz.resize(size) ;

      m_delv_xi.resize(size) ;
      m_delv_eta.resize(size) ;
      m_delv_zeta.resize(size) ;

      m_delx_xi.resize(size) ;
      m_delx_eta.resize(size) ;
      m_delx_zeta.resize(size) ;

      m_vnew.resize(size) ;
   }

   void AllocateNodesets(size_t size)
   {
      m_symmX.resize(size) ;
      m_symmY.resize(size) ;
      m_symmZ.resize(size) ;
   }

   /**********/
   /* Access */
   /**********/

   /* Node-centered */

   Real_t& x(Index_t idx)    { return m_x[idx] ; }
   Real_t& y(Index_t idx)    { return m_y[idx] ; }
   Real_t& z(Index_t idx)    { return m_z[idx] ; }

   Real_t& xd(Index_t idx)   { return m_xd[idx] ; }
   Real_t& yd(Index_t idx)   { return m_yd[idx] ; }
   Real_t& zd(Index_t idx)   { return m_zd[idx] ; }

   Real_t& xdd(Index_t idx)  { return m_xdd[idx] ; }
   Real_t& ydd(Index_t idx)  { return m_ydd[idx] ; }
   Real_t& zdd(Index_t idx)  { return m_zdd[idx] ; }

   Real_t& fx(Index_t idx)   { return m_fx[idx] ; }
   Real_t& fy(Index_t idx)   { return m_fy[idx] ; }
   Real_t& fz(Index_t idx)   { return m_fz[idx] ; }

   Real_t& nodalMass(Index_t idx) { return m_nodalMass[idx] ; }

   Index_t&  symmX(Index_t idx) { return m_symmX[idx] ; }
   Index_t&  symmY(Index_t idx) { return m_symmY[idx] ; }
   Index_t&  symmZ(Index_t idx) { return m_symmZ[idx] ; }

   /* Element-centered */

   Index_t&  matElemlist(Index_t idx) { return m_matElemlist[idx] ; }
   Index_t*  nodelist(Index_t idx)    { return &m_nodelist[Index_t(8)*idx] ; }

   Index_t&  lxim(Index_t idx) { return m_lxim[idx] ; }
   Index_t&  lxip(Index_t idx) { return m_lxip[idx] ; }
   Index_t&  letam(Index_t idx) { return m_letam[idx] ; }
   Index_t&  letap(Index_t idx) { return m_letap[idx] ; }
   Index_t&  lzetam(Index_t idx) { return m_lzetam[idx] ; }
   Index_t&  lzetap(Index_t idx) { return m_lzetap[idx] ; }

   Int_t&  elemBC(Index_t idx) { return m_elemBC[idx] ; }

   Real_t& dxx(Index_t idx)  { return m_dxx[idx] ; }
   Real_t& dyy(Index_t idx)  { return m_dyy[idx] ; }
   Real_t& dzz(Index_t idx)  { return m_dzz[idx] ; }

   Real_t& delv_xi(Index_t idx)    { return m_delv_xi[idx] ; }
   Real_t& delv_eta(Index_t idx)   { return m_delv_eta[idx] ; }
   Real_t& delv_zeta(Index_t idx)  { return m_delv_zeta[idx] ; }

   Real_t& delx_xi(Index_t idx)    { return m_delx_xi[idx] ; }
   Real_t& delx_eta(Index_t idx)   { return m_delx_eta[idx] ; }
   Real_t& delx_zeta(Index_t idx)  { return m_delx_zeta[idx] ; }

   Real_t& e(Index_t idx)          { return m_e[idx] ; }

   Real_t& p(Index_t idx)          { return m_p[idx] ; }
   Real_t& q(Index_t idx)          { return m_q[idx] ; }
   Real_t& ql(Index_t idx)         { return m_ql[idx] ; }
   Real_t& qq(Index_t idx)         { return m_qq[idx] ; }

   Real_t& v(Index_t idx)          { return m_v[idx] ; }
   Real_t& volo(Index_t idx)       { return m_volo[idx] ; }
   Real_t& vnew(Index_t idx)       { return m_vnew[idx] ; }
   Real_t& delv(Index_t idx)       { return m_delv[idx] ; }
   Real_t& vdov(Index_t idx)       { return m_vdov[idx] ; }

   Real_t& arealg(Index_t idx)     { return m_arealg[idx] ; }

   Real_t& ss(Index_t idx)         { return m_ss[idx] ; }

   Real_t& elemMass(Index_t idx)  { return m_elemMass[idx] ; }

   /* Params */

   Real_t& dtfixed()              { return m_dtfixed ; }
   Real_t& time()                 { return m_time ; }
   Real_t& deltatime()            { return m_deltatime ; }
   Real_t& deltatimemultlb()      { return m_deltatimemultlb ; }
   Real_t& deltatimemultub()      { return m_deltatimemultub ; }
   Real_t& stoptime()             { return m_stoptime ; }

   Real_t& u_cut()                { return m_u_cut ; }
   Real_t& hgcoef()               { return m_hgcoef ; }
   Real_t& qstop()                { return m_qstop ; }
   Real_t& monoq_max_slope()      { return m_monoq_max_slope ; }
   Real_t& monoq_limiter_mult()   { return m_monoq_limiter_mult ; }
   Real_t& e_cut()                { return m_e_cut ; }
   Real_t& p_cut()                { return m_p_cut ; }
   Real_t& ss4o3()                { return m_ss4o3 ; }
   Real_t& q_cut()                { return m_q_cut ; }
   Real_t& v_cut()                { return m_v_cut ; }
   Real_t& qlc_monoq()            { return m_qlc_monoq ; }
   Real_t& qqc_monoq()            { return m_qqc_monoq ; }
   Real_t& qqc()                  { return m_qqc ; }
   Real_t& eosvmax()              { return m_eosvmax ; }
   Real_t& eosvmin()              { return m_eosvmin ; }
   Real_t& pmin()                 { return m_pmin ; }
   Real_t& emin()                 { return m_emin ; }
   Real_t& dvovmax()              { return m_dvovmax ; }
   Real_t& refdens()              { return m_refdens ; }

   Real_t& dtcourant()            { return m_dtcourant ; }
   Real_t& dthydro()              { return m_dthydro ; }
   Real_t& dtmax()                { return m_dtmax ; }

   Int_t&  cycle()                { return m_cycle ; }

   Index_t&  sizeX()              { return m_sizeX ; }
   Index_t&  sizeY()              { return m_sizeY ; }
   Index_t&  sizeZ()              { return m_sizeZ ; }
   Index_t&  numElem()            { return m_numElem ; }
   Index_t&  numNode()            { return m_numNode ; }

private:

   /******************/
   /* Implementation */
   /******************/

   /* Node-centered */

   std::vector<Real_t> m_x ;  /* coordinates */
   std::vector<Real_t> m_y ;
   std::vector<Real_t> m_z ;

   std::vector<Real_t> m_xd ; /* velocities */
   std::vector<Real_t> m_yd ;
   std::vector<Real_t> m_zd ;

   std::vector<Real_t> m_xdd ; /* accelerations */
   std::vector<Real_t> m_ydd ;
   std::vector<Real_t> m_zdd ;

   std::vector<Real_t> m_fx ;  /* forces */
   std::vector<Real_t> m_fy ;
   std::vector<Real_t> m_fz ;

   std::vector<Real_t> m_nodalMass ;  /* mass */

   std::vector<Index_t> m_symmX ;  /* symmetry plane nodesets */
   std::vector<Index_t> m_symmY ;
   std::vector<Index_t> m_symmZ ;

   /* Element-centered */

   std::vector<Index_t>  m_matElemlist ;  /* material indexset */
   std::vector<Index_t>  m_nodelist ;     /* elemToNode connectivity */

   std::vector<Index_t>  m_lxim ;  /* element connectivity across each face */
   std::vector<Index_t>  m_lxip ;
   std::vector<Index_t>  m_letam ;
   std::vector<Index_t>  m_letap ;
   std::vector<Index_t>  m_lzetam ;
   std::vector<Index_t>  m_lzetap ;

   std::vector<Int_t>    m_elemBC ;  /* symmetry/free-surface flags for each elem face */

   std::vector<Real_t> m_dxx ;  /* principal strains -- temporary */
   std::vector<Real_t> m_dyy ;
   std::vector<Real_t> m_dzz ;

   std::vector<Real_t> m_delv_xi ;    /* velocity gradient -- temporary */
   std::vector<Real_t> m_delv_eta ;
   std::vector<Real_t> m_delv_zeta ;

   std::vector<Real_t> m_delx_xi ;    /* coordinate gradient -- temporary */
   std::vector<Real_t> m_delx_eta ;
   std::vector<Real_t> m_delx_zeta ;

   std::vector<Real_t> m_e ;   /* energy */

   std::vector<Real_t> m_p ;   /* pressure */
   std::vector<Real_t> m_q ;   /* q */
   std::vector<Real_t> m_ql ;  /* linear term for q */
   std::vector<Real_t> m_qq ;  /* quadratic term for q */

   std::vector<Real_t> m_v ;     /* relative volume */
   std::vector<Real_t> m_volo ;  /* reference volume */
   std::vector<Real_t> m_vnew ;  /* new relative volume -- temporary */
   std::vector<Real_t> m_delv ;  /* m_vnew - m_v */
   std::vector<Real_t> m_vdov ;  /* volume derivative over volume */

   std::vector<Real_t> m_arealg ;  /* characteristic length of an element */

   std::vector<Real_t> m_ss ;      /* "sound speed" */

   std::vector<Real_t> m_elemMass ;  /* mass */

   /* Parameters */

   Real_t  m_dtfixed ;           /* fixed time increment */
   Real_t  m_time ;              /* current time */
   Real_t  m_deltatime ;         /* variable time increment */
   Real_t  m_deltatimemultlb ;
   Real_t  m_deltatimemultub ;
   Real_t  m_stoptime ;          /* end time for simulation */

   Real_t  m_u_cut ;             /* velocity tolerance */
   Real_t  m_hgcoef ;            /* hourglass control */
   Real_t  m_qstop ;             /* excessive q indicator */
   Real_t  m_monoq_max_slope ;
   Real_t  m_monoq_limiter_mult ;
   Real_t  m_e_cut ;             /* energy tolerance */
   Real_t  m_p_cut ;             /* pressure tolerance */
   Real_t  m_ss4o3 ;
   Real_t  m_q_cut ;             /* q tolerance */
   Real_t  m_v_cut ;             /* relative volume tolerance */
   Real_t  m_qlc_monoq ;         /* linear term coef for q */
   Real_t  m_qqc_monoq ;         /* quadratic term coef for q */
   Real_t  m_qqc ;
   Real_t  m_eosvmax ;
   Real_t  m_eosvmin ;
   Real_t  m_pmin ;              /* pressure floor */
   Real_t  m_emin ;              /* energy floor */
   Real_t  m_dvovmax ;           /* maximum allowable volume change */
   Real_t  m_refdens ;           /* reference density */

   Real_t  m_dtcourant ;         /* courant constraint */
   Real_t  m_dthydro ;           /* volume change constraint */
   Real_t  m_dtmax ;             /* maximum allowable time increment */

   Int_t   m_cycle ;             /* iteration count for simulation */

   Index_t   m_sizeX ;           /* X,Y,Z extent of this block */
   Index_t   m_sizeY ;
   Index_t   m_sizeZ ;

   Index_t   m_numElem ;         /* Elements/Nodes in this domain */
   Index_t   m_numNode ;
} Domain;

/* Stuff needed for boundary conditions */
/* 2 BCs on each of 6 hexahedral faces (12 bits) */
#define XI_M        0x003
#define XI_M_SYMM   0x001
#define XI_M_FREE   0x002

#define XI_P        0x00c
#define XI_P_SYMM   0x004
#define XI_P_FREE   0x008

#define ETA_M       0x030
#define ETA_M_SYMM  0x010
#define ETA_M_FREE  0x020

#define ETA_P       0x0c0
#define ETA_P_SYMM  0x040
#define ETA_P_FREE  0x080

#define ZETA_M      0x300
#define ZETA_M_SYMM 0x100
#define ZETA_M_FREE 0x200

#define ZETA_P      0xc00
#define ZETA_P_SYMM 0x400
#define ZETA_P_FREE 0x800

extern Domain domain;

void checkCorrectness()
{
	printf ("\n\nIteration count = %i\n", domain.cycle());
	printf("Final Origin Energy = %12.6e \n", domain.e(0));

	Real_t MaxAbsDiff = Real_t(0.0);
	Real_t TotalAbsDiff = Real_t(0.0);
	Real_t MaxRelDiff = Real_t(0.0);

	Index_t edgeElems = domain.sizeX();
	for(Index_t j=0; j < edgeElems; ++j) {
		for(Index_t k=j+1; k < edgeElems; ++k) {
			Index_t a = j * edgeElems+k;
			Index_t b = k * edgeElems+j;
			Real_t AbsDiff = fabs(domain.e((int)a) - domain.e((int)b));
			TotalAbsDiff += AbsDiff;
			if (MaxAbsDiff < AbsDiff)
				MaxAbsDiff = AbsDiff;
			Real_t RelDiff = AbsDiff / domain.e((int) (k*edgeElems+j));
			if (MaxRelDiff<RelDiff)
				MaxRelDiff = RelDiff;
		}
	}
	printf("Testing Plane 0 of EnergyArray:\n");
	printf("	MaxAbsDiff = %12.6e\n", MaxAbsDiff);
	printf("	TotalAbsDiff = %12.6e\n",TotalAbsDiff);
	printf("	MaxRelDiff = %12.6e\n", MaxRelDiff);
}


#endif /* LULESH_H_ */
