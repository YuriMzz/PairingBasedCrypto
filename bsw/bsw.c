/*!	\file bsw.c
 *
 *	\brief 
 */


#include <assert.h>
#include <bsw.h>

/********************************************************************************
 * GBethencourt–Sahai–Waters Attribute Base Encryption scheme implementation
 ********************************************************************************/

/*!
 * Generate public and master key with the provided attributes list.
 *
 * @return				0 on error 1 on success
 */

int bsw_setup( g2_t h,  gt_t A, bn_t b, bn_t a )
{
	bn_t n;

	bn_new(n);
	pc_get_ord(n);
	
	log("get geterators\n");
 	bn_rand_mod(b, n);
	bn_rand_mod(a, n);

	log("compute A\n");
	gt_exp_gen(A, a); // A=e(m, g)^a

	g2_mul_gen(h,b); // h=g*a

	bn_free(n);
	return 1;
}



/*!
 * Encrypt a secret message with the provided attributes list, return a ciphertext
 *
 */


int bsw_keygen( bsw_prv_t* prv, bn_t b, bn_t a, char* attributes, size_t num_attributes)
{

	char* s_tmp = RLC_ALLOCA(char,strlen(attributes)+1);
	strcpy(s_tmp,attributes);
 	
	bn_t n;
	bn_new(n);
	pc_get_ord(n);

	bn_t r;
	bn_new(r);
	
	bn_t c;
	bn_new(c);
	
	bn_rand_mod(r, n);
	//bn_set_dig(r,1);
	g1_t m_r;
	g1_new(m_r);
	g1_mul_gen(m_r,r); // m*r

	log("compute  D\n");
	
	bn_add(r,r,a); // a + r
	bn_mod(r, r, n); // (a+r) mod n
	g1_print(prv->d);
	bn_mod_inv(c,b,n);	// 1/b mod n
	bn_mul(c,r,c);	// (a+r)/b mod n
	g1_mul_gen(prv->d,c); 
	log("D=\n");
	g1_print(prv->d);

	char* tok = strtok(s_tmp, "_");
	int i;
	for( i = 0; i < num_attributes && tok; i++)
	{
		log("hash attribute %10s in G1\n", tok);
		g1_map(prv->Dp[i],(uint8_t*)tok, strlen(tok) ); // H(attr)

		log("compute Dj\n");
		bn_rand_mod(r, n); // rj
		//bn_set_dig(r,1);
		g1_mul(prv->Dp[i],prv->Dp[i], r);// m_k*1 + H(attr)*rj
		g1_add(prv->Dp[i],prv->Dp[i],m_r); 
		if(g1_is_valid(prv->Dp[i]))
		    LOG_DBG("Dyp add OK\n");
	    else
		    LOG_DBG("Dyp add TRASH\n");

		g1_print(prv->Dp[i]);
		
		log("compute Dj'\n");
		g2_mul_gen(prv->D[i], r);
		g2_print(prv->D[i]);

		tok = strtok(NULL, "_");
	}

	
	RLC_FREE(s_tmp);
	bn_free(n);
	bn_free(r);
	bn_free(c);
	g1_free(m_r);

	if (i!= num_attributes){
		LOG_WARN("wrong number of attributes: %d\n", i);
		return 0;

	}
		
	return 1;
}

#include "bsw_encrypt.c"
/*!
 * Check whether the attributes in the ciphertext data structure can
 * access the root secret in the policy data structure, and mark all
 * possible path
 *
 * @param p				Policy node data structure (root)
 * @param prv			private key data structure
 * @return				1 if ok, 0 otherwise
 */

