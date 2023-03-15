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


#include <relic.h>


#undef LOG_MODULE
#undef LOG_LEVEL
#define LOG_MODULE "bsw"
#define LOG_LEVEL LOG_LEVEL_DBG

#include <abe_utils.h>


#define bsw_policy_t abe_policy_t
/**
 * @brief A chipertext.
 * 
 */
typedef  struct { 
	abe_policy_t* policy; 
	int nodes;
	gt_t Cs;	// M e(g,k)^as
	g2_t c;      // h*s=g*bs
	g1_t* Cp;      /*  for leaves  */
	g2_t* C;      /*  for leaves  */
} bsw_cph_t;

/**
 * @brief A private key.
 * 
 */
typedef  struct { 
	int num_attributes;
	g1_t d;
	g1_t* Dp;      /*  for each attribute  */
	g2_t* D;      /* for each attribute  */
} bsw_prv_t;


/*
  core function
*/
int bsw_setup( g2_t h,  gt_t A, bn_t b, bn_t a );

int bsw_enc(bsw_cph_t* cph, g2_t h, gt_t A,  gt_t m, int num_attributes, char* policy);

int bsw_keygen( bsw_prv_t* prv, bn_t b, bn_t a, char* attributes, size_t num_attributes);

int bsw_dec( gt_t m ,bsw_cph_t* cph,  bsw_prv_t* prv);

/*  size_t
bsw_enc_byte_array(
	 char* cph_buf, size_t cph_len,
	 gt_t Ep, g1_t* E, 
	 g1_t Y, g1_t* T, 
	 char* m, size_t m_len, size_t num_attributes );

 size_t
bsw_dec_byte_array( 
	char* clear_buf, size_t clear_len,
	 g1_t Y, g1_t* T, 
	 gt_t Ep, g1_t* E, 
	 bsw_prv_t* prv,
	 char* cph_buf, size_t cph_len, size_t num_attributes ); */



