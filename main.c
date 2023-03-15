#include "contiki.h"
#include "int-master.h" /* for int_master_read_and_disable(void) */
#include "relic.h"
#include <stdarg.h>
#include <assert.h>
#include <stdio.h> /* For LOG_DBG() */

#if defined GPSW
#include "gpsw.h"
#endif

//#define util_print(F, ...)		LOG_DBG(F, ##__VA_ARGS__)
#include "sys/log.h"
#include "sys/stack-check.h"
#undef LOG_MODULE
#undef LOG_LEVEL
#define LOG_MODULE "Main"
#define LOG_LEVEL LOG_LEVEL_INFO


#ifdef BENCH_TIME

/* /BENCH_TIME */
#define REPETITIONS 1
#define CLOCK RTIMER_NOW
#define CLOCK_TICKS_PER_SECOND RTIMER_SECOND
#define CLOCK_TICKS_PER_MS (CLOCK_TICKS_PER_SECOND/1000)
#include "time-util.h"
#define INIT \
    LOG_INFO_("function started\n"); \
    BINIT
#define RUN BRUN
#define END BEND
#define S_INIT    \
    for(int _r=0;_r<REPETITIONS;_r++){
#define S_END END 
#else 

#ifdef BENCH_SPACE
/* /BENCH_SPACE */
#define S_INIT
#define S_END 
#include "stack-util.h"
#define INIT(LABEL)                                                                                                           \
    LOG_INFO_("STACK: " LABEL "%*c = ", (int)(32 - strlen(LABEL)), ' '); \
    for (int _i = 0; _i < 1; ++_i) {
#define RUN(FUNCTION)              \
    do {                           \
        uint32_t _s;                 \
        STACK_USAGE(FUNCTION,_s); \
        LOG_INFO_("%ld ", _s);         \
    } while (0);
#define END \
    }       \
    LOG_INFO_("\n");
#else 

#define INIT(LABEL) LOG_INFO_(LABEL "\n");
#define RUN(FUNCTION) FUNCTION
#define END
#define S_INIT
#define S_END 

#endif
#endif /* /DEFAULT */

#define MSG_SIZE 5
#define ID_SIZE 5
#define IBE_CLEN MSG_SIZE+(2 * RLC_FP_BYTES + 1)
#define MAX_IBE_CLEN RLC_MD_LEN + l

#if defined BLS || defined ALL
void testBLS(void)
{
    g2_t q; // public key
    g1_t s; // sign
    bn_t k; // private key
    uint8_t msg[MSG_SIZE];

    g1_new(s);
    g2_new(q);
    bn_new(k);
    
    
    S_INIT

        INIT("bls_gen"){
            RUN(cp_bls_gen(k,q));
        }END;
        
        rand_bytes(msg, MSG_SIZE);
        INIT("bls_sign"){
            RUN(cp_bls_sig(s, msg,MSG_SIZE, k));
        }END;

        INIT("bls_ver"){
            RUN(cp_bls_ver(s, msg, MSG_SIZE, q));
        }END;

    S_END
    #ifdef BENCH_SPACE
        LOG_INFO_("STACK: bls_res_size = %d\n", g1_size_bin(s,1));
    #endif

    g2_free(q);
    g1_free(s);
    bn_free(k);
}
#endif

#if defined CLS || defined ALL
void testCLS(void)
{
    g2_t q[2];
    g1_t s[3];
    bn_t k[2];

    uint8_t msg[MSG_SIZE];
    for(int i=0; i<3; i++)
        {g1_new(s[i]);}
    g2_new(q[0]);
    g2_new(q[1]);
    bn_new(k[0]);
    bn_new(k[1]);
    
    S_INIT

        INIT("cls_gen"){
            RUN(cp_cls_gen(k[0],k[1],q[0],q[1]));
        }END;

        rand_bytes(msg, MSG_SIZE);
        INIT("cls_sign"){
            RUN(cp_cls_sig(s[0],s[1],s[2], msg,MSG_SIZE, k[0],k[1]));
        }END;

        INIT("cls_ver"){
            RUN(cp_cls_ver(s[0],s[1],s[2], msg, MSG_SIZE, q[0],q[1]));
        }END;

    S_END
    #ifdef BENCH_SPACE
        LOG_INFO_("STACK: cls_res_size = %d\n", g1_size_bin(s[0],1)*3);
    #endif

    for(int i=0; i<3; i++)
        {g1_free(s[i]);}
    g2_free(q[0]);
    g2_free(q[1]);
    bn_free(k[0]);
    bn_free(k[1]);
}
#endif
#if defined BBS || defined ALL
void testBBS(void)
{
    g2_t q;
    gt_t z;
    bn_t k;
    g1_t s;

    g1_new(s);
    g2_new(q);
    gt_new(z);
    bn_new(k);

    uint8_t msg[MSG_SIZE];
    
    LOG_INFO("\nsize of g1_t: %d size_bin: %d", sizeof(s), g1_size_bin(s,0));
    LOG_INFO("\nsize of g2_t: %d size_bin: %d", sizeof(q), g2_size_bin(q,0));
    LOG_INFO("\nsize of gt_t: %d size_bin: %d", sizeof(z), gt_size_bin(z,0));
    LOG_INFO("\nsize of bn_t: %d size_bin: %d\n", sizeof(k), bn_size_bin(k));
    
    S_INIT
        int res=10;
        INIT("bbs_gen"){
            RUN(cp_bbs_gen(k,q,z));
        }END;
        rand_bytes(msg, MSG_SIZE);
        INIT("bbs_sign"){
            RUN(cp_bbs_sig(s, msg,MSG_SIZE, 0,k));
        }END;
        //LOG_DBG("%s \n",msg);
        //rand_bytes(msg, MSG_SIZE);
        INIT("bbs_ver"){
            RUN(res=cp_bbs_ver(s, msg, MSG_SIZE,0, q,z));
            LOG_DBG("res= %d", res);
        }END;
        //LOG_DBG("%s \n",msg);
    S_END
     #ifdef BENCH_SPACE
        LOG_INFO_("STACK: bbs_res_size = %d\n", g1_size_bin(s,1));
    #endif

    g2_free(q);
    g1_free(s);
    bn_free(k);
    gt_free(z);
}
#endif

#if defined BBS2 || defined ALL
#include "bbs2.c"
void testbbs2(void)
{
    g2_t q;
    gt_t z;
    bn_t k;
    g1_t s;
    
    uint8_t b;
    g1_new(s);
    g2_new(q);
    gt_new(z);
    bn_new(k);

    uint8_t msg[MSG_SIZE];

    int res=10;
    S_INIT

        INIT("bbs2_gen"){
            RUN(bbs2_gen(k,q,z));
        }END;
        rand_bytes(msg, MSG_SIZE);
        INIT("bbs2_sign"){
            RUN(bbs2_sig(s, &b,msg,MSG_SIZE,k)); 
        }END;

        size_t size= g1_size_bin(s,1);
        uint8_t buf[size];
       
        //LOG_DBG("b=%hhx\n",b);
        //g1_print(s);
        //bbs2_sing_write_bin(buf, size, s, b);
        //bbs2_sing_read_bin(s, &b, buf, size);
        //LOG_DBG("b=%hhx\n",b);
        //g1_print(s);
        //LOG_DBG("%s \n",msg);
        //rand_bytes(msg, MSG_SIZE);
        INIT("bbs2_ver"){
            RUN(
                res=bbs2_ver(s,b, msg, MSG_SIZE, q,z));
            LOG_DBG("res= %d", res);
        }END;
    S_END
     #ifdef BENCH_SPACE
        LOG_INFO_("STACK: bbs2_res_size = %d\n", g1_size_bin(s,1));
    #endif

    g2_free(q);
    g1_free(s);
    bn_free(k);
    gt_free(z);
}
#endif
#if defined ZSS || defined ALL
void testZSS(void)
{
    g1_t q0;
    g2_t s;
    gt_t q1;
    bn_t k;
    uint8_t msg[MSG_SIZE];

    g1_new(q0);
    g2_new(s);
    gt_new(q1);
    bn_new(k);
    
    S_INIT

        INIT("zss_gen"){
            RUN(cp_zss_gen(k,q0,q1));
        }END;

        rand_bytes(msg, MSG_SIZE);
        INIT("zss_sign"){
            RUN(cp_zss_sig(s, msg,MSG_SIZE,0, k));
        }END;

        INIT("zss_ver"){
            RUN(cp_zss_ver(s, msg, MSG_SIZE, 0, q0, q1));
        }END;

    S_END
    #ifdef BENCH_SPACE
        LOG_DBG_("STACK: zss_res_size = %d\n", g2_size_bin(s,1));
    #endif

    g2_free(s);
    g1_free(q0);
    gt_free(q1);
    bn_free(k);
}
#endif
#if defined IBE || defined ALL
void testIBE(void){

    g1_t s;
    g2_t q;
    bn_t k;

    char id[ID_SIZE];
    uint8_t msg[MSG_SIZE];
    uint8_t msg_out[MSG_SIZE];
    uint8_t cyp[IBE_CLEN];
    g1_new(s);
    g2_new(q);
    bn_new(k);
    
    size_t cyp_size, clear_size;

    S_INIT

        INIT("ibe_gen"){
            RUN(cp_ibe_gen(k,s));
        }END;
       
        for(int ri=0; ri<ID_SIZE-1;ri++){
            id[ri]=(char) (32+rand()%94);
        }
        id[ID_SIZE-1]='\0';
        //LOG_DBG("id= %s \n", id);
        INIT("ibe_ext"){
            RUN(cp_ibe_gen_prv(q, id, k));
        }END;

        //LOG_DBG("\n size: %lx %lx \n", cyp_size, cyp_size_1);
        rand_bytes(msg, MSG_SIZE);
        cyp_size = IBE_CLEN;
        INIT("ibe_enc")
        {
            RUN(cp_ibe_enc(cyp, &cyp_size, msg, MSG_SIZE, id, s));  
        }END;
        /*
        LOG_DBG("\n size: %d , %d\n", cyp_size, out_size);
        int i;
        for(i=0; i<cyp_size; i++){
            cyp_tmp[i]=cyp[i];
        }*/
        
        clear_size = MSG_SIZE;
        INIT("ibe_dec"){
            /*LOG_DBG("\n %x %x \n cyp: ", cyp, msg_out);
            for(i=0; i<cyp_size; i++){
                LOG_DBG("%hhx ", cyp[i]);
            }
            LOG_DBG("\ncyp_tmp: ");
            for(i=0; i<cyp_size; i++){
                LOG_DBG("%hhx ", cyp_tmp[i]);
            }*/
            
            RUN(cp_ibe_dec(msg_out, &clear_size, cyp, cyp_size, q));
            //LOG_DBG("\n  decrypted: %5s \n", msg_out);
        }END;
        #ifdef BENCH_SPACE
            LOG_INFO_("STACK: ibe_res_size = %d\n", cyp_size-MSG_SIZE);
        #endif
        
    S_END

    g2_free(q);
    g1_free(s);
    bn_free(k);   
    
}
#endif
#if defined GPSW
void testgpsw(){
    #if (!defined GPSW_POLICY_STRING || !defined GPSW_N_ATTR)
    /* #define GPSW_N_ATTR 40
    #define POLICY_STRING \
    "0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31_32_33_34_35_36_37_38_39_40of40" 
    */
    #define GPSW_N_ATTR 10
    #define POLICY_STRING "0_1_2_3_4_5_6_7_8_9_10of10"
     /* #define GPSW_N_ATTR 20
    #define POLICY_STRING "0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15_16_17_18_19_20of20" */
    #else
    #define POLICY_STRING GPSW_POLICY_STRING
    #endif
    LOG_DBG(POLICY_STRING);
    LOG_DBG_("\n");

    gt_t Y;
    g1_t T[GPSW_N_ATTR];
    bn_t y;
    bn_t t[GPSW_N_ATTR];
    gt_t Ep;
    g1_t E[GPSW_N_ATTR];
    
    gt_t m, m_e;

    g2_t D[GPSW_N_ATTR];

    size_t _n=strtok_count(POLICY_STRING, "_")+1;
	gpsw_prv_t prv;
    gpsw_policy_t pol[_n];
    prv.nodes=_n;
    prv.D=D;
    prv.p=pol;
    


    gt_new(Y);
    bn_new(y);
    gt_new(Ep);
    gt_new(m);
    gt_new(m_e);

    for(int i=0;i<GPSW_N_ATTR;i++){
        g1_new(T[i]);
        bn_new(t[i]);
        g1_new(E[i]);
        g2_new(D[i]);
    }

    char msg[MSG_SIZE];
    char chp[100];
    if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("gpsw_start GEN ok\n");
        else
            LOG_WARN("gpsw_start GEN notok\n");      
    //gpsw_prv_new(prv, GPSW_POLICY_STRING);
     int r=0;
    LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());
    S_INIT
        INIT("gpsw_setup"){
            RUN(gpsw_setup(Y, T, y, t, GPSW_N_ATTR));
        }END;
        
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("gpsw_setup GEN ok\n");
        else
            LOG_WARN("gpsw_setup GEN notok\n");             
	
        INIT("gpsw_enc"){
           
            for(int ri=0; ri<MSG_SIZE-1;ri++)
                msg[ri]=(char) (32+rand()%94);
            msg[MSG_SIZE-1]='\0';
              gt_rand(m);
            RUN(
          
            gpsw_enc( Ep, E, Y, T,  m, GPSW_N_ATTR );
            
            //r =gpsw_enc_byte_array(chp, 100, Ep, E, Y, T,  msg, MSG_SIZE, GPSW_N_ATTR);
            );
            //LOG_INFO("message %d %s", r, msg);
        }END;
        
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("gpsw_enc GEN ok\n");
        else
            LOG_WARN("gpsw_enc GEN notok\n");       

        INIT("gpsw_keygen"){
            //gpsw_keygen( priv, pubk, msk, "attr1 attr2 attr3 3of3" );
            RUN(gpsw_keygen( &prv, y, t,GPSW_N_ATTR, POLICY_STRING ));
        }END;
   
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("gpsw_keygen GEN ok\n");
        else
            LOG_WARN("gpsw_keygen GEN notok\n");  
        LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());
        INIT("gpsw_dec"){
            char msg_dec[100];
           
            RUN(
                gpsw_dec(m_e,Y, &prv, Ep, E,GPSW_N_ATTR )
                
                //r=gpsw_dec_byte_array(msg_dec, 100,Y, &prv, Ep, E, chp, r,GPSW_N_ATTR );
                );
             //LOG_INFO("message decrypt %d %s", r ,msg_dec);
        }END;
    
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("gpsw_dec GEN ok\n");
        else
            LOG_WARN("gpsw_dec GEN notok\n");  
   
        if(gt_cmp(m, m_e)!=RLC_EQ)
            LOG_WARN("fail decrypt\n"); 

        LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());

    //util_banner("END",0);
    S_END
    gt_free(Y);
    bn_free(y);
    gt_free(Ep);
    gt_free(m);
    gt_free(m_e);

    for(int i=0;i<GPSW_N_ATTR;i++){
        g1_free(T[i]);
        bn_free(t[i]);
        g1_free(E[i]);
        g2_free(D[i]);
    }
    //gpsw_prv_free(prv);

 }