int bsw_check_sat( abe_policy_t* p, bsw_prv_t* prv )
{
	int i, l;

	p->satisfiable = 0;
	if( p->children_len == 0 )
	{
		if(p->flag>=prv->num_attributes){
            raise_error("invalid attribute id %d\n", p->flag);
            return 0;
        }
		if(g2_is_valid(prv->D[p->flag]) && g1_is_valid(prv->Dp[p->flag]))
        	p->satisfiable = 1;
		else
			return 0;
    }
	else
	{
		for( i = 0; i < p->children_len; i++ )
			if(!bsw_check_sat(&p->children[i], prv ))
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

#ifdef BSW_SIM
#include "bsw_decrypt_sim.c"
#else
#include "bsw_decrypt_seq.c"
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

int bsw_dec( gt_t m ,bsw_cph_t* cph,  bsw_prv_t* prv)
{
	if(!bsw_check_sat(cph->policy, prv))
		return 0;
	log("check sat\n");
 	if( !cph->policy[0].satisfiable )
	{
		raise_error("cannot decrypt, attributes in ciphertext do not satisfy policy\n");
		return 0;
	}
	for(int i=0; i<cph->nodes;i++){
		if((cph->policy[i].satisfiable==1) && (cph->policy[i].children_len!=0))
			cph->policy[i].satl=RLC_ALLOCA(short,cph->policy[i].children_len);
	}
	log("pick_sat_min_leaves\n");
	pick_sat_min_leaves(cph->policy);
	if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("pick_sat_min_leaves GEN ok\n");
        else
            LOG_WARN("pick_sat_min_leaves GEN notok\n");   

	log("dec_flatten\n");
	dec_flatten(m,cph->policy, cph, prv);

	for(int i=0; i<cph->nodes;i++){
		if((cph->policy[i].satisfiable==1) && (cph->policy[i].children_len!=0))
			RLC_FREE(cph->policy[i].satl);
	}
	return 1;
}


/**
 * @brief obtain a fixed-lenght key from a gt element
 * 
 * @param gt_key gt element
 * @param key a key
 * @param key_size lenght of the rewuired key
 * @return 1 if ok, 0 if error(s) occur 
 
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
*/
/*!
 * Encrypt a secret message with the provided attributes list, return a ciphertext.
 *
 * @param cph_bin       bytearray ciphertext buffer
 * @param buf_len		avaliable bytes on bytearray ciphertext buffer
 * @param cph			bsw chipertext
 * @param pub			Public key structure
 * @param m				Byte array containing plaintext
 * @param m_len         Length of the plaintext
 * @param attributes	Attributes list
 * @return				Length of ciphertext
 

 size_t
bsw_enc_byte_array_relic( char* cph_bin, size_t buf_len, bsw_cph_t* cph, bsw_pub_t* pub, char*  m, size_t m_len, char** attributes )
{
	uint8_t aes_key[RLC_BC_LEN];
	gt_t gt_key;

	gt_new(gt_key);
	gt_rand(gt_key);
	if(!gt_to_key(gt_key, aes_key,RLC_BC_LEN )){
		gt_free(gt_key);
		return 0;
	}
		
	// encrypt gt key with bsw
	if(!bsw_enc_relic( cph, pub, gt_key, attributes)){
		gt_free(gt_key);
		return 0;
	}
	gt_free(gt_key);

	// generate iv and write it before the chipertext
	uint8_t* iv_buf=(uint8_t*) cph_bin;
	uint8_t* cph_buf=iv_buf+RLC_BC_LEN;
	rand_bytes(iv_buf,RLC_BC_LEN);
	buf_len-=RLC_BC_LEN;

	// encrypt bytearray with aes
	if(bc_aes_cbc_enc(cph_buf, &buf_len,(uint8_t*) m, m_len, aes_key, RLC_BC_LEN, iv_buf )!=RLC_OK)
		return 0;
	
	return buf_len+=RLC_BC_LEN;
	

} 
*/
/**
 * @brief decrypt a binary message with the bsw private key
 * 
 * @param clear_buf buffer for the decrypter message
 * @param clear_len avaliable space in the clear message buffer
 * @param pub bsw publick key
 * @param prv bsw private key
 * @param cph_bin the encrypted message
 * @param cph_len encrypted messsage size
 * @param cph bsw chipertext
 * @return clear message size, 0 if error(s) occur
 

 size_t
bsw_dec_byte_array_relic( char* clear_buf, size_t clear_len, bsw_pub_t* pub, bsw_prv_t* prv, char * cph_bin, size_t cph_len, bsw_cph_t* cph)
{
	uint8_t aes_key[RLC_BC_LEN];
	gt_t gt_key;
	gt_new(gt_key);
        
	// decrypt gt_key with bsw
	if(!bsw_dec_relic( pub,  prv,  cph, gt_key )){
		gt_free(gt_key);
		return 0;
	}

	if(!gt_to_key(gt_key, aes_key,RLC_BC_LEN )){
		gt_free(gt_key);
		return 0;
	}
	gt_free(gt_key);

	uint8_t* iv_buf=(uint8_t*)cph_bin;
	uint8_t* cph_buf=iv_buf+RLC_BC_LEN;
	cph_len-=RLC_BC_LEN;

	// decrypt bytearray with aes
	if(bc_aes_cbc_dec((uint8_t*)clear_buf,&clear_len, cph_buf,cph_len, aes_key,  RLC_BC_LEN, iv_buf )!=RLC_OK)
		return 0;
	
	return clear_len;
} 
*/
