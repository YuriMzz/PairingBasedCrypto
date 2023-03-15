#include "abe_utils.h"
#include <string.h>
#include <assert.h>

size_t
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




abe_policy_t* cur_comp_pol;
int
cmp_int( const void* a, const void* b )
{
	int k, l;

	k = cur_comp_pol->children[*((int*)a)].min_leaves;
	l = cur_comp_pol->children[*((int*)b)].min_leaves;

	return
		k <  l ? -1 :
		k == l ?  0 : 1;
}



void pick_sat_min_leaves( abe_policy_t* p )
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
}



int gcd(int n, int m)
{
	int gcd, remainder;
 
	while (n != 0)
	{
		remainder = m % n;
		m = n;
		n = remainder;
	}
 
	gcd = m;
 
	return gcd;
}

/* void lagrange_coef( bn_t coef,short int* s, size_t s_len, int i )
{
	short int k;
	int num=1,den=1,_gcd;
	bn_t n;
	bn_new(n);
	pc_get_ord(n);
	log("\n");
	log("order= ");
	bn_print(n);
	log("\n");
	
	log("i = %d\n", i);
	for( k = 0; k < s_len; k++ )
	{
		LOG_INFO_("%hd ",s[k]);
		
		if( s[k] == i )
			continue;
		num= s[k]*num;
		den=den*(s[k]-i);
		_gcd=gcd(abs(num),abs(den));
		num=num/_gcd;
		den=den/_gcd;
	}
	LOG_INFO_("%d/%d ", num, den);
	bn_set_dig(coef,abs(den));
	bn_mod_inv(coef,coef,n);
	bn_mul_dig(coef,coef,abs(num));
	if((den*num)<0)
		bn_neg(coef,coef);
	bn_mod(coef,coef,n);
	bn_print(coef);
	bn_free(n);
} */

void lagrange_coef( bn_t coef,short int* s, size_t s_len, int i )
{
	short int k;
	int den=1;
	bn_t n,a;
	bn_new(n);
	bn_new(a);
	pc_get_ord(n);
	/* log("\n");
	log("order= ");
	bn_print(n);
	log("\n"); */
	bn_set_dig(coef,1);
	log("i = %d\n", i);
	for( k = 0; k < s_len; k++ )
	{
		LOG_DBG_("%hd ",s[k]);
		
		if( s[k] == i )
			continue;
		bn_mul_dig(coef,coef,s[k]);
		bn_mod(coef,coef,n);
		den=(s[k]-i);
		bn_set_dig(a,abs(den));
		bn_mod_inv(a,a,n);
		bn_mul(coef,coef,a);
		if(den<0)
			bn_neg(coef,coef);
	}
	bn_mod(coef,coef,n);
	bn_print(coef);
	bn_free(a);
	bn_free(n);
} 




void rand_poly( abe_polynomial_t* q, bn_t zero_val )
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
		//bn_set_dig(q->coef[i],1);
	}
	bn_free(n);
}

void eval_poly( bn_t r, abe_polynomial_t* q, bn_t x )
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
	
}

#if LOG_LEVEL==LOG_LEVEL_DBG
void print_node(abe_policy_t* n){
	log("-------address= %p --------\n",(void*)n);    
	log("id= %d\n",n->id);   
	log("flag= %d\n",n->flag);       /* attribute string if leaf, otherwise null */
	log("children= %p\n",(void*)n->children);       /* pointers to abe_policy_t's, NULL for leaves */
	log("children_len= %d\n",n->children_len);  
	log("----------------------------------------\n");
}
#endif