/*!	\file celia.h
 *
 *	\brief Routines for the Goyal-Pandey-Sahai-Waters ABE scheme.
 *	Include glib.h and pbc.h before including this file.
 *
 *	Copyright 2011 Yao Zheng.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "relic.h"
#undef LOG_MODULE
#undef LOG_LEVEL
#define LOG_MODULE "gpsw"
#include <abe_utils.h>
/* #define LOG_LEVEL LOG_LEVEL_DBG
#define log		LOG_DBG
#define raise_error LOG_ERR

#if LOG_LEVEL!=LOG_LEVEL_DBG
#undef g1_print
#define g1_print(...)
#undef g2_print
#define g2_print(...) 
#undef gt_print
#define gt_print(...) 
#undef bn_print
#define bn_print(...) 
#endif

 */
/**
 * @brief a polymnomial
 * 
 */
/* typedef struct
{
	int deg;
	// coefficients from [0] x^0 to [deg] x^deg 
	bn_t* coef ; // Z_p (of length deg + 1) 
}
gpsw_polynomial_t;

// polynomial new
#define gpsw_poly_new(_poly, _deg) \
	_poly=RLC_ALLOCA(gpsw_polynomial_t, 1); \
	_poly->deg=_deg;\
	_poly->coef=RLC_ALLOCA(bn_t, _deg+1); \
	for(int _i=0;_i<_deg+1;_i++)\
		bn_new(_poly->coef[_i]);

// polynomial free
#define gpsw_poly_free(_poly)\
	for(int _i=0;i<_poly->deg+1;_i++)\
		bn_free(_poly->coef[_i]);\
	RLC_FREE(_poly->coef);\
	RLC_FREE(_poly);
 */
/**
 * @brief a gpsw policy node
 * 
 */
/* typedef struct gpsw_policy_t gpsw_policy_t;
struct gpsw_policy_t
{
	short flag;        // attribute id if leaf, otherwise trashold 
	
	gpsw_policy_t* children; // pointers to gpsw_policy_t's, NULL for leaves 
	size_t children_len;
	short id;			// children id if not root, undefined otherwise 
	
	// only used during decryption 
	short satisfiable;
	short min_leaves;
	short* satl;
	size_t satl_len;
}; */


#define gpsw_policy_t abe_policy_t
#define gpsw_polynomial_t abe_polynomial_t
#define gpsw_poly_free abe_poly_free
#define gpsw_poly_new abe_poly_new
/**
 * @brief A private key.
 * 
 */
typedef  struct { 
	gpsw_policy_t*  p;
	int nodes;
	g2_t* D;      // G_2, for leaves  
} gpsw_prv_t;

/* size_t
strtok_count( char* s,  const char* delim ); */
// private key new
/*
#define gpsw_prv_new(priv,policy ) \
	size_t _n=strtok_count(policy, " ")+1;\
	priv=RLC_ALLOCA(gpsw_prv_t,1);\
	priv->p=RLC_ALLOCA(gpsw_policy_t,_n);\
	priv->nodes=_n;\
	log("priv_key nodes: %d\n", _n);\
	for(int _i=0;_i<_n;_i++){\
		g2_new(priv->p[_i].D);\
		priv->p[_i].satl=RLC_ALLOCA(short, _n);\
	}\
	priv->p[0].children=&priv->p[1];

// private key free

#define gpsw_prv_free(priv) \
	for(int _i=0;_i<_n;_i++){\
		g2_free(priv->p[_i].D);\
		RLC_FREE(priv->p[_i].satl);\
	}\
	RLC_FREE(priv->p);\
	RLC_FREE(priv);
	
*/
/*
  core function
*/
int gpsw_setup( gt_t Y, g1_t* T, bn_t y, bn_t* t, int num_attributes );

int gpsw_enc(gt_t Ep, g1_t* E, gt_t Y, g1_t* T, gt_t m, int num_attributes);

int gpsw_keygen( gpsw_prv_t* prv, bn_t y, bn_t* t, int num_attributes, char* policy);

int gpsw_dec( gt_t m ,gt_t Y,  gpsw_prv_t* prv, gt_t Ep, g1_t* E, int num_attributes );

  size_t
gpsw_enc_byte_array(
	 char* cph_buf, size_t cph_len,
	 gt_t Ep, g1_t* E, gt_t Y, g1_t* T, 
	 char* m, size_t m_len, size_t num_attributes );

 size_t
gpsw_dec_byte_array( 
	char* clear_buf, size_t clear_len,
	gt_t Y,	 gpsw_prv_t* prv, gt_t Ep, g1_t* E,
	char* cph_buf, size_t cph_len, size_t num_attributes ); 



