// This code fragment is from IntegrateStressForElems (and child routines)

typedef float        real4 ;
typedef double       real8 ;
typedef long double  real10 ;  /* 10 bytes on x86 */
 
typedef int    Index_t ; /* array subscript and loop index */
typedef real8  Real_t ;  /* floating point representation */
typedef int    Int_t ;   /* integer representation */
 

static inline
void SumElemFaceNormal(Real_t *normalX0, Real_t *normalY0, Real_t *normalZ0,
                       Real_t *normalX1, Real_t *normalY1, Real_t *normalZ1,
                       Real_t *normalX2, Real_t *normalY2, Real_t *normalZ2,
                       Real_t *normalX3, Real_t *normalY3, Real_t *normalZ3,
                       const Real_t x0, const Real_t y0, const Real_t z0,
                       const Real_t x1, const Real_t y1, const Real_t z1,
                       const Real_t x2, const Real_t y2, const Real_t z2,
                       const Real_t x3, const Real_t y3, const Real_t z3)
{
   Real_t bisectX0 = Real_t(0.5) * (x3 + x2 - x1 - x0);
   Real_t bisectY0 = Real_t(0.5) * (y3 + y2 - y1 - y0);
   Real_t bisectZ0 = Real_t(0.5) * (z3 + z2 - z1 - z0);
   Real_t bisectX1 = Real_t(0.5) * (x2 + x1 - x3 - x0);
   Real_t bisectY1 = Real_t(0.5) * (y2 + y1 - y3 - y0);
   Real_t bisectZ1 = Real_t(0.5) * (z2 + z1 - z3 - z0);
   Real_t areaX = Real_t(0.25) * (bisectY0 * bisectZ1 - bisectZ0 * bisectY1);
   Real_t areaY = Real_t(0.25) * (bisectZ0 * bisectX1 - bisectX0 * bisectZ1);
   Real_t areaZ = Real_t(0.25) * (bisectX0 * bisectY1 - bisectY0 * bisectX1);

   *normalX0 += areaX;
   *normalX1 += areaX;
   *normalX2 += areaX;
   *normalX3 += areaX;

   *normalY0 += areaY;
   *normalY1 += areaY;
   *normalY2 += areaY;
   *normalY3 += areaY;

   *normalZ0 += areaZ;
   *normalZ1 += areaZ;
   *normalZ2 += areaZ;
   *normalZ3 += areaZ;
}

static inline
void SumElemStressesToNodeForces( const Real_t B[][8],
                                  const Real_t stress_xx,
                                  const Real_t stress_yy,
                                  const Real_t stress_zz,
                                  Real_t* const fx,
                                  Real_t* const fy,
                                  Real_t* const fz )
{
  Real_t pfx0 = B[0][0] ;   Real_t pfx1 = B[0][1] ;
  Real_t pfx2 = B[0][2] ;   Real_t pfx3 = B[0][3] ;
  Real_t pfx4 = B[0][4] ;   Real_t pfx5 = B[0][5] ;
  Real_t pfx6 = B[0][6] ;   Real_t pfx7 = B[0][7] ;

  Real_t pfy0 = B[1][0] ;   Real_t pfy1 = B[1][1] ;
  Real_t pfy2 = B[1][2] ;   Real_t pfy3 = B[1][3] ;
  Real_t pfy4 = B[1][4] ;   Real_t pfy5 = B[1][5] ;
  Real_t pfy6 = B[1][6] ;   Real_t pfy7 = B[1][7] ;

  Real_t pfz0 = B[2][0] ;   Real_t pfz1 = B[2][1] ;
  Real_t pfz2 = B[2][2] ;   Real_t pfz3 = B[2][3] ;
  Real_t pfz4 = B[2][4] ;   Real_t pfz5 = B[2][5] ;
  Real_t pfz6 = B[2][6] ;   Real_t pfz7 = B[2][7] ;

  fx[0] = -( stress_xx * pfx0 );
  fx[1] = -( stress_xx * pfx1 );
  fx[2] = -( stress_xx * pfx2 );
  fx[3] = -( stress_xx * pfx3 );
  fx[4] = -( stress_xx * pfx4 );
  fx[5] = -( stress_xx * pfx5 );
  fx[6] = -( stress_xx * pfx6 );
  fx[7] = -( stress_xx * pfx7 );

  fy[0] = -( stress_yy * pfy0  );
  fy[1] = -( stress_yy * pfy1  );
  fy[2] = -( stress_yy * pfy2  );
  fy[3] = -( stress_yy * pfy3  );
  fy[4] = -( stress_yy * pfy4  );
  fy[5] = -( stress_yy * pfy5  );
  fy[6] = -( stress_yy * pfy6  );
  fy[7] = -( stress_yy * pfy7  );

  fz[0] = -( stress_zz * pfz0 );
  fz[1] = -( stress_zz * pfz1 );
  fz[2] = -( stress_zz * pfz2 );
  fz[3] = -( stress_zz * pfz3 );
  fz[4] = -( stress_zz * pfz4 );
  fz[5] = -( stress_zz * pfz5 );
  fz[6] = -( stress_zz * pfz6 );
  fz[7] = -( stress_zz * pfz7 );
}