#endif

#if defined BSW
#include "bsw.h"
void testbsw(){
    #if (!defined BSW_POLICY_STRING || !defined BSW_N_ATTR || !defined BSW_ATTRIBUTES)
    /* #define BSW_N_ATTR 12 // MAXIMUM for SYM
    #define POLICY_STRING "1_18_24_6_29_23_3_27_21_5_14_4_12of12"
    #define BSW_ATTRIBUTES "1_18_24_6_29_23_3_27_21_5_14_4" */
    #define BSW_N_ATTR 40 
    #define POLICY_STRING "0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31_32_33_34_35_36_37_38_39_40of40"
    #define BSW_ATTRIBUTES "0_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31_32_33_34_35_36_37_38_39"
    #else
    #define POLICY_STRING BSW_POLICY_STRING
    #endif
    LOG_DBG(POLICY_STRING);
    LOG_DBG_("\n");

    g2_t h; // PK 
    gt_t A; // PK
    bn_t a, b; // MK

    gt_new(A);
    g2_new(h);
    bn_new(a);
    bn_new(b);

    g1_t Cp[BSW_N_ATTR];
    g1_t Dp[BSW_N_ATTR];
    g2_t C[BSW_N_ATTR];
    g2_t D[BSW_N_ATTR];

    size_t _n=strtok_count(POLICY_STRING, "_")+1;
    bsw_cph_t cph;
    bsw_policy_t pol[_n];
    cph.nodes=_n;
    cph.Cp=Cp;
    cph.C=C;
    cph.policy=pol;
    gt_new(cph.Cs);
    g2_new(cph.c);

    bsw_prv_t prv;
    prv.num_attributes=BSW_N_ATTR;
    prv.Dp=Dp;
    prv.D=D;
    g1_new(prv.d);
    
    
    gt_t m, m_e;
    gt_new(m);
    gt_new(m_e);

    for(int i=0;i<BSW_N_ATTR;i++){
        g1_new(Cp[i]);
        g1_new(Dp[i]);
        g2_new(C[i]);
        g2_new(D[i]);
    }

    char msg[MSG_SIZE];
    //char chp[100];
    if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("start GEN ok\n");
        else
            LOG_WARN("start GEN notok\n");      
    //gpsw_prv_new(prv, GPSW_POLICY_STRING);
     int r=0;
    LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());
    S_INIT
        INIT("bsw_setup"){
            RUN(bsw_setup(h,A,b,a));
        }END;
        
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("setup GEN ok\n");
        else
            LOG_WARN("setup GEN notok\n");             
	
        INIT("bsw_enc"){
           
            /* for(int ri=0; ri<MSG_SIZE-1;ri++)
                msg[ri]=(char) (32+rand()%94);
            msg[MSG_SIZE-1]='\0'; */
            gt_rand(m);
            RUN(
           
            r=bsw_enc(&cph, h, A, m, BSW_N_ATTR, POLICY_STRING);
            
            //r =gpsw_enc_byte_array(chp, 100, Ep, E, Y, T,  msg, MSG_SIZE, GPSW_N_ATTR);
            );
            //LOG_INFO("message %d %s", r, msg);
        }END;
        
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("enc GEN ok\n");
        else
            LOG_WARN("enc GEN notok\n");       

        INIT("bsw_keygen"){

            RUN(bsw_keygen( &prv, b, a, BSW_ATTRIBUTES, BSW_N_ATTR ));
        }END;
   
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("keygen GEN ok\n");
        else
            LOG_WARN("keygen GEN notok\n");  
        LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());
        INIT("bsw_dec"){
            //char msg_dec[100];
           
            RUN(
                r=bsw_dec(m_e,&cph,&prv)
                
                //r=gpsw_dec_byte_array(msg_dec, 100,Y, &prv, Ep, E, chp, r,GPSW_N_ATTR );
                );
             //LOG_INFO("message decrypt %d %s", r ,msg_dec);
        }END;
    
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("dec GEN ok\n");
        else
            LOG_WARN("dec GEN notok\n");  
   
        if(gt_cmp(m, m_e)!=RLC_EQ)
            LOG_WARN("fail decrypt\n"); 

        LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());

    //util_banner("END",0);
    S_END
    gt_free(A);
    g2_free(h);
    bn_free(a);
    bn_free(b);

    gt_free(cph.Cs);
    g2_free(cph.c);
    g1_free(prv.d);

    gt_free(m);
    gt_free(m_e);

    for(int i=0;i<BSW_N_ATTR;i++){
        g1_free(Cp[i]);
        g1_free(Dp[i]);
        g2_free(C[i]);
        g2_free(D[i]);
    }
    //gpsw_prv_free(prv);

 }
