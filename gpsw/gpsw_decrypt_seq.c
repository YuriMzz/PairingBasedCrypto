
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
dec_leaf_flatten( gt_t r, bn_t exp,g2_t D,g1_t E  )
{
	gt_t s;
	g1_t a;
	g1_new(a);
	gt_new(s);
	g1_print(E);
	log("\n");
	g2_print(D);
	g1_mul(a,E,exp);
	/* if(exp<0){ // you can't mul for e<0
		g1_neg(a,E);
		log("g1_neg\n");
		g1_mul_dig(a,a,-exp);
	}
	else
		g1_mul_dig(a,E,exp); */
	log("pc_map\n");
	pc_map(s,  a, D); /* num_pairings++; */
	if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("pc_map GEN ok\n");
        else
            LOG_WARN("pc_map GEN notok\n");   
	gt_print(s);
	if(gt_is_unity(s))
		log("UNO\n");
	log("gt_mul\n");
	gt_mul(r, r, s); /* num_muls++; */
	gt_print(r);
	log("gt_free\n");
	gt_free(s);
	g1_free(a);
}




void dec_node_flatten( gt_t r, bn_t exp,
									gpsw_policy_t* p, g2_t* D, g1_t* E );
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
dec_internal_flatten( gt_t r,bn_t exp, gpsw_policy_t* p, g2_t* D, g1_t* E  )
{
	int i;
	bn_t e;
	bn_new(e);
	log("dec_internal_flatten\n");

	for( i = 0; i < p->satl_len; i++ )
	{	
		log("lagrange_coef i= %d\n", p->satl[i]);
		lagrange_coef(e,p->satl, p->satl_len, p->satl[i]);
		bn_mul(e,e,exp);
		log("exponet= ");
		bn_print(e);
		log("\n");
		dec_node_flatten(r, e, &p->children[p->satl[i] - 1], D,E);
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
									gpsw_policy_t* p, g2_t* D, g1_t* E )
{
	assert(p->satisfiable);
	print_node(p);
	if( p->children_len == 0 ){
		dec_leaf_flatten(r, exp, D[p->flag], E[p->flag]);
		log("dec_leaf_flatten %d\n",p->flag );
	}
	else
		dec_internal_flatten(r, exp, p,D, E);
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

	bn_new(one);

	bn_set_dig(one, 1);
	gt_set_unity(r);
	log("dec_node_flatten\n");
	dec_node_flatten(r, one, p,D, E);
	log("dec_flatten\n");
	bn_free(one);
}
