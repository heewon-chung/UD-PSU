"""
@brief  evaluate communication costs
"""
from math import ceil, log

def eval_comm_cost(p_len, kappa, eta, m, ell, delta, n):
    #1. preparation
    Y_i_len, B_i_len, L_i_len = p_len * n * ell, eta * ell, p_len * ell
    total_bits_for_prep = Y_i_len + B_i_len + L_i_len
    
    #2. computation
    #2.1 Query
    total_bits_for_query = 2 * kappa
    total_bits_for_query += p_len * n * ell
    
    #2.2 Consent
    total_bits_for_conset  = kappa  # assume |ID|=kappa
    
    #2.3 Delegate
    total_bits_for_delegate = (2 * p_len + 2) * n * ell + kappa + (2 * p_len) * ell
    
    #2.4 Compute
    total_bits_for_compute = (2 * p_len + 3) * int(ceil(log(m, 2))) * n * ell
    
    return (total_bits_for_prep+
            total_bits_for_query+
            total_bits_for_delegate+
            total_bits_for_compute) 

def main():
    p_len = 40
    kappa, eta = 128, 128    # 80-bit security
    m = [3, 5, 10] 
    ell = 2**6               # fix the number of bins
    delta = [2**10, 2**11, 2**12, 2**13, 2**14]
    for i in range(len(m)):
        for j in range(len(delta)):
            print("======<parameters>=======")
            d = delta[j] / ell
            n = 2 * m[i] * d + 1
            print("no. of clients=", m[i], " and no. of elt.s=", delta[j])
            number_of_bits = eval_comm_cost(p_len, kappa, eta, m[i], ell, delta[j], n)
            print("Total transmission amount=", format(number_of_bits/(8*10**6), ".1f"), "(MB)", sep="")
            print("\n")
    return None

if ("__main__" == __name__):
    main()