#endif

#if defined PYS
#include "pys.h"
void testpys(){
    #if (!defined PYS_POLICY_STRING || !defined PYS_N_ATTR)
    #define PYS_N_ATTR 4
    #define POLICY_STRING "1_-1"
    #else
    #define POLICY_STRING PYS_POLICY_STRING
    #endif
    LOG_DBG(POLICY_STRING);
    LOG_DBG_("\n");

    pys_mk mk;
    pys_mk_new(mk,PYS_N_ATTR);
    pys_pub pub;
    pys_pub_new(pub,PYS_N_ATTR);

    pys_cph cph;
    pys_cph_new(cph,PYS_N_ATTR);

    gt_t m, m_e;
    gt_new(m);
    gt_new(m_e);

    char W [PYS_N_ATTR]={'n', 'n','w','w'};
    char S [PYS_N_ATTR]={'n', 'n','n','n'};

    pys_prv prv;
    pys_prv_new(prv,PYS_N_ATTR);

    if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("start GEN ok\n");
        else
            LOG_WARN("start GEN notok\n");      
    
    LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());
    S_INIT
        INIT("pys_setup"){
            RUN(pys_setup(&mk, &pub));
        }END;
        
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("setup GEN ok\n");
        else
            LOG_WARN("setup GEN notok\n");             
	
        INIT("pys_enc"){
            gt_rand(m);
            LOG_INFO("message generated\n");
            RUN(pys_enc(&cph, m,&pub,W));
        }END;
        
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("enc GEN ok\n");
        else
            LOG_WARN("enc GEN notok\n");       

        INIT("bsw_keygen"){
            RUN(pys_keygen( &prv, &mk, &pub, S, PYS_N_ATTR));
        }END;
   
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("keygen GEN ok\n");
        else
            LOG_WARN("keygen GEN notok\n");  
        LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());
        INIT("bsw_dec"){
            RUN(
                pys_decrypt(m_e,&cph,&prv, &pub));
             //LOG_INFO("message decrypt %d %s", r ,msg_dec);
        }END;
    
        if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("dec GEN ok\n");
        else
            LOG_WARN("dec GEN notok\n");  
   
        if(gt_cmp(m, m_e)!=RLC_EQ)
            LOG_WARN("fail decrypt\n");  

        LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage()); 

    S_END

    pys_pub_free(pub);
    pys_mk_free(mk);
    pys_cph_free(cph);
    pys_prv_free(prv);
    gt_free(m);
    gt_free(m_e);

 }
