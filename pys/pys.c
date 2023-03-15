#include "pys_utils.h"
#include "pys.h"
#include <relic.h>
#include <math.h>

#ifdef LOG_LEVEL
#undef LOG_LEVEL
#endif
#define LOG_LEVEL LOG_LEVEL_INFO
void pys_setup(pys_mk* mk, pys_pub* pk){
    
    bn_t n;
    bn_new(n);
    pc_get_ord(n);

    g1_t tmp;
    g2_new(tmp);

    g2_new(pk->ag);
    g2_get_gen(pk->ag);

    bn_rand_mod(mk->b1, n);
    bn_rand_mod(mk->b2, n);

    for(int i=0; i<pk->L; i++){
        bn_rand_mod(mk->aa[i], n);
        g1_mul_gen(pk->A[i], mk->aa[i]);
    }
    bn_rand_mod(mk->a,n);

    g1_rand(pk->v0);
    g1_rand(pk->v1);
    g1_add(pk->ah,pk->v0,pk->v1 );  // v0+v1

    bn_mul(n, mk->a, mk->b1);   // a*b1
    g1_mul(tmp, pk->ah, n);     // (v0+v1)*a*b1
    pc_map(pk->O1,tmp, pk->ag);      // e(tmp,g)=(e(v0,g)*e(v1,g))^(a*b1)

    bn_mul(n, mk->a, mk->b2);   // a*b2
    g1_mul(tmp, pk->ah, n);     // (v0+v1)*a*b2
    pc_map(pk->O2,tmp, pk->ag);      // e(tmp,g)=(e(v0,g)*e(v1,g))^(a*b2)

    g1_mul_gen(pk->ah, mk->a);
    g2_mul_gen(pk->ag,mk->a );

    bn_free(n);
    g1_free(tmp);
}

void pys_enc(pys_cph* cph,gt_t msg, pys_pub* pk, char* W){

    gt_t O;
    gt_new(O);

    bn_t r1, r2, tw,n;
    bn_new(r1);
    bn_new(r2);
    bn_new(tw);
    bn_new(n);

    pc_get_ord(n);
    bn_rand_mod(r1,n);
    bn_rand_mod(r2,n);

    LOG_INFO("computing c0\n");
    gt_exp(O,pk->O1, r1);
    gt_exp(cph->c0, pk->O2,r2);
    gt_mul(cph->c0, cph->c0, O);
    gt_mul(cph->c0, cph->c0, msg);

    gt_free(O);

    int n1,n2,n3;
    n1=n2=n3=0;
    for(int i=0; i<pk->L; i++){
        if(W[i]==PYS_WILD)
            n1++;
        if(W[i]==PYS_POS)
            n2++;
        if(W[i]==PYS_NEG)
            n3++;
    }
    cph->n1=n1;
    short int* J =cph->J;
    short int V [n2] ;
    short int Z [n3] ;
    
    LOG_INFO("n1:%d n2:%d n3:%d\n", n1, n2, n3);
    for(int i=0,i1=0,i2=0,i3=0; i<pk->L; i++){
        if(W[i]==PYS_WILD)
            J[i1++]=i;
        if(W[i]==PYS_POS)
            V[i2++]=i;
        if(W[i]==PYS_NEG)
            Z[i3++]=i;
    }

    LOG_INFO("t_lambda\n");
    t_lambda(tw,J, n1);
    LOG_INFO("bn_mod_inv\n");
    bn_mod_inv(tw,tw,n);

    LOG_INFO("computing c1\n");
    bn_mul(n,r1,tw);
    g2_mul(cph->c1,pk->ag, n);

    LOG_INFO("computing c2\n");
    bn_mul(n,r2,tw);
    g2_mul_gen(cph->c2, n);

    bn_add(r1,r1,r2);
    bn_mul(r1,r1,tw);

    LOG_INFO("filling m2\n");
    bn_t m2 [n2] ;
    g1_t A2 [n2] ;
    for(int i=0; i<n2; i++){
        bn_new(m2[i]);
        bn_set_dig(m2[i],1);
        for(int j=0; j<n1;j++){
            bn_mul_dig(m2[i], m2[i], abs(V[i]-J[j]));
            if(V[i]<J[j])
                bn_neg(m2[i],m2[i]);
        }
        g1_new(A2[i]);
        g1_copy(A2[i],pk->A[V[i]] );
    }
    
    LOG_INFO("computing c3\n");
    g1_mul_sim_lot(cph->c3, A2, m2,n2);
    g1_add(cph->c3, cph->c3, pk->v0);
    g1_mul(cph->c3, cph->c3,r1);

    for(int i=0; i<n2; i++)
        bn_free(m2[i]);


    LOG_INFO("filling m3\n");
    bn_t m3 [n3];
    g1_t A3 [n3] ;
    for(int i=0; i<n3; i++){
        bn_new(m3[i]);
        bn_set_dig(m3[i],1);
        
        for(int j=0; j<n1;j++){
           bn_mul_dig(m3[i], m3[i], abs(Z[i]-J[j]));
            if(Z[i]<J[j])
                bn_neg(m3[i],m3[i]);
        }
        LOG_INFO("filling m3 %d\n", Z[i]);
        g1_new(A3[i]);
        g1_copy(A3[i],pk->A[Z[i]] );
        LOG_INFO("filling m3 %d\n", i);
    }

    LOG_INFO("computing c4\n");
    g1_mul_sim_lot(cph->c4, A3, m3,n3);
    g1_add(cph->c4, cph->c4, pk->v1);
    g1_mul(cph->c4, cph->c4,r1);
        
    for(int i=0; i<n3; i++)
        bn_free(m3[i]);

    bn_free(r1);
    bn_free(r2);
    bn_free(tw);
    bn_free(n);
    
}

