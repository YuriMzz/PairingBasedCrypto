
/*!
 * DecryptNode(E;D;x) algorithm for leaf node
 *
 * @param r				Pairing result
 * @param exp			Recursive exponent from DecryptNode(E;D;z) algorithm from non-leaf node above
 * @param p				Policy node dtat structure(leaf node x)
 * @param cph			Ciphertext data structure
 * @param pub			Public key data structure
 * @return				None
 */

void
dec_leaf_flatten( gt_t r, bn_t exp,g1_t Cyp,g2_t Cy, g1_t Dyp, g2_t Dy    )
{
	gt_t s;
	g1_t a[2];
	g2_t b[2];
	g1_new(a[0]);
	g1_new(a[1]);
	g2_new(b[0]);
	g2_new(b[1]);
	gt_new(s);
	
	/* g2_get_gen(b[1]);
	if(g2_cmp(Dy, b[1])==RLC_EQ)
        LOG_DBG("Dy is G2_GEN\n");
	else
		LOG_DBG("Dy NOT OK\n");
	g2_mul_dig(b[0],b[1],3);
	if(g2_cmp(Cy, b[0])==RLC_EQ)
        LOG_DBG("Cy coef 3\n");
	g2_mul_dig(b[0],b[1],7);
	if(g2_cmp(Cy, b[0])==RLC_EQ)
        LOG_DBG("Cy coef 7\n");
	g2_mul_dig(b[0],b[1],13);
	if(g2_cmp(Cy, b[0])==RLC_EQ)
        LOG_DBG("Cy coef 13\n"); */
	
	if(g2_is_valid(Cy))
		LOG_DBG("Cy is OK\n");
	else
		LOG_DBG("Cy is TRASH\n");
	if(g2_is_valid(Dy))
		LOG_DBG("Dy is OK\n");
	else
		LOG_DBG("Dy is TRASH\n");


	g2_copy(b[1], Cy);
	g2_copy(b[0], Dy);

	if(g1_is_valid(Dyp))
		LOG_DBG("Dyp is OK\n");
	else
		LOG_DBG("Dyp is TRASH\n");
	/* g1_get_gen(a[1]);
	g1_map(a[0],(uint8_t*)"A",1);
	g1_add(a[0],a[0],a[1]);
	if(g1_cmp(Dyp, a[0])==RLC_EQ)
        LOG_DBG("Dyp of attribute A\n");
	else
		LOG_DBG("Dyp NOT of attribute A\n"); */

	/* g1_map(a[0],(uint8_t*)"2",1);
	g1_add(a[0],a[0],a[1]);
	if(g1_cmp(Dyp, a[0])==RLC_EQ)
        LOG_DBG("Dyp of attribute 2\n");
	g1_map(a[0],(uint8_t*)"1",1);
	g1_add(a[0],a[0],a[1]);
	if(g1_cmp(Dyp, a[0])==RLC_EQ)
        LOG_DBG("Dyp of attribute 1\n");
	g1_map(a[0],(uint8_t*)"3",1);
	g1_add(a[0],a[0],a[1]);
	if(g1_cmp(Dyp, a[0])==RLC_EQ)
        LOG_DBG("Dyp of attribute 3\n");
	g1_map(a[0],(uint8_t*)"2",1);
	g1_mul_dig(a[0],a[0],3);
	if(g1_cmp(Cyp, a[0])==RLC_EQ)
        LOG_DBG("Cyp of attribute 2 coef 3\n");
	g1_map(a[0],(uint8_t*)"1",1);
	g1_mul_dig(a[0],a[0],7);
	if(g1_cmp(Cyp, a[0])==RLC_EQ)
        LOG_DBG("Cyp of attribute 1 coef 7\n");
	g1_map(a[0],(uint8_t*)"3",1);
	g1_mul_dig(a[0],a[0],13);
	if(g1_cmp(Cyp, a[0])==RLC_EQ)
        LOG_DBG("Cyp of attribute 3 coef 13\n"); */
	// multiply g1 instead of exponentiate gt
	//LOG_DBG("exp= %d \n",exp );

	/* bn_t ee;
	bn_new(ee);
	bn_set_dig(ee,abs(exp));
	if(exp<0)
		bn_neg(ee,ee); */
	
	/* int j=0;
	g1_copy(a[1],Dyp);
	for(j=1;j<abs(exp);j++)
		g1_add(a[1],a[1],Dyp);
	g1_norm(a[1],a[1]); */
	g1_mul(a[1],Dyp,exp);
	if(g1_is_valid(a[1]))
		LOG_DBG("a1 pre inv is OK\n");
	else
		LOG_WARN("a1 pre inv is TRASH\n");
	/* if(exp<0)
		g1_neg(a[1],a[1]);	
	if(g1_is_valid(a[1]))
		LOG_DBG("a1 is OK\n");
	else
		LOG_DBG("a1 is TRASH\n"); */

	g1_mul(a[0],Cyp,exp);
	//if(exp>0)
	g1_neg(a[0],a[0]);
	if(g1_is_valid(a[0]))
		LOG_DBG("a0 is OK\n");
	else
		LOG_WARN("a0 is TRASH\n");
	//bn_free(ee); 

	/* g1_mul_dig(a[0],Cyp,abs(exp));
	g1_mul_dig(a[1],Dyp,abs(exp));
	if(exp>0){ // you can't mul for e<0
		//g1_mul_dig(a[1],Cyp,exp);
		g1_neg(a[0],a[0]);
		log("a0 = Cyp^-exp");
		//g1_mul_dig(a[0],Dyp,exp);
	}
	else{ 
		//g1_mul_dig(a[0],Dyp,-exp);
		g1_neg(a[1],a[1]);
		//g1_mul_dig(a[1],Cyp,-exp);
	} */

	
	log("pc_map_sim\n");
	pc_map_sim(s,  a, b,2); 
	if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("pc_map GEN ok\n");
        else
            LOG_WARN("pc_map GEN notok\n");   
	if(gt_is_unity(s))
		log("UNO\n");

	/* gt_t comp;
	gt_new(comp);
	gt_t app;
	gt_new(app);

	
	pc_map(s,Dyp,b[1]);
	gt_print(s);
	if(exp>0)
		gt_exp_dig(s,s,exp);
	else{
		gt_exp_dig(s,s,abs(exp));
		gt_inv(s,s);
	} 

	if(!gt_is_valid(s))
		LOG_DBG("pair 1 not valid\n");
	pc_map(comp,a[1],b[1]);
	LOG_DBG("-\n");
	gt_print(s);
	if(gt_cmp(comp, s)==RLC_EQ)
        LOG_DBG("MAPS 1 OK\n");
	else
		LOG_DBG("MAPS 1 NOOO\n");

	
	pc_map(app,Cyp,b[0]);
	gt_print(app);
	if(exp>0){
		gt_inv(app,app);
		gt_exp_dig(app,app,exp);
	}
	else{
		gt_exp_dig(app,app,abs(exp));
	} 
	if(!gt_is_valid(app))
		LOG_DBG("pair 2 not valid\n");
	pc_map(comp,a[0],b[0]);
	LOG_DBG("-\n");
	gt_print(comp);
	if(gt_cmp(comp, app)==RLC_EQ)
        LOG_DBG("MAPS 2 OK\n");
	else
		LOG_DBG("MAPS 2 NOOO\n");

	gt_mul(s, s, app);

	gt_free(app);  */
	//pc_map_sim(s,  a, b,2); 
	/* g1_copy(a[0],Dyp);
	g1_neg(a[1],Cyp);
	pc_map_sim(s,  a, b,2);
	if(exp>0)
		gt_exp_dig(s,s,exp);
	else{
		gt_exp_dig(s,s,-exp);
		gt_inv(s,s);
	}  */
	
	/* if(gt_is_valid(s)){
	gt_exp_dig(comp,core_get()->gt_g, 9);
	if(gt_cmp(comp, s)==RLC_EQ)
        LOG_DBG("gen exp 9\n");
   	gt_exp_dig(comp,core_get()->gt_g, 21);
	gt_inv(comp,comp);
	if(gt_cmp(comp, s)==RLC_EQ)
            LOG_DBG("gen exp -21\n"); 
	gt_exp_dig(comp,core_get()->gt_g, 13);
	if(gt_cmp(comp, s)==RLC_EQ)
            LOG_DBG("gen exp 13\n"); 
	gt_free(comp);
	}else
		 LOG_DBG("s NOT VALID\n");
 */
	

	log("gt_mul\n");
	gt_mul(r, r, s);
	//gt_print(r);
	log("gt_free\n");

	
	gt_free(s);
	g1_free(a[0]);
	g1_free(a[1]);
	g2_free(b[0]);
	g2_free(b[1]);
}