#endif
#ifdef SIZES
void testSIZES(){
    bn_t bn;
    bn_new(bn);
    pc_get_ord(bn);
    LOG_INFO("size bn: %d\n", sizeof(bn));
    bn_free(bn);

    g1_t g1;
    g1_new(g1);
    g1_get_gen(g1);
    LOG_INFO("size g1: %d\n", sizeof(g1));
    g1_free(g1);

    g2_t g2;
    g2_new(g2);
    g2_get_gen(g2);
    LOG_INFO("size g2: %d\n", sizeof(g2));
    g2_free(g2);

    gt_t gt;
    gt_new(gt);
    gt_get_gen(gt);
    LOG_INFO("size gt: %d\n", sizeof(gt));
    g2_free(g2);

    LOG_INFO("size int: %d\n", sizeof(int));

    LOG_INFO("size size_t: %d\n", sizeof(size_t));

    LOG_INFO("size short int: %d\n", sizeof(short int));

    LOG_INFO("size uint8_t: %d\n", sizeof(uint8_t));

    LOG_INFO("size char: %d\n", sizeof(char));

}
#endif


#ifdef ZH
#include "zh.h"
void testzh(){

    #if (!defined ZH_POLICY_STRING || !defined ZH_N_ATTR || !defined ZH_USER_STRING)
    #define ZH_N_ATTR 20
    #define POLICY_STRING   "ppwwnppwwnppwwnppwwn"
    #define USER_STRING     "pppnnpppnnpppnnpppnn"
    #else
    #define POLICY_STRING ZH_POLICY_STRING
    #define USER_STRING ZH_USER_STRING
    #endif
    LOG_DBG(POLICY_STRING);
    LOG_DBG_("\n");
    
    
    zh_mk mk;
    zh_mk_new(mk);
    zh_pub pub;
    zh_pub_new(pub,ZH_N_ATTR);

    zh_cph cph;
    zh_cph_new(cph,ZH_N_ATTR);

    gt_t m, m_e;
    gt_new(m);
    gt_new(m_e);

    zh_prv prv;
    zh_prv_new(prv,ZH_N_ATTR);

    INIT("zh_setup"){
        RUN(zh_setup(&mk, &pub, ZH_N_ATTR));
    }END;
    if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("setup GEN ok\n");
        else
            LOG_WARN("setup GEN notok\n");  
    //LOG_INFO("m:\n");
    //gt_print(m);
    INIT("zh_enc"){
        RUN(zh_enc(&cph, m, &pub, POLICY_STRING));
    }END;
    //LOG_INFO("m:\n");
    //gt_print(m);
    if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("enc GEN ok\n");
        else
            LOG_WARN("enc GEN notok\n");  
    INIT("zh_keygen"){
        RUN(zh_keygen(&prv, &mk, &pub,USER_STRING));
    }END;
    if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("keygen GEN ok\n");
        else
            LOG_WARN("keygen GEN notok\n");  
        
    INIT("zh_dec"){
        RUN(zh_decrypt(m_e, &cph, &prv, &pub));
    }END;

    if(gt_is_valid(core_get()->gt_g))
            LOG_DBG("dec GEN ok\n");
        else
            LOG_WARN("dec GEN notok\n");  
    //LOG_INFO("m:\n");
    //gt_print(m_e);
    if(gt_cmp(m, m_e)!=RLC_EQ)
        LOG_WARN("fail decrypt\n");  
    //LOG_INFO("gt g:\n");
    //gt_print(core_get()->gt_g);

    zh_pub_free(pub);
    zh_mk_free(mk);
    zh_cph_free(cph);
    zh_prv_free(prv);
    gt_free(m);
    gt_free(m_e);

}

