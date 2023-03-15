#include <string.h>
#include "sys/log.h"

/* size_t
strtok_count( char* s,  const char* delim )
{
	int count = 0;
	char *ptr = s;
	while((ptr = strpbrk(ptr, delim)) != NULL)
	{
		count++;
		ptr++;
	}

	return count;
}


# if LOG_LEVEL == LOG_LEVEL_DBG
void print_node(gpsw_policy_t* n){
	log("-------address= %p --------\n",(void*)n);    
	log("id= %d\n",n->id);   
	log("flag= %d\n",n->flag);      
	log("children= %p\n",(void*)n->children);      
	log("children_len= %d\n",n->children_len);  
	log("----------------------------------------\n");
}
#else
#define print_node(...)	// empty
#endif
 */

/*
 * Generate a Policy tree from the input policy string.
 *
 * @param root                      Pointer to the root of the policy
 * @param s				Policy string
 * @return				Policy root node data structure
 */
int
parse_policy_postfix( gpsw_policy_t* priv, char* s, short n_nodes )
{
	int i=0;
	short j=n_nodes-1;
    short attr_id;
	char*  tok;
	
	gpsw_policy_t stack[n_nodes];
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
            if(sscanf(tok, "%hd", &attr_id)==1){
			    /* push leaf token */
				stack[top].flag = attr_id;
				stack[top].children = NULL;
				stack[top].children_len = 0;
			   	top++;
            }else{
                raise_error("attributes id must be deciaml integers\n");
				return 0;
            }
			
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
				priv[j]=stack[--top];
				priv[j].id=i;
				j--;
			}
			stack[top].flag = k;
			stack[top].id = 0;
			stack[top].children = &priv[j+1];
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

	priv[0]=stack[0];

	for(int kn=0; kn<n_nodes; kn++)
		print_node(&priv[kn]);
	RLC_FREE(s_tmp);
	
	return 1;
}

/*!
 * Randomly generate the Lagrange basis polynomial base on provided constant value
 *
 * @param q                           Pointer to structure containing the lagrange basis polynomial
 * @param zero_val		Constant value of the lagrange basis polynomial
 * @return				Lagrange basis polynomial data structure
 */
/* void
rand_poly( gpsw_polynomial_t* q, bn_t zero_val )
{
	int i;
	bn_t n;
	bn_new(n);
	g1_get_ord(n);

	log("deg %d\n",q->deg);
	log("bn_rand_copy %p\n",(void*) q->coef);
	bn_copy(q->coef[0],zero_val);
	
	for( i = 1; i < (q->deg + 1); i++ ){
		log("deg %d - i %d\n",q->deg, i);
		log("bn_rand_mod %p\n", (void*)q->coef[i]);
		bn_rand_mod(q->coef[i], n);
	}
	bn_free(n);
} */

/*!
 * Compute the constant value of the child node's Lagrange basis polynomial,
 *
 * @param r				Constant value of this child node's Lagrange basis polynomial
 * @param q				Pointer to the lagrange basis polynomial of parent node
 * @param x				index of this child node in its parent node
 * @return				None
 */
/* 
void
eval_poly( bn_t r, gpsw_polynomial_t* q, bn_t x )
{
	 int i;
	bn_t s, t, tmp, n;
	bn_new(tmp);
	bn_new(s);
	bn_new(t);
	bn_new(n);
	pc_get_ord(n);
	log("poly x: ");
		bn_print(x);
	bn_zero(tmp);
	bn_set_dig(t, 1);
	log("eval_poly:\n");
	for( i = 0; i < q->deg + 1; i++ )
	{
		log("coef: ");
		bn_print(q->coef[i]);
		// r += q->coef[i] * t 
		bn_mul(s, q->coef[i], t);
		bn_mod(s,s,n);
		bn_add(tmp, tmp, s);
		bn_mod(tmp,tmp,n);
		// t *= x 
		bn_mul(t, t, x);
		bn_mod(t,t,n);
	}
	bn_free(s);
	bn_free(t); 
	bn_free(n); 
	bn_copy(r,tmp);
	bn_free(tmp);
	
	
} */
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
fill_policy( g2_t* D,gpsw_policy_t* p,  bn_t* t, int num_attributes, bn_t e )
{
	int i;
	bn_t a;
	bn_t n; 

	bn_new(a);
	bn_new(n);
	pc_get_ord(n);

	if( p->children == NULL )
	{
        if(p->flag>=num_attributes){
            raise_error("invalid attribute id %d\n", p->flag);
            return 0;
        }
		log("fill leaf %d \n", p->flag);
		bn_mod_inv(a, t[p->flag],n ); // 1/ti
		log( "1/ti\n");
		bn_print(e);
		log("a address= %p\n",(void*)a );
		bn_mul(a, e, a);	// q(0)/ti
		bn_mod(a, a, n);
		log( "q(0)/ti\n");
		g2_mul_gen(D[p->flag], a);	
		log("D address= %p\n",(void*)D[p->flag]);		// h*[q(0)/ti]
		log( "h*[q(0)/ti]\n");
		g2_print(D[p->flag]);
		
	}
	else
	{	
        gpsw_polynomial_t* poly;
        gpsw_poly_new(poly, p->flag-1);
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
			if(!fill_policy(D,&p->children[i], t, num_attributes, a))
				return 0;
			log("recursive end\n");
			log("coef: ");
			bn_print(poly->coef[0]);
		}
        gpsw_poly_free(poly);
	}
	log("return\n");
	bn_free(a);
	bn_free(n);
	return 1;
}


/*!
 * Generate private key with the provided policy.
 *
*/
int
gpsw_keygen( gpsw_prv_t* prv,  bn_t y, bn_t* t, int num_attributes,char* policy){
	
	/* initialize */
	log("parse policy\n");
	if(!parse_policy_postfix(prv->p,policy, prv->nodes))
		return 0;
	/* compute */
	log("fill policy\n");
	if(!fill_policy(prv->D,prv->p, t, num_attributes, y))
		return 0;

	return 1;
}