static inline
void CalcElemNodeNormals(Real_t pfx[8],
                         Real_t pfy[8],
                         Real_t pfz[8],
                         const Real_t x[8],
                         const Real_t y[8],
                         const Real_t z[8])
{
   for (Index_t i = 0 ; i < 8 ; ++i) {
      pfx[i] = Real_t(0.0);
      pfy[i] = Real_t(0.0);
      pfz[i] = Real_t(0.0);
   }
   /* evaluate face one: nodes 0, 1, 2, 3 */
   SumElemFaceNormal(&pfx[0], &pfy[0], &pfz[0],
                  &pfx[1], &pfy[1], &pfz[1],
                  &pfx[2], &pfy[2], &pfz[2],
                  &pfx[3], &pfy[3], &pfz[3],
                  x[0], y[0], z[0], x[1], y[1], z[1],
                  x[2], y[2], z[2], x[3], y[3], z[3]);
   /* evaluate face two: nodes 0, 4, 5, 1 */
   SumElemFaceNormal(&pfx[0], &pfy[0], &pfz[0],
                  &pfx[4], &pfy[4], &pfz[4],
                  &pfx[5], &pfy[5], &pfz[5],
                  &pfx[1], &pfy[1], &pfz[1],
                  x[0], y[0], z[0], x[4], y[4], z[4],
                  x[5], y[5], z[5], x[1], y[1], z[1]);
   /* evaluate face three: nodes 1, 5, 6, 2 */
   SumElemFaceNormal(&pfx[1], &pfy[1], &pfz[1],
                  &pfx[5], &pfy[5], &pfz[5],
                  &pfx[6], &pfy[6], &pfz[6],
                  &pfx[2], &pfy[2], &pfz[2],
                  x[1], y[1], z[1], x[5], y[5], z[5],
                  x[6], y[6], z[6], x[2], y[2], z[2]);
   /* evaluate face four: nodes 2, 6, 7, 3 */
   SumElemFaceNormal(&pfx[2], &pfy[2], &pfz[2],
                  &pfx[6], &pfy[6], &pfz[6],
                  &pfx[7], &pfy[7], &pfz[7],
                  &pfx[3], &pfy[3], &pfz[3],
                  x[2], y[2], z[2], x[6], y[6], z[6],
                  x[7], y[7], z[7], x[3], y[3], z[3]);
   /* evaluate face five: nodes 3, 7, 4, 0 */
   SumElemFaceNormal(&pfx[3], &pfy[3], &pfz[3],
                  &pfx[7], &pfy[7], &pfz[7],
                  &pfx[4], &pfy[4], &pfz[4],
                  &pfx[0], &pfy[0], &pfz[0],
                  x[3], y[3], z[3], x[7], y[7], z[7],
                  x[4], y[4], z[4], x[0], y[0], z[0]);
   /* evaluate face six: nodes 4, 7, 6, 5 */
   SumElemFaceNormal(&pfx[4], &pfy[4], &pfz[4],
                  &pfx[7], &pfy[7], &pfz[7],
                  &pfx[6], &pfy[6], &pfz[6],
                  &pfx[5], &pfy[5], &pfz[5],
                  x[4], y[4], z[4], x[7], y[7], z[7],
                  x[6], y[6], z[6], x[5], y[5], z[5]);
}


void kernel2(Index_t numElem, const Index_t * const *elemsToNodesConnectivity,
             const Real_t *x, const Real_t *y, const Real_t *z,
	     Real_t *fx, Real_t *fy, Real_t *fz,
	     Real_t *sigxx, Real_t *sigyy, Real_t *sigzz 
	     ) {

  Real_t B[3][8] ;
  Real_t x_local[8] ;
  Real_t y_local[8] ;
  Real_t z_local[8] ;
  Real_t fx_local[8] ;
  Real_t fy_local[8] ;
  Real_t fz_local[8] ;

  // loop over all elements

 for( Index_t k=0 ; k<numElem ; ++k )

  {

    const Index_t* const elemNodes = elemsToNodesConnectivity[k];

    // get nodal coordinates from global arrays and copy into local arrays.

    for( Index_t lnode=0 ; lnode<8 ; ++lnode )

    {

      Index_t gnode = elemNodes[lnode];
      x_local[lnode] = x[gnode];
      y_local[lnode] = y[gnode];
      z_local[lnode] = z[gnode];

    }

    /* NOTE REMOVAL OF SHAPE DERIVATIVE CALL THAT WAS HERE! */

    CalcElemNodeNormals( B[0] , B[1], B[2], x_local, y_local, z_local );

    SumElemStressesToNodeForces( B, sigxx[k], sigyy[k], sigzz[k],

                                         fx_local, fy_local, fz_local ) ;

    // copy nodal force contributions to global force arrray.

    for( Index_t lnode=0 ; lnode<8 ; ++lnode )

    {

      Index_t gnode = elemNodes[lnode];

      fx[gnode] += fx_local[lnode];

      fy[gnode] += fy_local[lnode];

      fz[gnode] += fz_local[lnode];

    }

  }

}/* --------------------------------------------------- */
/* ----------- Driver below this line ---------------- */
/* --------------------------------------------------- */
 