#endif 
/* #define KEYSZ 128
int test_aes(){
    uint8_t key[KEYSZ] = { 1 };
    uint8_t iv[RLC_BC_LEN] = { 0 };
    uint8_t input[5] = {'A','l','i','c','e'};
    uint8_t output[KEYSZ];
    int output_sz = KEYSZ;zh
    
    if (bc_aes_cbc_enc(output, &output_sz, input, 5, key, KEYSZ, iv) == RLC_OK)
        LOG_INFO("1.Success enc\n");
    else
        LOG_INFO("1.Error enc\n");
    
    LOG_INFO("2.Output size in enc is %d\n", output_sz);
    
    LOG_INFO("3.Input: ");
    for (int i = 0; i < 5; i++) {
        LOG_INFO_("%c", input[i]);
    }
    LOG_INFO_("\n");
    
    uint8_t dec[KEYSZ];
    memset(dec, 0, KEYSZ);
    LOG_INFO("\n");
    
    int dec_sz = KEYSZ;
    if (bc_aes_cbc_dec(dec, &dec_sz, output, output_sz, key, KEYSZ, iv) == RLC_OK)
        LOG_INFO("4.Success dec\n");
    else
        LOG_INFO("4.Error dec\n");
    
    LOG_INFO("5.Output size in dec is %d\n", dec_sz);
    
    LOG_INFO("6.Output: ");
    for (int i = 0; i < dec_sz; i++) {
        LOG_INFO_("%c", dec[i]);
    }
    LOG_INFO_("\n");
} */
int test(void)
{

    //uint8_t* p = NULL;
    //size_t size = 0;

    //while((p = malloc(sizeof(uint8_t))) != NULL)
    //    size++;
    //LOG_DBG("size %d\n", size);
    //*a=100;
    /*p =malloc(100);
    if(p!=NULL)
        LOG_DBG("heap size= %ls\n", p);
    else
        LOG_DBG("no mem\n");*/
    LOG_DBG("STACK_SIZE: %ld\n", stack_check_get_reserved_size());
    LOG_DBG("RLC_BN_BITS: %d\n", RLC_BN_BITS);
    LOG_DBG("RLC_DIG: %d\n", RLC_DIG);
    LOG_DBG("RLC_BN_DIGS: %d\n", RLC_BN_DIGS);
    LOG_DBG("RLC_BN_SIZE: %d\n", RLC_BN_SIZE);
    LOG_DBG("BN_MAGNI: %d\n", BN_MAGNI);
    if (core_init() != RLC_OK) {
        LOG_ERR("core not ok\n");
        core_clean();
        return 1;
    }
    LOG_DBG("bn_copy %d digits\n", core_get()->ep_r.used);
    /* LOG_ERR("log err %d\n", 0);
    LOG_WARN("log warn %d\n", 0);
    LOG_INFO("log info %d\n", 0);
    LOG_DBG("log dbg %d\n", 0); */

    //util_banner("Benchmarks for the PC module:", 0);
    LOG_INFO("\nBenchmarks for the PC module:\n");
    if (pc_param_set_any() != RLC_OK) {
        LOG_ERR("params not ok\n");
        RLC_THROW(ERR_NO_CURVE);
        core_clean();
        return 1;
    }
    LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());
    LOG_DBG("bn_copy %d digits\n", core_get()->ep_r.used);
    util_print("security level: %d\n", ep_param_level());
    pc_param_print();
    
    // util_banner("Sizeof Types:", 0);
    // testSize();
	// util_banner("Test rand:", 0);
	// testRand();
	// return 1;

    // util_banner("Test Stack Usage:", 0);
    // testSU(10);

    util_banner("test started:", 0);

    #ifdef SIZES
        testSIZES();
    #endif
    #if defined BLS || defined ALL
        testBLS();
    #endif
    #if defined BBS || defined ALL
        testBBS();
    #endif
    #if defined BBS2 || defined ALL
        testbbs2();
    #endif
    #if defined CLS || defined ALL
        testCLS();
    #endif
    #if defined ZSS || defined ALL
        testZSS();
    #endif
    #if defined IBE || defined ALL
        testIBE();
    #endif
    #if defined PYS
        testpys();
    #endif 
    #if defined ZH
        testzh();
    #endif 

   
    #if defined GPSW
        testgpsw();
    #endif
    #if defined BSW
        testbsw();
       /*  bn_t n;
	    bn_new(n);
	    pc_get_ord(n);
        g1_t h1,h2;
        g1_new(h1);
        g1_new(h2);
        g1_rand(h1);
        bn_t a;
        bn_new(a);
        g1_get_gen(h1);
        bn_set_dig(a,1);
        bn_mod_inv(a,a,n);
        bn_mul_dig(a,a,abs(-2));
        bn_neg(a,a);
        bn_mod(a,a,n);
        log("\n");
	    log("a1= ");
	    bn_print(a);
	    log("\n");
        g1_mul(h2,h1,a);

        g1_mul_dig(h1,h1,abs(-2));
        g1_neg(h1,h1);
        bn_set_dig(a,1);
        bn_mod_inv(a,a,n);
         log("\n");
	    log("a2= ");
	    bn_print(a);
	    log("\n");
        g1_mul(h1,h1,a);
        //g1_map(h1,(uint8_t*)"A",1);
        
        if(g1_cmp(h1,h2)==RLC_EQ)
            LOG_INFO("eq\n");
        else
            LOG_INFO("div\n");
        g1_free(h1);
        g1_free(h2);
        bn_free(a); */
    #endif
    //test_aes();
    LOG_DBG("STACK_USAGE: %ld\n", stack_check_get_usage());
    util_banner("END", 0);

    core_clean();
    return 0;
}
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
    PROCESS_BEGIN();

    assert(int_master_is_enabled());
    int_master_read_and_disable();
    test();
    PROCESS_END();
}