void
dec_node_flatten( gt_t r, bn_t exp,
									abe_policy_t* p, bsw_cph_t* cph, bsw_prv_t* prv );
/*!
 * DecryptNode(E;D;z) algorithm for non-leaf node
 *
 * @param r				Pairing result
 * @param exp			Recursive exponent from DecryptNode(E;D;z) algorithm from non-leaf node above
 * @param p				Policy node dtat structure(non-leaf node z)
 * @param cph			Ciphertext data structure
 * @param pub			Public key data structure
 * @return				None
 */

void
dec_internal_flatten( gt_t r,bn_t exp, abe_policy_t* p, bsw_cph_t* cph, bsw_prv_t* prv )
{
	int i;
	bn_t e;
	bn_new(e);
	log("dec_internal_flatten\n");

	for( i = 0; i < p->satl_len; i++ )
	{	
		LOG_DBG_("lagrange_coef i= %d\n", p->satl[i]);
		//bn_copy(e,exp);
		lagrange_coef(e,p->satl, p->satl_len, p->satl[i]);
		bn_mul(e,e,exp);
		dec_node_flatten(r, e, &p->children[p->satl[i] - 1], cph,prv);
	}
	bn_free(e);

}

/*!
 * Choose DecryptNode algorithm for non-leaf node and leaf node
 *
 * @param r				Pairing result
 * @param exp			Recursive exponent from DecryptNode(E;D;z) algorithm from non-leaf node above
 * @param p				Policy node data structure
 * @param cph			Ciphertext data structure
 * @param pub			Public key data structure
 * @return				None
 */

