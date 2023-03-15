/*!
 * Function that compare the minimal leaves of two child policy node of the same parent node
 *
 * @param a				index of first child node in its parent node
 * @param b				index of second child node in its parent node
 * @return	k			compare result
 */

/* gpsw_policy_t* cur_comp_pol;
int
cmp_int( const void* a, const void* b )
{
	int k, l;

	k = cur_comp_pol->children[*((int*)a)].min_leaves;
	l = cur_comp_pol->children[*((int*)b)].min_leaves;

	return
		k <  l ? -1 :
		k == l ?  0 : 1;
} */


/*!
 * Choose the path with minimal leaves node from all possible path which are marked as satisfiable
 * Mark the respective "min_leaves" element in the policy node data structure
 *
 * @param p				Policy node data structure (root)
 * @return				None
 */

/* void
pick_sat_min_leaves( gpsw_policy_t* p )
{
	int i, k, l = 0;
	int* c;

	assert(p->satisfiable == 1);
	print_node(p);
	if( p->children_len == 0 )
		p->min_leaves = 1;
	else
	{
		for( i = 0; i < p->children_len; i++ )
			if( p->children[i].satisfiable )
				pick_sat_min_leaves(&p->children[i]);

		c = RLC_ALLOCA(int , p->children_len);
		for( i = 0; i < p->children_len; i++ )
			c[i] = i;

		cur_comp_pol = p;
		qsort(c, p->children_len, sizeof(int), cmp_int);

		p->satl_len = 0;
		p->min_leaves = 0;
		l = 0;
		for( i = 0; i < p->children_len && l < p->flag; i++ )
			if( p->children[c[i]].satisfiable )
			{
				l++;
				p->min_leaves += p->children[c[i]].min_leaves;
				k = c[i] + 1;
				p->satl[p->satl_len++] = k;
			}
		assert(l == p->flag);

		RLC_FREE(c);
	}
} */

/*!
 * Compute Lagrange coefficient
 *
 * @param r				Lagrange coefficient
 * @param s				satisfiable node set
 * @param s_len                    length of node set
 * @param i				index of this node in the satisfiable node set
 * @return				None
 */

/* int
lagrange_coef(  short int* s, size_t s_len, int i )
{
	short int k;
	float kr=1.;

	log("i = %d\n", i);
	for( k = 0; k < s_len; k++ )
	{
		if( s[k] == i )
			continue;
		kr= -(float)s[k]*kr;
		kr=kr/(float)(i-s[k]);
	}
	
	log("lagrange coeff= %d\n", (int)(kr < 0 ? (kr - 0.5) : (kr + 0.5)));
	return (int)(kr < 0 ? (kr - 0.5) : (kr + 0.5));
} */

/*!
 * Check whether the attributes in the ciphertext data structure can
 * access the root secret in the policy data structure, and mark all
 * possible path
 *
 * @param p				Policy node data structure (root)
 * @param cph			Ciphertext data structure
 * @param oub			Public key data structure
 * @return				None
 */

int
check_sat( gpsw_policy_t* p,g2_t* D, int num_attributes )
{
	int i, l;

	p->satisfiable = 0;
	if( p->children_len == 0 )
	{
		if(p->flag>=num_attributes){
            raise_error("invalid attribute id %d\n", p->flag);
            return 0;
        }
		if(g2_is_valid(D[p->flag]))
        	p->satisfiable = 1;
		else
			return 0;
    }
	else
	{
		for( i = 0; i < p->children_len; i++ )
			if(!check_sat(&p->children[i], D, num_attributes ))
			{
				return 0;
			}

		l = 0;
		for( i = 0; i < p->children_len; i++ )
			if( p->children[i].satisfiable )
			{
			    l++;
			}
		
		if( l >= p->flag )
			p->satisfiable = 1;
	}

	return 1;
} 


#ifdef GPSW_SIM
#include "gpsw_decrypt_sim.c"
#else
#include "gpsw_decrypt_seq.c"
#endif

/*!
 * Decrypt the secret message m
 *
 * @param pub				Public key data structure
 * @param prv				Private key data structure
 * @param cph				Ciphertext data structure
 * @param m_e					Secret message
 * @return int				Successfully decrypt or not
 */

int
gpsw_dec( gt_t m ,
    gt_t Y, gpsw_prv_t* prv, gt_t Ep, g1_t* E, int num_attributes )
{
	gt_t Ys;
	gt_new(Ys);
	
	if(!check_sat(prv->p, prv->D, num_attributes))
		return 0;
	log("check sat\n");
 	if( !prv->p[0].satisfiable )
	{
		raise_error("cannot decrypt, attributes in ciphertext do not satisfy policy\n");
		return 0;
	}
	for(int i=0; i<prv->nodes;i++){
		if((prv->p[i].satisfiable==1) && (prv->p[i].children_len!=0))
			prv->p[i].satl=RLC_ALLOCA(short,prv->p[i].children_len);
	}
	log("pick_sat_min_leaves\n");
	pick_sat_min_leaves(prv->p);
	if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("pick_sat_min_leaves GEN ok\n");
        else
            LOG_WARN("pick_sat_min_leaves GEN notok\n");   
	log("dec_flatten\n");
	dec_flatten(Ys, prv->p,prv->D,E);
	log("#### Ys = ");
	gt_print(Ys);
	log("gt_inv\n");
	gt_inv(Ys, Ys); // 1/Ys
	log("gt_mul\n");
	gt_mul(m, Ep, Ys); // E'/Ys

	gt_free(Ys);
	for(int i=0; i<prv->nodes;i++){
		if((prv->p[i].satisfiable==1) && (prv->p[i].children_len!=0))
			RLC_FREE(prv->p[i].satl);
	}
	return 1;
}