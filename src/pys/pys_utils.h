#ifndef PYS_UTILS_H
#define PYS_UTILS_H
#include <relic.h>

/**
 * @brief compute lambda(i) of vector J
 * 
 * @param r result
 * @param J the vector
 * @param i index of lambda
 */
void lambda(bn_t r, short int* J, int J_len, int i);

/**
 * @brief compute t = sum(lambda(i)) of vector J
 * 
 * @param r result
 * @param J the vector
 * @param i index of lambda
 */
void t_lambda(bn_t r,short int* J, int J_len);








#endif // PYS_UTILS_H