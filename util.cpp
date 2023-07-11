#include "util.h"

using namespace NTL;

void mpz_to_zz(const mpz_t& origin, ZZ& dest){
    std::stringstream temp;
    temp << origin;

    dest = conv<ZZ>(temp.str().c_str());
}

void mpz_to_zz(const mpz_t& origin, ZZ_p& dest){
    std::stringstream temp;
    temp << origin;

    dest = conv<ZZ_p>(temp.str().c_str());
}


void zz_to_mpz(const ZZ& origin, mpz_t& dest){
    std::stringstream temp;
    temp << origin;

    mpz_set_str(dest, temp.str().c_str(), 10);
}

void zz_to_mpz(const ZZ_p& origin, mpz_t& dest){
    std::stringstream temp;
    temp << origin;

    mpz_set_str(dest, temp.str().c_str(), 10);
}


void mpz_to_zzpx(mpz_t* origin, ZZ_pX& dest, int degree)
{
// #pragma omp parallel for    
    for(int i = 0; i < degree; i++)
    {
        std::stringstream temp;
        temp << origin[i];
        
        ZZ_p coeff = conv<ZZ_p>(temp.str().c_str());
        SetCoeff(dest, i, coeff);
    }
}


void zzpx_to_mpz(ZZ_pX& origin, mpz_t* dest)
{
    int degree = deg(origin);
// #pragma omp parallel for
    for(int i = 0; i <= degree; i++)
    {
        // ZZ_p coeff = coeff(origin, (long) i);
        ZZ_p coeff = origin[i];
        std::stringstream temp;
        temp << coeff;

        mpz_set_str(dest[i], temp.str().c_str(), 10);

    }
}


void rational_to_RLS(ZZ_pX& numerator, ZZ_pX& denominator, ZZ_pX& rns)
{    
    ZZ_pX temp = numerator;
    temp <<= (deg(denominator) * 2);

    ZZ_pX rem;
    div(rns, temp, denominator);
}



void RLS_to_rational(ZZ_pX& rns, ZZ_pX& numerator, ZZ_pX& denominator, int degree, int number_of_clients)
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
    
    long rem_degree = number_of_clients * degree;

    while(deg(remainder) >= rem_degree)
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