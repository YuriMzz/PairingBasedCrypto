

#include "relic.h"


#define bbs2_gen cp_bbs_gen

/**
 * @brief compute the checksum of a byte (xor of every bit)
 * 
 * @param b byte
 * @return uint8_t chekcsum in the most significant bit, 0 elsewhere
            0x80 if checksum=1
            0x00 if checksum=0
 */
uint8_t checksum(uint8_t b){
    uint8_t c=b;
    for(int i=0; i<8; i++){
        b<<=1;
        c=c^b;
    }
    return c & 0x80;
}

/**
 * @brief sing 
 * 
 * @param s 
 * @param b 
 * @param msg 
 * @param len 
 * @param d 
 * @return int 
 */
int 
bbs2_sig(g1_t s, uint8_t* b,const uint8_t *msg,  size_t len, const bn_t d){

    uint8_t m[RLC_MD_LEN] ;
    size_t bn_size=bn_size_bin(d);
    uint8_t bm[bn_size+len];
    memcpy(bm, msg, len);
    bn_write_bin(bm+len,bn_size,d); 
    md_map(m,bm,bn_size+len); // H(Sk, M)
    *b=checksum(m[1]);   // take the first byte checksum (b) 
    memcpy(bm+len, b, 1); // and sing it with the message ->(b,M)
    return cp_bbs_sig(s, bm,len+1, 0,d); // internally: m=H(b,M)
}

/**
 * @brief 
 * 
 * @param s 
 * @param b 
 * @param msg 
 * @param len 
 * @param q 
 * @param z 
 * @return int 
 */
int 
bbs2_ver( g1_t s,uint8_t b, const uint8_t *msg, size_t len, 
    const g2_t q,const gt_t z) {

    uint8_t m[len+1] ;
    memcpy(m, msg, len);
    m[len]=b;
    return cp_bbs_ver(s, m, len+1,0, q,z);
}

/**
 * @brief 
 * 
 * @param bin 
 * @param len 
 * @param a 
 * @param b 
 */
void bbs2_sing_write_bin(uint8_t *bin, int len, const ep_t a, uint8_t b){
    ep_write_bin(bin, len,  a,1);
    bin[0]|=b; // most significant bit used to encode the additional byte
}

/**
 * @brief 
 * 
 * @param a 
 * @param b 
 * @param bin 
 * @param len 
 */
void bbs2_sing_read_bin(ep_t a,  uint8_t* b, uint8_t *bin, int len){
    if(len>0){
        *b=bin[0] & 0x80; // most significant bit used to encode the additional byte
        bin[0] &= 0x7F; //remove additional information
    }
    ep_read_bin(a,bin, len);
    bin[0]|=*b; //restore additional information
}