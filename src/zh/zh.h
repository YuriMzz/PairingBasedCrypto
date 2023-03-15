
#include <relic.h>


#define zh_POS 'p'
#define zh_NEG 'n'
#define zh_WILD 'w'
/**
 * @brief zh master key
 * 
 */
typedef struct zh_mk{
    bn_t a;
    bn_t gamma;
} zh_mk;

#define zh_mk_new(mk)\
    bn_new(mk.a);\
    bn_new(mk.gamma);

#define zh_mk_free(mk)\
    bn_free(mk.a);\
    bn_free(mk.gamma);

/**
 * @brief zh public key
 * 
 */
typedef struct zh_pub{
    g1_t* g;
    g2_t* h;
    size_t k;
    g1_t v;
}zh_pub;

#define zh_pub_new(pub,l)\
    g1_new(pub.v);\
    pub.k=l;\
    pub.g=RLC_ALLOCA(g1_t,(l*6)); \
    pub.h=RLC_ALLOCA(g2_t,(l*3)); \
    for(int i=0;i<((l*6));i++){\
        g1_new(pub.g[i]);\
    }\
    for(int i=0;i<((l*3));i++){\
        g2_new(pub.h[i]);\
    }

#define zh_pub_free(pub)\
    g1_free(pub.v);\
    for(int i=0;i<((pub.k*6));i++){\
        g1_free(pub.g[i]);\
        g2_free(pub.h[i]);\
    }

typedef struct zh_cph{
    g2_t c0;
    g1_t c1;
    char* W; // policy
    size_t k; // number of attributes
}zh_cph;

#define zh_cph_new(cph,n)\
    g2_new(cph.c0);\
    g1_new(cph.c1);\
    cph.k=n;\
    cph.W=RLC_ALLOCA(char,n); 

#define zh_cph_free(cph)\
    g2_free(cph.c0);\
    g1_free(cph.c1);\
    RLC_FREE( cph.W); 

typedef struct zh_prv{
    g1_t* D;
    g1_t  d;
    size_t k;

}zh_prv;

#define zh_prv_new(prv,n)\
    g1_new(prv.d);\
    prv.k=n;\
    prv.D=RLC_ALLOCA(g1_t,(n*2)); \
    for(int i=0;i<(n*2);i++)\
        g1_new(prv.D[i]);

#define zh_prv_free(prv)\
    g1_free(prv.d);\
    for(int i=0;i<(prv.k)*2;i++)\
        g1_free(prv.D[i]);

/**
 * @brief zh setup function
 * 
 * @param mk mater key
 * @param pk public key
 */
void zh_setup(zh_mk* mk, zh_pub* pub, size_t k);

void zh_enc(zh_cph* cph,gt_t msg, zh_pub* pub, char* W);

void zh_keygen(zh_prv* prv, zh_mk* mk, zh_pub* pub,char* S);

void zh_decrypt(gt_t m,zh_cph* cph,zh_prv* prv,zh_pub* pub);

