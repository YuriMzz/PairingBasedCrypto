
/*
 * Generate a Policy tree from the input policy string.
 *
 * @param root                      Pointer to the root of the policy
 * @param s				Policy string
 * @return				Policy root node data structure
 */
int
bsw_parse_policy_postfix( bsw_cph_t* cph, char* s )
{
	int i=0;
	short n_nodes=cph->nodes;
	short j=n_nodes-1;
    short leaf_id=0;
	char*  tok;
	
	abe_policy_t* pol= cph->policy;
	abe_policy_t stack[n_nodes];
	short top=0;

	char* s_tmp = RLC_ALLOCA(char,strlen(s)+1);
	strcpy(s_tmp,s);
	
	tok = strtok(s_tmp, "_");
	while( tok )
	{
		short k, n;
		
		log("tok : %10s\n",tok);
		if( sscanf(tok, "%hdof%hd", &k, &n) != 2 )
		{
			g1_map(cph->Cp[leaf_id],(uint8_t*)tok, strlen(tok) ); // H(attr)
			log("hash attribute %10s in G1\n", tok);
			/* push leaf token */
			stack[top].flag = leaf_id;
			stack[top].children = NULL;
			stack[top].children_len = 0;
			top++;
			leaf_id++;
		}
		else
		{
			log("parse %hdof%hd operator \n", k ,n);
			/* parse "kofn" operator */
			if( k < 1 )
			{
				raise_error("error parsing \"%s\": trivially satisfied operator \"%s\"\n", s_tmp, tok);
				return 0;
			}
			else if( k > n )
			{
				raise_error("error parsing \"%s\": unsatisfiable operator \"%s\"\n", s_tmp, tok);
				return 0;
			}
			else if( n == 1 )
			{
				raise_error("error parsing \"%s\": identity operator \"%s\"\n", s_tmp, tok);
				return 0;
			}
			else if( n > top )
			{
				raise_error("error parsing \"%s\": stack underflow at \"%s\"\n", s_tmp, tok);
				return 0;
			}
			/* pop n things and fill in children */
			for( i = 0; i <n; i++ )
			{
				pol[j]=stack[--top];
				pol[j].id=i;
				j--;
			}
			stack[top].flag = k;
			stack[top].id = 0;
			stack[top].children = &pol[j+1];
			stack[top].children_len = n;
			top++;
		}
		tok = strtok(NULL, "_");
	}
	log("exit\n");
	if( top > 1 )
	{
		raise_error("error parsing \"%s\": extra tokens left on stack\n", s_tmp);
		return 0;
	}
	else if( top < 1 )
	{
		raise_error("error parsing \"%s\": empty policy\n", s_tmp);
		return 0;
	}

	pol[0]=stack[0];

	for(int kn=0; kn<n_nodes; kn++)
		print_node(&pol[kn]);
	RLC_FREE(s_tmp);
	
	return 1;
}


/*!
 * Routine to fill out the Policy tree
 *
 * @param P				Pointer to Root node policy data structure
 * @param pub			Public key
 * @param msk			Master key
 * @param e				Root secret
 * @return				None
 */

int
bsw_fill_policy( bsw_cph_t* cph,abe_policy_t* p,  int num_attributes, bn_t e )
{
	

	if( p->children == NULL )
	{
        if(p->flag>=num_attributes){
            raise_error("invalid attribute id %d\n", p->flag);
            return 0;
        }
		log("fill leaf %d \n", p->flag);
		log("compute Cy: coef=\n");
		bn_print(e);
		g2_mul_gen(cph->C[p->flag], e);	
		
		//g2_print(cph->C[p->flag]);
		log("compute Cyp'\n");
		g1_mul(cph->Cp[p->flag], cph->Cp[p->flag], e);
		//g1_print(cph->Cp[p->flag]);
		
	}
	else
	{	
		int i;
		bn_t a;
		bn_new(a);
        abe_polynomial_t* poly;
        abe_poly_new(poly, (p->flag)-1);
        rand_poly(poly, e);	
	    log("rand_poly root= \n");
	    bn_print(e);
		log("childrens %d\n", p->children_len);
		for( i = 0; i < p->children_len; i++ )
		{
			log("bn_set_dig\n");
			bn_set_dig(a, i + 1);
			//bn_print(a);
			log("eval_poly\n");
			print_node(&p->children[i]);
			eval_poly(a, poly, a);
			log("q(%d)= ", i+1);
			bn_print(a);
			if(!bsw_fill_policy(cph,&p->children[i], num_attributes, a))
				return 0;
			log("recursive end\n");
			log("coef: ");
			bn_print(poly->coef[0]);
		}
        abe_poly_free(poly);
		bn_free(a);
	}
	log("return\n");
	
	return 1;
}


/*!
 * Generate polate key with the provided policy.
 *
*/
int bsw_enc(bsw_cph_t* cph, g2_t h, gt_t A,  gt_t m, int num_attributes, char* policy){
	
	/* initialize */
	log("parse policy\n");
	if(!bsw_parse_policy_postfix(cph,policy))
		return 0;

	bn_t s;
	bn_t n;
	bn_new(s);
	bn_new(n);
	pc_get_ord(n);

	log("generate s\n");
	bn_rand_mod(s,n);
	//bn_set_dig(s,1);

	/* compute */
	log("fill policy\n");
	if(!bsw_fill_policy(cph,cph->policy, num_attributes, s))
		return 0;

	log("compute Cs \n");
	gt_exp(cph->Cs, A,s);
	gt_mul(cph->Cs,cph->Cs, m );

	log("compute C\n");
	g2_mul(cph->c, h, s);

	bn_free(s);
	bn_free(n);

	return 1;
}