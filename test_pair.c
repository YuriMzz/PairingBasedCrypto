void testPair(){
    bn_t a, y, n,s, tmp;
    bn_t t[2];
    bn_t q[2];
    g1_t T[2];
    g2_t D[2];
    gt_t c, Ys;
    gt_t e[2];
    //gt_t ee[2];

    LOG_DBG("bn_copy %d digits\n", core_get()->ep_r.used);
    bn_new(a);
    bn_new(y);
    bn_new(n);
    bn_new(s);
    bn_new(tmp);
    LOG_DBG("bn_copy %d digits\n", core_get()->ep_r.used);
    LOG_DBG("digits at %p\n", (void*) &core_get()->ep_r);
    bn_new(q[0]);
    bn_new(q[1]);
    gt_new(c);
    gt_new(Ys);
   
    LOG_DBG("pc_get_ord\n");
    pc_get_ord(n);
    LOG_DBG("bn_rand_mod\n");
   /*  bn_set_dig(y,5);
    bn_set_dig(a,7);
    bn_set_dig(s,9);
    LOG_DBG("bn_add\n");
    bn_add( q[0], a, y);
    LOG_DBG("bn_mod\n");
    bn_mod(q[0], q[0], n);
    LOG_DBG("bn_mul_dig\n");
    bn_mul_dig(q[1], a, 2);
    LOG_DBG("bn_mod\n");
    bn_mod(q[1], q[1], n);
    LOG_DBG("bn_add\n");
    bn_add(q[1], q[1],y);
    LOG_DBG("bn_mod\n");
     bn_mod(q[1], q[1], n); */


    for(int j=0; j<2; j++){
        bn_new(t[j]);
        g1_new(T[j]);
        g2_new(D[j]);
        gt_new(e[j]);
        //gt_new(ee[j]);
        /* LOG_DBG("gt_new ee\n");
        //bn_rand_mod(t[j], n);
        bn_set_dig(t[j], 11);
        LOG_DBG("bn_rand_mod\n");
        g1_mul_gen(T[j], t[j]);
        LOG_DBG("g1_mul_gen\n");
        g1_mul_key(T[j],T[j], s);
        LOG_DBG("g1_mul_key\n");
        bn_mod_inv(tmp, t[j], n);
        LOG_DBG("bn_mod_inv\n");
        bn_mul(tmp, q[j], tmp);
        LOG_DBG("bn_mul\n");
        bn_mod(tmp, tmp, n);
        LOG_DBG("bn_mod\n");
        g2_mul_gen(D[j],tmp);
        LOG_DBG("g2_mul_gen\n");
        pc_map(e[j],T[j], D[j]);
        LOG_DBG("e %d\n",j);
        gt_print(e[j]);
        gt_exp_gen(ee[j], q[j]);
        LOG_DBG("ee %d\n",j);
        gt_print(ee[j]);
        LOG_DBG("\n");
        gt_exp(ee[j], ee[j], s);
        gt_print(ee[j]);
        LOG_DBG("\n");
        if(gt_cmp(e[j], ee[j])==RLC_EQ)
            LOG_DBG("e ok!!!\n");
        else
            LOG_DBG("e nope\n"); */
    }
    if(gt_is_valid(core_get()->gt_g))
        LOG_DBG("gt GEN ok\n");
    else
        LOG_DBG("gt GEN notok\n");
    LOG_DBG("_____________\n");
    //g1_rand(T[1]);  
    //g2_rand(D[1]);
    g1_get_gen(T[0]);
    if(g1_is_valid(T[0]))
        LOG_DBG("g1 ok\n");
    else
        LOG_DBG("g1 notok\n");
    g2_get_gen(D[0]);
    if(g2_is_valid(D[0]))
        LOG_DBG("g2 ok\n");
    else
        LOG_DBG("g2 notok\n");
    //pc_map(Ys,T[1], D[1]);
    if(gt_is_valid(core_get()->gt_g))
        LOG_DBG("gt GEN ok\n");
    else
        LOG_DBG("gt GEN notok\n");
    bn_set_dig(q[0], 2);
    bn_set_dig(q[1], 2);
    bn_set_dig(t[0], 2);
    bn_set_dig(t[1], 2);
    //bn_mod_inv(q[0],q[0],n);
    if(gt_is_valid(core_get()->gt_g))
        LOG_DBG("gt GEN ok\n");
    else
        LOG_DBG("gt GEN notok\n");
    g1_mul_gen(T[0], t[0]);
    g2_mul_gen(D[0], q[0]);
    g1_mul_gen(T[1], t[1]);
    g2_mul_gen(D[1], q[1]);
    if(gt_is_valid(core_get()->gt_g))
        LOG_DBG("gt GEN ok\n");
    else
        LOG_DBG("gt GEN notok\n");
    //g2_mul_gen(D[0], q[1]);
    pc_map(e[0],T[0],D[0]);  
    if(gt_is_valid(core_get()->gt_g))
        LOG_DBG("gt map 1 GEN ok\n");
    else
        LOG_DBG("gt map 1 GEN notok\n");
    pc_map(e[1],T[1],D[1]);  
    if(gt_is_valid(core_get()->gt_g))
        LOG_DBG("gt map 2 GEN ok\n");
    else
        LOG_DBG("gt map 2 GEN notok\n");
    gt_mul(c,e[0],e[1]);
    if(gt_is_valid(core_get()->gt_g))
        LOG_DBG("gt mul GEN ok\n");
    else
        LOG_DBG("gt mul GEN notok\n");
    bn_set_dig(s,8);
    gt_exp_gen(Ys, s);
    if(gt_is_valid(Ys))
        LOG_DBG("gt ok\n");
    else
        LOG_DBG("gt notok\n");
    //gt_exp_gen(Ys,1);
    if(gt_cmp(Ys, c)==RLC_EQ)
        LOG_INFO("MATH ok!!!\n");
    else
        LOG_INFO("MATH nope\n");
    LOG_DBG("_____________\n");
    
    //g1_mul_dig(T[0],T[0], 2);
    //g1_mul_dig(T[1],T[1], -1);
   /*  gt_exp_dig(e[0],e[0],2);
    gt_inv(e[0],e[0]);
    //gt_get_gen(e[1]);
    
     gt_print(e[0]);
    LOG_DBG("\n");
    gt_print(e[1]); 
    LOG_DBG("\n");
    //pc_map_sim(c,T,D,2);
    gt_mul(c,e[0], e[1]);
    gt_print(c);
    LOG_DBG("\n");
    bn_mul(tmp, y, s);
    bn_mod(tmp, tmp, n);
    gt_exp_gen(Ys, tmp);
    gt_print(Ys);
    if(gt_cmp(Ys, c)==RLC_EQ)
        LOG_INFO("ok!!!\n");
    else
        LOG_INFO("nope\n"); */

    for(int j=0; j<2; j++){
        bn_free(t[j]);
        bn_free(q[j]);
        g1_free(T[j]);
        g2_free(D[j]);
        gt_free(e[j]);
        //gt_free(ee[j]);
    }
    gt_free(Ys);
    gt_free(c);
    bn_free(a);
    bn_free(y);
    bn_free(n);
    bn_free(s);
    bn_free(tmp);

}