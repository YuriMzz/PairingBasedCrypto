

#ifndef ABE_UTILS_H
#define ABE_UTILS_H
#include <relic.h>
/**
 * @brief a polymnomial
 * 
 */
typedef struct
{
	int deg;
	/* coefficients from [0] x^0 to [deg] x^deg */
	bn_t* coef ; /* Z_p (of length deg + 1) */
}
abe_polynomial_t;
// polynomial new
#define abe_poly_new(_poly, _deg) \
	_poly=RLC_ALLOCA(abe_polynomial_t, 1); \
	_poly->deg=_deg;\
	_poly->coef=RLC_ALLOCA(bn_t, _deg+1); \
	for(int _i=0;_i<_deg+1;_i++)\
		bn_new(_poly->coef[_i]);

// polynomial free
#define abe_poly_free(_poly)\
	for(int _i=0;i<_poly->deg+1;_i++)\
		bn_free(_poly->coef[_i]);\
	RLC_FREE(_poly->coef);\
	RLC_FREE(_poly);

/**
 * @brief a abe policy node
 * 
 */
typedef struct abe_policy_t abe_policy_t;
struct abe_policy_t
{
	/* serialized */
	short flag;        /* attribute id if leaf, otherwise trashold */   
	
	abe_policy_t* children; /* pointers to abe_policy_t's, NULL for leaves */
	size_t children_len;
	short id;			// children id if not root, undefined otherwise 
	
	/* only used during decryption */
	short satisfiable;
	short min_leaves;
	short* satl;
	size_t satl_len;
};

/**
 * @brief 
 * 
 * @param n 
 * @param m 
 * @return int 
 */
int gcd(int n, int m);


/**
 * @brief Helper method: Counts the number of tokens in the string
 * 
 * @param s a string
 * @param delim a delimiter
 * @return size_t 
 */
size_t strtok_count( char* s,  const char* delim );




/*!
 * Function that compare the minimal leaves of two child policy node of the same parent node
 *
 * @param a				index of first child node in its parent node
 * @param b				index of second child node in its parent node
 * @return	k			compare result
 */
int cmp_int( const void* a, const void* b );

/*!
 * Choose the path with minimal leaves node from all possible path which are marked as satisfiable
 * Mark the respective "min_leaves" element in the policy node data structure
 *
 * @param p				Policy node data structure (root)
 * @return				None
 */
void pick_sat_min_leaves( abe_policy_t* p );

/*!
 * Compute Lagrange coefficient
 *
 * @param s				satisfiable node set
 * @param s_len         length of node set
 * @param i				index of this node in the satisfiable node set
 * @return				Lagrange coefficient
 */
void lagrange_coef( bn_t coef,short int* s, size_t s_len, int i );


/*!
 * Randomly generate the Lagrange basis polynomial base on provided constant value
 *
 * @param q             pointer to structure containing the lagrange basis polynomial
 * @param zero_val		Constant value of the lagrange basis polynomial		
 */
void rand_poly( abe_polynomial_t* q, bn_t zero_val );


/*!
 * Compute r=q(x) where q is a polynomial ,
 *
 * @param r				result
 * @param q				Pointer to the lagrange basis polynomial 
 * @param x				argument
 */
void eval_poly( bn_t r, abe_polynomial_t* q, bn_t x );


#include "sys/log.h"
#include "abe_utils.h"
#define log		LOG_DBG
#define raise_error LOG_ERR

/* #if LOG_LEVEL!=LOG_LEVEL_DBG
#undef g1_print
#define g1_print(...)
#undef g2_print
#define g2_print(...) 
#undef gt_print
#define gt_print(...) 
#undef bn_print
#define bn_print(...) 

#endif */

#define LOG_LEVEL LOG_LEVEL_INFO
#if LOG_LEVEL==LOG_LEVEL_DBG
void print_node(abe_policy_t* n);
#else
#define print_node(...)	/* empty */
#endif

#endif