void
dec_node_flatten( gt_t r, bn_t exp,
									abe_policy_t* p, bsw_cph_t* cph, bsw_prv_t* prv )
{
	assert(p->satisfiable);
	print_node(p);
	if( p->children_len == 0 ){
		dec_leaf_flatten(r, exp, cph->Cp[p->flag], cph->C[p->flag], prv->Dp[p->flag], prv->D[p->flag]);
		LOG_DBG("dec_leaf_flatten %d\n",p->flag );
	}
	else
		dec_internal_flatten(r, exp, p,cph, prv);
	if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("dec_node_flatten GEN ok\n");
        else
            LOG_WARN("dec_node_flatten GEN notok\n");   
}

/*!
 * DecryptNode algorithm for root secret
 *
 * @param r				Root secret
 * @param p				Policy node dtat structure(root)
 * @param cph			Ciphertext data structure
 * @param pub			Public key data structure
 * @return				None
 */

void
dec_flatten( gt_t r, abe_policy_t* p,bsw_cph_t* cph, bsw_prv_t* prv )
{
	LOG_DBG("decrypt seq\n");
	g1_neg(prv->d, prv->d);
	pc_map(r,prv->d, cph->c); // 1/e(d,c)
	g1_neg(prv->d, prv->d);

	log("gt_mul\n");
	gt_mul(r, cph->Cs, r); // Cs/e(d,c)

	log("dec_node_flatten\n");
	bn_t one;
	bn_new(one);
	bn_set_dig(one,1);
	dec_node_flatten(r, one, p,cph,prv);
	bn_free(one);
}
