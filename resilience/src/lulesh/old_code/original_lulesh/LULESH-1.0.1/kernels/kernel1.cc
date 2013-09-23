// This code fragment is from CalcKinematicsForElems (and child routines):
 
typedef float        real4 ;
typedef double       real8 ;
typedef long double  real10 ;  /* 10 bytes on x86 */
 
typedef int    Index_t ; /* array subscript and loop index */
typedef real8  Real_t ;  /* floating point representation */
typedef int    Int_t ;   /* integer representation */
 
static inline
Real_t CalcElemVolume( const Real_t x0, const Real_t x1,
               const Real_t x2, const Real_t x3,
               const Real_t x4, const Real_t x5,
               const Real_t x6, const Real_t x7,
               const Real_t y0, const Real_t y1,
               const Real_t y2, const Real_t y3,
               const Real_t y4, const Real_t y5,
               const Real_t y6, const Real_t y7,
               const Real_t z0, const Real_t z1,
               const Real_t z2, const Real_t z3,
               const Real_t z4, const Real_t z5,
               const Real_t z6, const Real_t z7 )
{
  Real_t twelveth = Real_t(1.0)/Real_t(12.0);
 
  Real_t dx61 = x6 - x1;
  Real_t dy61 = y6 - y1;
  Real_t dz61 = z6 - z1;
 
  Real_t dx70 = x7 - x0;
  Real_t dy70 = y7 - y0;
  Real_t dz70 = z7 - z0;
 
  Real_t dx63 = x6 - x3;
  Real_t dy63 = y6 - y3;
  Real_t dz63 = z6 - z3;
 
  Real_t dx20 = x2 - x0;
  Real_t dy20 = y2 - y0;
  Real_t dz20 = z2 - z0;
 
  Real_t dx50 = x5 - x0;
  Real_t dy50 = y5 - y0;
  Real_t dz50 = z5 - z0;
 
  Real_t dx64 = x6 - x4;
  Real_t dy64 = y6 - y4;
  Real_t dz64 = z6 - z4;
 
  Real_t dx31 = x3 - x1;
  Real_t dy31 = y3 - y1;
  Real_t dz31 = z3 - z1;
 
  Real_t dx72 = x7 - x2;
  Real_t dy72 = y7 - y2;
  Real_t dz72 = z7 - z2;
 
  Real_t dx43 = x4 - x3;
  Real_t dy43 = y4 - y3;
  Real_t dz43 = z4 - z3;
 
  Real_t dx57 = x5 - x7;
  Real_t dy57 = y5 - y7;
  Real_t dz57 = z5 - z7;
 
  Real_t dx14 = x1 - x4;
  Real_t dy14 = y1 - y4;
  Real_t dz14 = z1 - z4;
 
  Real_t dx25 = x2 - x5;
  Real_t dy25 = y2 - y5;
  Real_t dz25 = z2 - z5;
 
#define TRIPLE_PRODUCT(x1, y1, z1, x2, y2, z2, x3, y3, z3) \
   ((x1)*((y2)*(z3) - (z2)*(y3)) + (x2)*((z1)*(y3) - (y1)*(z3)) + (x3)*((y1)*(z2) - (z1)*(y2)))
 
  Real_t volume =
    TRIPLE_PRODUCT(dx31 + dx72, dx63, dx20,
       dy31 + dy72, dy63, dy20,
       dz31 + dz72, dz63, dz20) +
    TRIPLE_PRODUCT(dx43 + dx57, dx64, dx70,
       dy43 + dy57, dy64, dy70,
       dz43 + dz57, dz64, dz70) +
    TRIPLE_PRODUCT(dx14 + dx25, dx61, dx50,
       dy14 + dy25, dy61, dy50,
       dz14 + dz25, dz61, dz50);
 
#undef TRIPLE_PRODUCT
 
  volume *= twelveth;
 
  return volume ;
}
 
static inline
Real_t CalcElemVolume( const Real_t x[8], const Real_t y[8], const Real_t z[8] )
{
return CalcElemVolume( x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7],
                       y[0], y[1], y[2], y[3], y[4], y[5], y[6], y[7],
                       z[0], z[1], z[2], z[3], z[4], z[5], z[6], z[7]);
}
 
 
void kernel1(Index_t numTotalElems,
             const Index_t * const *elemsToNodesConnectivity,
             const Real_t *x, const Real_t *y, const Real_t *z,
             Real_t *volume)
{
  Real_t x_local[8] ;
  Real_t y_local[8] ;
  Real_t z_local[8] ;
 
  for( Index_t k=0 ; k<numTotalElems ; ++k )
  {
 
    const Index_t* const elemToNodes = elemsToNodesConnectivity[k] ;
 
    // get nodal coordinates from global arrays and copy into local arrays.
 
    for( Index_t lnode=0 ; lnode<8 ; ++lnode )
    {
      Index_t gnode = elemToNodes[lnode];
      x_local[lnode] = x[gnode];
      y_local[lnode] = y[gnode];
      z_local[lnode] = z[gnode];
    }
 
    volume[k] = CalcElemVolume(x_local, y_local, z_local );
  }
}
 
/* --------------------------------------------------- */
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
 
int main(int argc, char *argv[])
{
   Index_t edgeElems = 45 ;
   Index_t edgeNodes = edgeElems+1 ;
   Index_t *connectivityMemBlock ;
   Index_t **elemsToNodesConnectivity ;
   Real_t *x, *y, *z ;
   Real_t *volume ;
 
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
 
   volume = new Real_t[numTotalElems] ;
 
   InitializeMesh(edgeElems, edgeNodes, elemsToNodesConnectivity, x, y, z) ;
 
   kernel1(numTotalElems, elemsToNodesConnectivity, x, y, z, volume) ;
 
   return 0 ;
}
