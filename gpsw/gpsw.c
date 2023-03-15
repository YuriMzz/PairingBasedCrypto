/*!	\file core.c
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


#include <assert.h>
#include "gpsw.h"

/********************************************************************************
 * Goyal-Pandey-Sahai-Waters Implementation
 ********************************************************************************/

/*!
 * Generate public and master key with the provided attributes list.
 *
 * @return				0 on error 1 on success
 */

int gpsw_setup( gt_t Y, g1_t* T, bn_t y, bn_t* t, int num_attributes )
{
	bn_t n;
	int i;

	bn_new(n);
	pc_get_ord(n);
	
	log("get geterators\n");
	/* compute */
 	bn_rand_mod(y, n);

	log("compute Y\n");
	gt_exp_gen(Y, y); // Y=e(g, h)^y
	log("n_comp %d\n", num_attributes);

	for( i = 0; i < num_attributes; i++)
	{
 		bn_rand_mod(t[i], n);
		g1_mul_gen(T[i], t[i]);
	}
	bn_free(n);
	return 1;
}



/*!
 * Encrypt a secret message with the provided attributes list, return a ciphertext
 *
 */


int gpsw_enc(gt_t Ep, g1_t* E, gt_t Y, g1_t* T, gt_t m, int num_attributes)
{
 	bn_t s;
	bn_t n;
	int i;

	/* initialize */
	bn_new(n);
	bn_new(s);
	
	g1_get_ord(n);

	/* compute */
 	bn_rand_mod(s, n);
	
	gt_exp(Ep, Y, s);
	log("#### Ys = ");
	gt_print(Ep);
	log("compute  EP\n");
	gt_mul(Ep, Ep, m);
	
	for( i = 0; i < num_attributes; i++)
	{
		log("g1_mul_key %p - %p\n", (void*)E[i],(void*) T[i]);
		g1_mul_key(E[i], T[i], s);
		g1_print(E[i]);
	}

	bn_free(s);
	bn_free(n);

	return 1;
}


#include "gpsw_keygen.c"
//#define GPSW_SIM
#include "gpsw_decrypt.c"


/*
 * @brief obtain a fixed-lenght key from a gt element
 * 
 * @param gt_key gt element
 * @param key a key
 * @param key_size lenght of the rewuired key
 * @return 1 if ok, 0 if error(s) occur 
 */
 
int gt_to_key(gt_t gt_key, uint8_t* key, size_t key_size){
	size_t gt_len=gt_size_bin(gt_key, 1);
	uint8_t gt_bin[gt_len];
	uint8_t digest[RLC_MD_LEN];
	if(key_size > RLC_MD_LEN){
		LOG_ERR("digest size smaller than key size");
		return 0;
	}
	gt_write_bin(gt_bin, gt_len,gt_key, 1);
	md_map(digest, gt_bin,gt_len);
	memcpy(key, digest,key_size);
	return 1;
}

/*!
 * Encrypt a secret message with the provided attributes list, return a ciphertext.
 *
 * @param cph_bin       bytearray ciphertext buffer
 * @param buf_len		avaliable bytes on bytearray ciphertext buffer
 * @param cph			gpsw chipertext
 * @param pub			Public key structure
 * @param m				Byte array containing plaintext
 * @param m_len         Length of the plaintext
 * @param attributes	Attributes list
 * @return				Length of ciphertext
*/
 size_t
gpsw_enc_byte_array(
	 char* cph_buf, size_t cph_len,
	 gt_t Ep, g1_t* E, gt_t Y, g1_t* T, 
	 char* m, size_t m_len, size_t num_attributes )
{
	uint8_t aes_key[RLC_BC_LEN];
	gt_t gt_key;

	if(cph_len<RLC_BC_LEN)
		return 0;

	gt_new(gt_key);
	gt_rand(gt_key);
	if(!gt_to_key(gt_key, aes_key,RLC_BC_LEN )){
		gt_free(gt_key);
		return 0;
	}
		
	// encrypt gt key with gpsw
	if(!gpsw_enc(  Ep, E,Y, T, gt_key, num_attributes )){
		gt_free(gt_key);
		return 0;
	}
	gt_free(gt_key);

	// generate iv and write it before the chipertext
	char* iv_buf= cph_buf;
	cph_buf=iv_buf+RLC_BC_LEN;
	rand_bytes((uint8_t*)iv_buf,RLC_BC_LEN);
	cph_len-=RLC_BC_LEN;

	// encrypt bytearray with aes
	if(bc_aes_cbc_enc(
		(uint8_t*) cph_buf, &cph_len,
		(uint8_t*) m, m_len, 
		aes_key, RLC_BC_LEN, (uint8_t*)iv_buf )
		!=RLC_OK)
			return 0;
	
	return cph_len+=RLC_BC_LEN;
	

} 

/*!
 * @brief decrypt a binary message with the gpsw private key
 * 
 * @param clear_buf buffer for the decrypter message
 * @param clear_len avaliable space in the clear message buffer
 * @param pub gpsw publick key
 * @param prv gpsw private key
 * @param cph_bin the encrypted message
 * @param cph_len encrypted messsage size
 * @param cph gpsw chipertext
 * @return clear message size, 0 if error(s) occur
 */

 size_t
gpsw_dec_byte_array( char* clear_buf, size_t clear_len, gt_t Y,  gpsw_prv_t* prv, gt_t Ep, g1_t* E,
	 char* cph_buf, size_t cph_len, size_t num_attributes)
{
	uint8_t aes_key[RLC_BC_LEN];
	gt_t gt_key;


	if(cph_len<RLC_BC_LEN)
		return 0;
	gt_new(gt_key);
        
	// decrypt gt_key with gpsw
	if(!gpsw_dec( gt_key,Y, prv, Ep, E,num_attributes )){
		gt_free(gt_key);
		return 0;
	}

	if(!gt_to_key(gt_key, aes_key,RLC_BC_LEN )){
		gt_free(gt_key);
		return 0;
	}
	gt_free(gt_key);

	char* iv_buf=cph_buf;
	cph_buf=iv_buf+RLC_BC_LEN;
	cph_len-=RLC_BC_LEN;

	// decrypt bytearray with aes
	if(bc_aes_cbc_dec(
		(uint8_t*)clear_buf,&clear_len, 
		(uint8_t*)cph_buf,cph_len,
		aes_key,  RLC_BC_LEN, (uint8_t*)iv_buf )
		!=RLC_OK)
			return 0;
	
	return clear_len;
} 

