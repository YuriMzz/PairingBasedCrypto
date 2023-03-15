
void
dec_node_flatten_sim( g1_t* a, g2_t* b,int* counter, bn_t exp,
									abe_policy_t* p, bsw_cph_t* cph, bsw_prv_t* prv );



void
dec_internal_flatten_sim( g1_t* a, g2_t* b, int* counter, bn_t exp,
	abe_policy_t* p,  bsw_cph_t* cph, bsw_prv_t* prv )
{
	int i;
	bn_t e;
	bn_new(e);

	log("dec_internal_flatten_sim\n");

	for( i = 0; i < p->satl_len; i++ )
	{	
		lagrange_coef(e,p->satl, p->satl_len, p->satl[i]);
		bn_mul(e,e,exp);
		dec_node_flatten_sim(
				a,b,counter,
				e, &p->children[p->satl[i] - 1], cph, prv);
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
dec_node_flatten_sim( g1_t* a, g2_t* b,int* counter, bn_t exp,
									abe_policy_t* p, bsw_cph_t* cph, bsw_prv_t* prv )
{
	assert(p->satisfiable);
	print_node(p);
	
	if( p->children_len == 0 ){
	
		g2_copy(b[*counter], cph->C[p->flag]);
		g2_copy(b[(*counter)+1], prv->D[p->flag]);

		g1_mul(a[*counter],prv->Dp[p->flag], exp);
		bn_neg(exp,exp);
		g1_mul(a[(*counter)+1],cph->Cp[p->flag],exp);
		bn_neg(exp,exp);

		(*counter)+=2;
		log("dec_leaf_flatten %d\n",p->flag );
		log("counter: %d\n", *counter);
	}
	else
		dec_internal_flatten_sim(a, b,counter, exp, p, cph, prv);
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
	LOG_DBG("decrypt sim\n");
	int counter=0;
	short num_pair=(cph->policy->min_leaves)*2+1;
	g1_t g1_arr[num_pair];
	g2_t g2_arr[num_pair];
	for(int i=0; i<num_pair;i++){
		g1_new(g1_arr[i]);
		g2_new(g2_arr[i]);
	}
	bn_t one;
	bn_new(one);
	bn_set_dig(one,1);
	log("dec_node_flatten_sim\n");
	dec_node_flatten_sim(g1_arr, g2_arr,&counter, one,  p,cph, prv);
	bn_free(one);
	g1_neg(g1_arr[counter], prv->d);
	g2_copy(g2_arr[counter],cph->c );

	log("pc_map_sim\n");
	pc_map_sim(r,g1_arr, g2_arr,counter+1);

	log("gt_mul\n");
	gt_mul(r, cph->Cs, r); // Cs/e(d,c)
		
	log("dec_flatten\n");
	for(int i=0; i<num_pair;i++){
		g1_free(g1_arr[i]);
		g2_free(g2_arr[i]);
	}
}
