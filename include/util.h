#ifndef _UTIL_
#define _UTIL_
#include <sstream>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <gmp.h>
#include <gmpxx.h>

#include <omp.h>

#include "Rand.h"


using namespace NTL;

void mpz_to_zz(const mpz_t& origin, ZZ& dest);
void mpz_to_zz(const mpz_t& origin, ZZ_p& dest);

void zz_to_mpz(const ZZ& origin, mpz_t& dest);
void zz_to_mpz(const ZZ_p& origin, mpz_t& dest);

void mpz_to_zzpx(bigint* origin, ZZ_pX& dest, int degree);
void zzpx_to_mpz(ZZ_pX& origin, bigint* dest);

void rational_to_RLS(ZZ_pX& numerator, ZZ_pX& denominator, ZZ_pX& rns);
void RLS_to_rational(ZZ_pX& rns, ZZ_pX& numerator, ZZ_pX& denominator, int degree, int number_of_clients);

#endif