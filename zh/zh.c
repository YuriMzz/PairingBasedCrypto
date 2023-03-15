#include "zh.h"
void zh_setup(zh_mk* mk, zh_pub* pub, size_t k){
    bn_t n;
    bn_new(n);
    pc_get_ord(n);
    bn_rand_mod(mk->a, n);
    //bn_set_dig(mk->a,1);
    bn_rand_mod(mk->gamma, n);
    //bn_set_dig(mk->gamma,1);
    bn_free(n);

    g1_mul_gen(pub->g[0], mk->a);
    g2_mul_gen(pub->h[0], mk->a);
    for(int i=1;i<((k*6));i++){
        g1_mul(pub->g[i], pub->g[i-1],mk->a);
        if(i<(k*3))
            g2_mul(pub->h[i], pub->h[i-1],mk->a);
    }
    g1_mul_gen(pub->v, mk->gamma);
    
}

void zh_enc(zh_cph* cph,gt_t msg, zh_pub* pk, char* W){
    bn_t n,t;
    bn_new(n);
    bn_new(t);
    pc_get_ord(n);
    bn_rand_mod(t, n);
    int k=pk->k;
    //bn_set_dig(t,1);
    memcpy(cph->W,W,k);
    
    int K=(k*3);
    g1_copy(cph->c1, pk->v);
    for(int i=0;i<k;i++){
        if(W[i]=='p')
            g1_add(cph->c1,cph->c1, pk->g[K-i-1]);
        if(W[i]=='n')
           g1_add(cph->c1,cph->c1, pk->g[K-i-k-1]); 
        if(W[i]=='w')
           g1_add(cph->c1,cph->c1, pk->g[K-i-k-k-1]); 
    }
    g1_mul(cph->c1,cph->c1,t);
    g2_mul_gen(cph->c0, t);


    g1_t app;
    g1_new(app);
    bn_mul_dig(t,t,k);
    g1_mul(app, pk->g[K-1],t);
    pc_map(msg,app,pk->h[0]);
    g1_free(app);
    bn_free(n);
    bn_free(t);

}

void zh_keygen(zh_prv* prv, zh_mk* mk, zh_pub* pk,char* S){
    bn_t n,r,c,s;
    bn_new(n);
    bn_new(r);
    bn_new(c);
    bn_new(s);
    pc_get_ord(n);
    bn_set_dig(s,0);
    int k=pk->k;
    for(int i=0;i<k;i++){
        bn_rand_mod(r, n);
        //bn_set_dig(r,1);

        bn_mxp_dig(c,mk->a, i+1+k+k, n); // (a^(i+2k)) mod n
        bn_add(c,c,r);
        bn_mod(c,c,n);
        g1_mul(prv->D[i+k], pk->v, c); 

        if(S[i]=='p')
            bn_mxp_dig(c,mk->a, i+1, n); // (a^i) mod n
        else
            bn_mxp_dig(c,mk->a, i+1+k, n); // (a^(i+k)) mod n

        bn_add(c,c,r);
        bn_mod(c,c,n);
        g1_mul(prv->D[i], pk->v, c);
        bn_add(s,s,r);
        bn_mod(s,s,n);
    }
    g1_mul(prv->d, pk->v, s);
    bn_free(n);
    bn_free(r);
    bn_free(c);
    bn_free(s);
            
}

int zh_offset(char c){
    if(c=='p')
        return 0;
    if(c=='n')
        return 1;
   return 2;
}
void zh_decrypt(gt_t m,zh_cph* cph,zh_prv* prv,zh_pub* pub){

    g1_t a[2];
    g2_t b[2];
    g1_new(a[0]);
    g1_new(a[1]);
    g2_new(b[0]);
    g2_new(b[1]);
    int i=0,j=0, p=0;
    int k=pub->k;
    LOG_DBG("k=%d\n",k);
    int K=(3*k);
    g2_set_infty(b[0]);
    g1_neg(a[1], prv->d);
    for(int l1=0;l1<k;l1++){
        i=zh_offset(cph->W[l1])*k + l1;
        p=l1;
        if(cph->W[l1]=='w')
            p=l1+k;
        LOG_DBG("i=%d\n",i);
        LOG_DBG("p=%d\n",p);
        g2_add(b[0],b[0],pub->h[i]);
        g1_add(a[1],a[1],prv->D[p]);
        for(int l2=0;l2<k;l2++){
            j=zh_offset(cph->W[l2])*k + l2;
            LOG_DBG("j=%d\n",j);
            if(j!=i)
                {g1_add(a[1],a[1],pub->g[K-j+i]);}
        }
    }

    g1_copy(a[0], cph->c1);
    g2_neg(b[1], cph->c0);

    pc_map_sim(m,a,b,2);
        
}