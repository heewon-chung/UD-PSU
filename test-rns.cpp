#include <vector>
#include <sstream>

#include <NTL/ZZ.h>
#include <NTL/ZZX.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vec_ZZ_p.h>
#include <gmp.h>
#include <gmpxx.h>

using namespace NTL;

void zzpx_to_mpz(ZZ_pX& origin, mpz_t* dest)
{
    int degree = deg(origin);
#pragma omp parallel for
    for(int i = 0; i <= degree; i++)
    {
        ZZ_p coeff = origin[i];
        std::stringstream temp;
        temp << coeff;

        mpz_set_str(dest[i], temp.str().c_str(), 10);
    }
    std::cout << std::endl;
}

void RationalToRLS(ZZ_pX& numerator, ZZ_pX& denominator, ZZ_pX& rns)
{    
    ZZ_pX temp = numerator;
    temp <<= (deg(denominator) * 4);

    ZZ_pX rem;
    div(rns, temp, denominator);
}


void RLSToRational(ZZ_pX& rns, ZZ_pX& numerator, ZZ_pX& denominator, int degree, int number_of_clients)
{
    ZZ_pX quotient, remainder, temp1, temp2;
    SetX(temp1);
    int num_shift = degree * 2 * number_of_clients;

    temp1 <<= (num_shift - 1);
    temp2 = rns;

    ZZ_pX   prev_num, curr_num, next_num, 
            prev_denom, curr_denom, next_denom;

    SetCoeff(prev_num, 0, 1);
    SetCoeff(curr_denom, 0, 1);
    
    DivRem(quotient, remainder, temp1, temp2); // temp1 = quotient * temp2 + remainder
    next_num = prev_num - curr_num * quotient;
    next_denom = prev_denom - curr_denom * quotient;

    prev_num = curr_num; prev_denom = curr_denom;
    curr_num = next_num; curr_denom = next_denom;

    temp1 = temp2;
    temp2 = remainder;

    while(deg(remainder) >= (long) number_of_clients * degree)
    {
        DivRem(quotient, remainder, temp1, temp2); // temp1 = quotient * temp2 + remainder
        next_num = prev_num - curr_num * quotient;
        next_denom = prev_denom - curr_denom * quotient;

        prev_num = curr_num; prev_denom = curr_denom;
        curr_num = next_num; curr_denom = next_denom;

        temp1 = temp2;
        temp2 = remainder;
    }

    MakeMonic(next_num);
    MakeMonic(next_denom);

    numerator = next_num;
    denominator = next_denom;
}

int main(void)
{
    ZZ modulus = GenPrime_ZZ(10);
    std::cout << "modulus: " << modulus << std::endl;
    int degree = 5;

    ZZ_p::init(modulus);
    ZZ_pX poly1, poly2, poly3, poly4, rns1, rns2, rns, numerator, denominator;
    mpz_t* gmp_poly;
    gmp_poly = (mpz_t*)malloc((degree + 1) * sizeof(mpz_t));

    vec_ZZ_p roots1, roots2;

    random(poly2, degree - 1);
    SetCoeff(poly2, degree - 1, 1);
    random(poly4, degree - 1);
    SetCoeff(poly4, degree - 1, 1);

    zzpx_to_mpz(poly2, gmp_poly);

    for(int i = 0; i < degree; i++)
    {
        roots1.append(random_ZZ_p());
        roots2.append(random_ZZ_p());
    }
    BuildFromRoots(poly1, roots1);
    BuildFromRoots(poly3, roots2);

    RationalToRLS(poly2, poly1, rns1);
    RationalToRLS(poly4, poly3, rns2);

    RLSToRational(rns1, numerator, denominator, degree, 1);

    rns = rns1 + rns2;

    RLSToRational(rns, numerator, denominator, degree, 2);

    std::cout << "Poly2: " << poly2 << std::endl;
    std::cout << "Poly1: " << poly1 << std::endl;
    std::cout << "Poly4: " << poly4 << std::endl;
    std::cout << "Poly3: " << poly3 << std::endl;

    std::cout << "==========" << std::endl;
    std::cout << "RNS1: " << rns1 << std::endl;
    std::cout << "RNS2: " << rns2 << std::endl;
    std::cout << "degree of RNS2: " << deg(rns2) << std::endl;
    std::cout << "==========" << std::endl;
    std::cout << "numerator: " << numerator << std::endl;
    std::cout << "denominator: " << denominator << std::endl;
    std::cout << "==========" << std::endl;
    for(int i = 0; i < degree; i++)
    {
        std::cout << eval(denominator, roots1[i]) << ", ";
        // std::cout << eval(rns1, roots1[i]) << ", ";
    }
    std::cout << std::endl;
    for(int i = 0; i < degree; i++)
    {
        std::cout << eval(denominator, roots2[i]) << ", ";
    }
    std::cout << std::endl;


    return 0;
}