void InitializeMesh(Index_t edgeElems, Index_t edgeNodes,
                    Index_t **elemsToNodesConnectivity,
                    Real_t *x, Real_t *y, Real_t *z)
{
   Real_t tx, ty, tz ;
   Index_t nidx, zidx ;
 
   /* construct a uniform box for this processor */
 
   /* initialize nodal coordinates */
 
   nidx = 0 ;
   tz  = Real_t(0.) ;
   for (Index_t plane=0; plane<edgeNodes; ++plane) {
      ty = Real_t(0.) ;
      for (Index_t row=0; row<edgeNodes; ++row) {
         tx = Real_t(0.) ;
         for (Index_t col=0; col<edgeNodes; ++col) {
            x[nidx] = tx ;
            y[nidx] = ty ;
            z[nidx] = tz ;
            ++nidx ;
            tx = Real_t(1.125)*Real_t(col+1)/Real_t(edgeElems) ;
         }
         ty = Real_t(1.125)*Real_t(row+1)/Real_t(edgeElems) ;
      }
      tz = Real_t(1.125)*Real_t(plane+1)/Real_t(edgeElems) ;
   }
 
 
   /* embed hexehedral elements in nodal point lattice */
 
   nidx = 0 ;
   zidx = 0 ;
   for (Index_t plane=0; plane<edgeElems; ++plane) {
      for (Index_t row=0; row<edgeElems; ++row) {
         for (Index_t col=0; col<edgeElems; ++col) {
            Index_t *elemToNodes = elemsToNodesConnectivity[zidx] ;
            elemToNodes[0] = nidx                                       ;
            elemToNodes[1] = nidx                                   + 1 ;
            elemToNodes[2] = nidx                       + edgeNodes + 1 ;
            elemToNodes[3] = nidx                       + edgeNodes     ;
            elemToNodes[4] = nidx + edgeNodes*edgeNodes                 ;
            elemToNodes[5] = nidx + edgeNodes*edgeNodes             + 1 ;
            elemToNodes[6] = nidx + edgeNodes*edgeNodes + edgeNodes + 1 ;
            elemToNodes[7] = nidx + edgeNodes*edgeNodes + edgeNodes     ;
            ++zidx ;
            ++nidx ;
         }
         ++nidx ;
      }
      nidx += edgeNodes ;
   }
}
// The only global variables in kernel2 are elemsToNodesConnectivity, x, y, z, 
// fx, fy, fz, and sigxx, sigyy, sigzz.  elemsToNodesConnectivity,x,y,z would be
//  calculated as in kernel1, fx,fy,fz should be initialized to zero, and sigxx,
// sigyy,sigzz could all be initialized to 1.0.
 

int main(int argc, char *argv[])
{
   Index_t edgeElems = 45 ;
   Index_t edgeNodes = edgeElems+1 ;
   Index_t *connectivityMemBlock ;
   Index_t **elemsToNodesConnectivity ;
   Real_t *x, *y, *z ;
   Real_t *fx, *fy, *fz ;
   Real_t *volume ;
   Real_t *sigxx;
   Real_t *sigyy; 
   Real_t *sigzz;

   Index_t numTotalElems = edgeElems*edgeElems*edgeElems ;
   Index_t numTotalNodes = edgeNodes*edgeNodes*edgeNodes ;
 
   /* allocate field memory */
 
   /* each volume element is connected to eight nodes */
   connectivityMemBlock     = new Index_t[8*numTotalElems] ;
   elemsToNodesConnectivity = new Index_t *[numTotalElems] ;
   for (Index_t i=0 ; i<numTotalElems; ++i) {
      elemsToNodesConnectivity[i] = &connectivityMemBlock[8*i] ;
   }
 
   x = new Real_t[numTotalNodes] ;
   y = new Real_t[numTotalNodes] ;
   z = new Real_t[numTotalNodes] ;

   fx = new Real_t[numTotalNodes] ;
   fy = new Real_t[numTotalNodes] ;
   fz = new Real_t[numTotalNodes] ;
 
   for (int i=0; i<numTotalNodes;i++) {
     fx[i] = fy[i] = fz[i] = 0.0;
   }

   volume = new Real_t[numTotalElems] ;
   sigxx = new Real_t[numTotalElems] ;
   sigyy = new Real_t[numTotalElems] ;
   sigzz = new Real_t[numTotalElems] ;

   for (int i=0; i<numTotalElems;i++) {
     sigxx[i] = sigyy[i] = sigzz[i] = 1.0;
   }

   InitializeMesh(edgeElems, edgeNodes, elemsToNodesConnectivity, x, y, z) ;
 
   kernel2(numTotalElems, elemsToNodesConnectivity, x, y, z, fx, fy, fz,
	   sigxx, sigyy, sigzz
	   ) ;
 
   return 0 ;
}

