#include "pys_utils.h"
#ifdef LOG_LEVEL
#undef LOG_LEVEL
#endif
#define LOG_LEVEL LOG_LEVEL_INFO
// https://stackoverflow.com/questions/12991758/creating-all-possible-k-combinations-of-n-items-in-c
void sum_prod(bn_t s, bn_t p,short int* J, int n,  int k){
    if (k == 0){
        bn_add(s,s,p);
        return;
    } // TODO CHECK CORRECTNESS
    bn_t app;
    bn_new(app);
    for (int i = 0; i <= n - k; ++i) {
        bn_mul_dig(app,p,J[i]);
        sum_prod(s,app,&J[i+1],n-i-1,k-1);
        //bn_print(s);
    }
    bn_free(app);

}
void lambda(bn_t r,short int* J, int n, int i){
    int k= n-i;
    bn_t p;
    bn_set_dig(r,0);
    bn_new(p);
    bn_set_dig(p,1);
    if(k%2)
        bn_neg(p,p);
    sum_prod(r,p,J,n,k);
    pc_get_ord(p);
    //bn_mod(r,r,p);

    bn_print(r);
    bn_free(p);
}

void t_lambda(bn_t r,short int* J, int J_len){
    bn_t p;
    bn_new(p);
    bn_set_dig(r,0);
    for(int k=0; k<(J_len+1);k++){
        LOG_INFO("K=%d\n",k);
        bn_set_dig(p,1);
        if(k%2)
            bn_neg(p,p);
        sum_prod(r,p,J,J_len,k);
        bn_print(r);
        pc_get_ord(p);
        //bn_mod(r,r,p);
    }
    bn_free(p);
}

