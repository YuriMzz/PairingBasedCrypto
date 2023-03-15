#include <relic.h>


#define PYS_POS 'p'
#define PYS_NEG 'n'
#define PYS_WILD 'w'
/**
 * @brief pys master key
 * 
 */
typedef struct pys_mk{
    bn_t a;
    bn_t b1;
    bn_t b2;
    bn_t* aa;
    int L;
} pys_mk;

#define pys_mk_new(mk,l)\
    bn_new(mk.a);\
    bn_new(mk.b1);\
    bn_new(mk.b2);\
    mk.L=l;\
    mk.aa=RLC_ALLOCA(bn_t,l); \
    for(int i=0;i<l;i++)\
        bn_new(mk.aa[i]);

#define pys_mk_free(mk)\
    bn_free(mk.a);\
    bn_free(mk.b1);\
    bn_free(mk.b2);\
    for(int i=0;i<mk.L;i++)\
        bn_free(aa[i]);\
    RLC_FREE(mk.aa); 
/**
 * @brief pys public key
 * 
 */
typedef struct pys_pub{
    gt_t O1;
    gt_t O2;
    g1_t* A;
    size_t L;
    g1_t ah;
    g2_t ag;
    g1_t v0;
    g1_t v1;
}pys_pub;

#define pys_pub_new(pub,l)\
    gt_new(pub.O1);\
    gt_new(pub.O1);\
    g1_new(pub.ah);\
    g2_new(pub.ag);\
    g1_new(pub.v0);\
    g1_new(pub.v1);\
    pub.L=l;\
    pub.A=RLC_ALLOCA(g1_t,l); \
    for(int i;i<l;i++)\
        g1_new(pub.A[i]);

#define pys_pub_free(pub)\
    gt_free(pub.O1);\
    gt_free(pub.O1);\
    g1_free(pub.ah);\
    g2_free(pub.ag);\
    g1_free(pub.v0);\
    g1_free(pub.v1);\
    for(int i;i<pub.L;i++)\
        g1_free(pub.A[i]);\
    RLC_FREE(pub.A); 

typedef struct pys_cph{
    gt_t c0;
    g2_t c1;
    g2_t c2;
    g1_t c3;
    g1_t c4;
    short int* J;
    size_t n1;
}pys_cph;

#define pys_cph_new(cph,n)\
    gt_new(cph.c0);\
    g2_new(cph.c1);\
    g2_new(cph.c2);\
    g1_new(cph.c3);\
    g1_new(cph.c4);\
    cph.n1=n;\
    cph.J=RLC_ALLOCA(short int,n); 

#define pys_cph_free(cph)\
    gt_free(cph.c0);\
    g2_free(cph.c1);\
    g2_free(cph.c2);\
    g1_free(cph.c3);\
    g1_free(cph.c4);\
    RLC_FREE( cph.J); 

typedef struct pys_prv{
    g2_t l1;
    g2_t l2;
    g1_t* k1;
    g1_t* k1p;
    g1_t* k2;
    g1_t* k2p;
    bn_t tv, tz;
    size_t N1;

}pys_prv;

#define pys_prv_new(prv,n)\
    g2_new(prv.l1);\
    g2_new(prv.l2);\
    prv.N1=n;\
    prv.k1=RLC_ALLOCA(g1_t,n); \
    prv.k1p=RLC_ALLOCA(g1_t,n); \
    prv.k2=RLC_ALLOCA(g1_t,n); \
    prv.k2p=RLC_ALLOCA(g1_t,n); \
    for(int i;i<n;i++){\
        g1_new(prv.k1[i]);\
        g1_new(prv.k1p[i]);\
        g1_new(prv.k2[i]);\
        g1_new(prv.k2p[i]);\
    }\
    bn_new(prv.tv);\
    bn_new(prv.tz);\
    prv.N1=n;

#define pys_prv_free(prv)\
    g2_free(prv.l1);\
    g2_free(prv.l2);\
    for(int i;i<prv.N1;i++){\
        g1_free(prv.k1[i]);\
        g1_free(prv.k1p[i]);\
        g1_free(prv.k2[i]);\
        g1_free(prv.k2p[i]);\
    }\
    RLC_FREE(prv.k1); \
    RLC_FREE(prv.k1p); \
    RLC_FREE(prv.k2); \
    RLC_FREE(prv.k2p); \
    bn_free(prv.tv);\
    bn_free(prv.tz);

typedef char* pys_as;

/**
 * @brief pys setup function
 * 
 * @param mk mater key
 * @param pk public key
 */
void pys_setup(pys_mk* mk, pys_pub* pk);

void pys_enc(pys_cph* cph,gt_t msg, pys_pub* pk, char* W);

void pys_keygen(pys_prv* prv, pys_mk* mk, pys_pub* pub,char* S, size_t N1);

void pys_decrypt(gt_t m,pys_cph* cph,pys_prv* prv,pys_pub* pub);

