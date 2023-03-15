
void
dec_node_flatten_sim( g1_t* a, g2_t* b,int* counter, bn_t exp,
									gpsw_policy_t* p, g2_t* D, g1_t* E );



void
dec_internal_flatten_sim( g1_t* a, g2_t* b, int* counter,bn_t exp,
	gpsw_policy_t* p, g2_t* D, g1_t* E  )
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
				e, &p->children[p->satl[i] - 1], D, E);
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
									gpsw_policy_t* p, g2_t* D, g1_t* E )
{
	assert(p->satisfiable);
	print_node(p);
	if( p->children_len == 0 ){
		g2_copy(b[*counter],D[p->flag]);
		/* if(exp<0){
			g1_neg(a[*counter],E[p->flag]);
			g1_mul_dig(a[*counter],a[*counter], -exp);
		}else
			g1_mul_dig(a[*counter],E[p->flag], exp); */
		g1_mul(a[*counter],E[p->flag], exp);
		(*counter)++;
		log("dec_leaf_flatten %d\n",p->flag );
		log("counter: %d\n", *counter);
		//log("exp: %d\n", exp);
	}
	else
		dec_internal_flatten_sim(a, b,counter, exp, p, D, E);
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
dec_flatten( gt_t r, gpsw_policy_t* p, g2_t* D, g1_t* E )
{
	bn_t one;
	int counter=0;
	bn_new(one);

	bn_set_dig(one, 1);
	gt_set_unity(r);
	g1_t g1_arr[p->min_leaves];
	g2_t g2_arr[p->min_leaves];
	for(int i=0; i<p->min_leaves;i++){
		g1_new(g1_arr[i]);
		g2_new(g2_arr[i]);
	}
	
	log("dec_node_flatten\n");
	dec_node_flatten_sim(g1_arr, g2_arr,&counter, one,  p,D, E);
	pc_map_sim(r,g1_arr, g2_arr,counter);
	log("dec_flatten\n");
	bn_free(one);
	for(int i=0; i<p->min_leaves;i++){
		g1_free(g1_arr[i]);
		g2_free(g2_arr[i]);
	}
}