void pys_keygen(pys_prv* prv, pys_mk* mk, pys_pub* pub, char* S, size_t N1){

    
    bn_t s, n, t;
    bn_new(s);
    bn_new(n);
    bn_new(t);
    pc_get_ord(n);
    bn_rand_mod(s,n);

    // compute Vp e Zp from S
    int n2,n3;
    n2=n3=0;
    for(int i=0; i<pub->L; i++){
        if(S[i]==PYS_POS)
            n2++;
    }
    n3=pub->L-n2;

    LOG_INFO("n2:%d n3:%d\n", n2, n3);
    short int V [n2] ;
    short int Z [n3] ;
    
    for(int i=0,i2=0,i3=0; i<pub->L; i++){
        if(S[i]==PYS_POS)
            V[i2++]=i;
        else
            Z[i3++]=i;
    }
    
    // compute tv and L1
    LOG_INFO("compute tv\n");
    t_lambda(t,V, n2);
    bn_copy(prv->tv,t);
    LOG_INFO("compute L1\n");
    bn_mod_inv(t,t,n);
    bn_mul(t,t,s);
    bn_mul(t,t,mk->a);
    //bn_mod(t,t,n);
    g2_mul_gen(prv->l1, t);

    // compute tz and L2
    LOG_INFO("compute tz\n");
    t_lambda(t,Z, n3);
    bn_copy(prv->tz,t);
    LOG_INFO("compute L2\n");
    bn_mod_inv(t,t,n);
    bn_mul(t,t,s);
    bn_mul(t,t,mk->a);
    //bn_mod(t,t,n);
    g2_mul_gen(prv->l2, t);


    // compute Ks
    g1_t hs;
    g1_new(hs);
    bn_t s2, app;
    bn_new(s2);
    bn_new(app);
    g1_mul_gen(hs,s);
    bn_add(s2,s,mk->b2); // s2
    bn_add(s,s,mk->b1); // s1

    LOG_INFO("compute Ks\n");
    for(int k=0; k<N1; k++){
        LOG_INFO("compute K1s %d\n", k);
        for(int j=0; j<n2;j++){
            int i=V[j];
            bn_mul_dig(app,mk->aa[i],pow(i,k));
            bn_add(t,t,app);
        }
        bn_mod(t,t,n);
        g1_mul_sim(prv->k1[k],hs,t,pub->v0,s );
        bn_mul(t,t,mk->a);
        bn_mod(t,t,n);
        LOG_INFO("compute K1ps %d\n", k);
        g1_mul_sim(prv->k1p[k],hs,t,pub->v0,s2 );

        LOG_INFO("compute K2s %d\n", k);
        for(int j=0; j<n3;j++){
            int i=Z[j];
            bn_mul_dig(app,mk->aa[i],pow(i,k));
            bn_add(t,t,app);
        }
        bn_mod(t,t,n);
        g1_mul_sim(prv->k2[k],hs,t,pub->v1,s );
        bn_mul(t,t,mk->a);
        bn_mod(t,t,n);
        LOG_INFO("compute K2ps %d\n", k);
        g1_mul_sim(prv->k2p[k],hs,t,pub->v1,s2 );
    }

    prv->N1=N1;

    bn_free(s);
    bn_free(app);
    bn_free(s2);
    bn_free(t);
    bn_free(n);
    g1_free(hs);
}

void pys_decrypt(gt_t m,pys_cph* cph,pys_prv* prv,pys_pub* pub){
    
    bn_t Lam [cph->n1+1];
    for(int i=0; i<=cph->n1; i++){
        bn_new(Lam[i]);
        lambda(Lam[i],cph->J,cph->n1,i );

        //bn_neg(Lam[i],Lam[i]); // avoid g1_neg later
    }
    g1_t a [6];
    g2_t b [6];
    for(int i=0;i<6;i++){
        g1_new(a[i]);
        g2_new(b[i]);
    }

    g1_mul(a[0],cph->c3, prv->tv);
    g1_mul(a[1],cph->c4, prv->tz);
    g1_mul_sim_lot(a[2],prv->k1,Lam,cph->n1+1);
    g1_mul_sim_lot(a[3],prv->k1p,Lam,cph->n1+1);
    g1_mul_sim_lot(a[4],prv->k2,Lam,cph->n1+1);
    g1_mul_sim_lot(a[5],prv->k2p,Lam,cph->n1+1);

    g1_neg(a[2], a[2]);
   g1_neg(a[3], a[3]);
   g1_neg(a[4], a[4]);
   g1_neg(a[5], a[5]);

    for(int i=0; i<=cph->n1; i++)
        bn_free(Lam[i]);

    g2_copy(b[0],prv->l1);
    g2_copy(b[1],prv->l2);
    g2_copy(b[2],cph->c1);
    g2_copy(b[3],cph->c2);
    g2_copy(b[4],cph->c1);
    g2_copy(b[5],cph->c2);

    pc_map_sim(m,a,b,6);

    gt_mul(m,m,cph->c0);

    for(int i=0;i<6;i++){
        g1_free(a[i]);
        g2_free(b[i]);
    }
    for(int i=0; i<=cph->n1; i++)
        bn_free(Lam[i]);
}