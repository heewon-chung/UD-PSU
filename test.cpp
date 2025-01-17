
/*

- a test that runs both update and PSI computation in Feather protocol.

*/
//*********************************************************************
#include <iostream>
#include <vector>
#include <random>

// #include "NTL/ZZ_pX.h"

#include "util.h"
#include "Client.h"



//*********************************************************************
// - Function description: generates a set of random bigintegers,
// and ensures that the values are smaller than the public moduli and unequal to x-coordinates.
bigint* gen_randSet (int size, 				/// c
					 int max_bitsize, 		/// bit length of elements
					 bigint* pubModuli, 
					 bigint* x_points, 		/// \vec{x}
					 int xpoint_size)		/// n
{
	int counter = 0;
	Random rd;
	mpz_t *pr_val;
	pr_val = (mpz_t*)malloc(size * sizeof(mpz_t));
	unordered_map <string, int> map;
	string s_val;
	int max_bytesize = max_bitsize;
	gmp_randstate_t rand;
	bigint ran;
	rd.init_rand3(rand, ran, max_bytesize);
	bigint temp;
	mpz_init(temp);
	bool duplicated = false;

	for (int i = 0; i < size; i++) {
		mpz_urandomb(temp, rand, max_bitsize);
		mpz_mod(temp, temp, pubModuli[0]);
		/*
		for(int k=0;k<counter; k++){
			if(mpz_cmp(pr_val[k],temp)==0)
			duplicated=true;
		}
		*/
		while (mpz_cmp(temp, pubModuli[0]) > 0 || duplicated == true) { // ensures the elements are smaller than the public moduli.
			mpz_init(temp);
			mpz_urandomb(temp, rand, max_bitsize);
			//extra checks-- ensures they are distinc.
			/*
			for(int k=0;k<counter; k++){
			if(mpz_cmp(pr_val[k],temp)==0){
			duplicated=true;break;
			}
			else{duplicated=false;
			}
			}
			*/
			for (int j = 0; j < xpoint_size; j++) { //checks the random element is not equal to any x_points.
				if (mpz_cmp(temp, x_points[j]) == 0) {
					mpz_init(temp);
					mpz_urandomb(temp, rand, max_bitsize);
					for (int k = 0; k < counter; k++){
						if (mpz_cmp(pr_val[k], temp) == 0) {
							duplicated = true; 
							break;
						}
						else {
							duplicated = false;
						}
					}
				}
			}
		}
		mpz_init_set(pr_val[i], temp);
		counter++;		/// 현재까지 만든 good elements의 수
		//s_val.clear();
		//s_val = mpz_get_str(NULL, 10, temp);
		//map.insert(make_pair(s_val, 1));
	}
	return pr_val;
}

std::vector<unsigned int> 
_genRandSet(const int size, const std::vector<unsigned int> I)
{
	std:vector<unsigned int> rand_set;

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<unsigned int> uniform(1, 99);

	for (int i = 0; i < size - I.size(); i++) {
		rand_set.push_back(uniform(generator));
	}
	for (std::vector<unsigned int>::const_iterator itr = I.begin(); itr != I.end(); itr++) {
		rand_set.push_back(*itr);
	}

	return rand_set;
}


// #define __UPDATE
#define PSU

int main()
{

	int pub_mod_bitsize = 40;
	int max_setsize = 4096;
	int table_length = 20;  				/// h
	int bucket_max_load = 310;				/// d
	int interSec_size = 1;

	if (interSec_size > max_setsize) {
		cout << "interSec_size > max_setsize" << endl; 
		return 0;
	}
	int number_of_experiments = 3;
	int number_of_clients = 2;				/// m+1

	// int xsize = 2 * bucket_max_load + 1;  						/// n
	int xsize = 2 * number_of_clients * bucket_max_load + 1;  						/// n
	// if (xsize < (2 * number_of_clients * bucket_max_load) + 1) {
	// 	cout << "\nxsize must be greater than 2*bucket_max_load)+1, reset it\n";
	// 	return 0;
	// }
	double temp_req = 0;
	double temp_grant = 0;
	double temp_res = 0;
	double temp_intersect = 0;
	double temp_out = 0;
	double sum = 0;
	double start_out_1 = 0;
	double end_out_1 = 0;
	double sum_1 = 0;
	double diff_b = 0;

	for (int l = 0; l < number_of_experiments; l++) {
		Server serv(xsize, 					// n
				    number_of_clients, 		// m+1
					pub_mod_bitsize, 
					max_setsize, 			// c
					bucket_max_load, 		// d
					table_length);			// h
		Server* serv_ptr(& serv);
		int elem_bit_size = 100;
		// int elem_bit_size = 10;
		bigint* pub_mod = serv.get_pubModuli();
		ZZ modulus;
		mpz_to_zz(pub_mod[0], modulus);
		ZZ_p::init(modulus);

		// Assigning random values to two sets a and b.
		cout << "\n--------------------  Parameter --------------------------------\n";
		cout << "\t c :                " << max_setsize << endl;
		cout << "\t Pub_mod_bitsize :  " << pub_mod_bitsize << endl;
		cout << "\t d :                " << bucket_max_load << endl;
		cout << "\t h :                " << table_length << endl;
		cout << "\t x size :     	   " << xsize << endl;
		cout << "\t Modulus :          " << pub_mod[0] << endl;
		cout << "\t m+1 :              " << number_of_clients << endl;
		cout << "\n----------------------------------------------------------------\n";
		int t1, t2;
		mpz_t *aa, *bb, **a;



		a = (mpz_t**)malloc((number_of_clients - 1) * sizeof(mpz_t));	// A_i의 set에 대한 pointer
		bb = (mpz_t*)malloc(max_setsize * sizeof(mpz_t));				// set S_0
		cout << "\n** Generating two random sets with distinct values and unequal to x_points" << endl;
		bb = gen_randSet (max_setsize, 					/// c
						  elem_bit_size, 
						  serv.get_pubModuli(), 
						  serv.get_xpoints(t2), 		/// t2 = n
						  xsize);						/// n
		for (int i = 0; i < number_of_clients - 1; i++) { // A_i의 set S_i 생성
			a[i] = (mpz_t*)malloc(max_setsize * sizeof(mpz_t));
			a[i] = gen_randSet (max_setsize, 			/// c
								elem_bit_size, 
								serv.get_pubModuli(), 
								serv.get_xpoints(t1), 
								xsize);					/// n
		}
		bigint *x_p = serv.get_xpoints(t1);
		for(int j = 0; j < number_of_clients - 1; j++) {
			for(int i = 0; i < interSec_size; i++) {
				mpz_set(a[j][i], bb[i]);				/// S_0의 처음 interSec_size 만큼 S_i에 모두 copy
			}
		}
#ifdef __DEBUG
		std::cout << "A_0's set" << std::endl;
		for (int i = 0; i < max_setsize; i++) {
			std::cout << bb[i] << ", ";
		}
		std::cout << std::endl;
#endif

#ifdef __DEBUG
		for (int i = 0; i < number_of_clients - 1; i++) {
			std::cout << "A_" << i+1 << "'s set" << std::endl;
			for (int j = 0; j < max_setsize; j++) {
				std::cout << a[i][j] << ", ";
			}
			std::cout << std::endl;
		}
#endif 
		//define the authorizers: A_i
		Client **A_;
		string A_IDs[number_of_clients - 1];

		A_ = new Client* [number_of_clients - 1];
		for (int j = 0; j < number_of_clients - 1; j++) {
			A_[j] = new Client(serv_ptr, a[j], max_setsize);
			A_IDs[j] = "A_ID: " + to_string(j);
		}

		/// A_0 client
		Client B(serv_ptr, bb, max_setsize);
		string b_id = "B_ID";
		bigint label;
		cout << "** Client A_0 outsourcing" << endl;
		double start_out_b = clock();
		B.outsource_db(b_id);
		double end_out_b = clock();
		diff_b += end_out_b - start_out_b;
		free(bb);

		cout << "** Senders (A_i) are outsourcing" << endl;
		for (int j = 0; j < number_of_clients - 1; j++) {
			cout << "  - client " << A_IDs[j] << " is outsourcing" << endl;
			A_[j]->outsource_db(A_IDs[j]);
		}
		for(int j = 0; j < number_of_clients -1; j++){
			for(int i = 0; i < max_setsize; i++) {
				mpz_clear(a[j][i]);
			}
			free(a[j]);
		}
		free (a); 

		Random rd_;
		bigint* labels;
		Random rd_1;
		bigint *temp_9;
		int size_;
		
		//-------Update----------
#ifdef __UPDATE	
		bigint *temp;
		temp = (mpz_t*)malloc(1 * sizeof(mpz_t));
		temp= gen_randSet (1, elem_bit_size,serv.get_pubModuli(), serv.get_xpoints(t1), xsize);
		// cout << "\n inserting:" << temp[0] << endl;
		double start_update = clock();
		cout << "** Updates on Outsourced Data" << endl;
		string ss1 = B.update(temp[0], "insertion", label, "B_ID");
		double end_update = clock();
		double update_time = end_update - start_update;
		cout << "Previous Update Time: " << update_time << endl;

		temp= gen_randSet (1, elem_bit_size,serv.get_pubModuli(), serv.get_xpoints(t1), xsize);
		double start_new_update = clock();
		string ss2 = B.new_update(temp[0], "insertion", label, "B_ID");
		double end_new_update = clock();
		double new_update_time = end_new_update - start_new_update;
		cout << "new Update Time: " << new_update_time << endl;
		
		cout << "Ratio: " << update_time / new_update_time << endl;
 		// string sss = B.update(temp[0], "deletion", label, "B_ID");
 		// cout << ss1 << endl;
 		// cout << ss2 << endl;
 		// cout << sss << endl;
#endif	

		//-----------Set Intersection------------
		bigint **q;
		int* sz;
		cout << "** Generate the Computation Request" << endl;
		byte B_tk[AES::DEFAULT_KEYLENGTH] = {0};		/// 16 bytes
		byte B_tIV [AES::BLOCKSIZE] = {0};				/// 16 bytes

		// memset(B_tk, 0x00, (AES::DEFAULT_KEYLENGTH) + 1);
		// memset(B_tIV, 0x00, (AES::BLOCKSIZE) + 1);
		double start_req = clock();
		CompPerm_Request* req = B.gen_compPerm_req(B_tk, B_tIV);		/// A_0가 trapdoor 생성 
		double end_req = clock();
		temp_req += end_req - start_req;
		cout << "** Grant the Computation Done" << endl;
		GrantComp_Info** ptr;
		ptr = new GrantComp_Info*[number_of_clients - 1];
		bigint ***Q;
		Q = (mpz_t***)malloc((number_of_clients - 1) * sizeof(mpz_t));
		double start_grant;
		double end_grant;
		for(int j = 0; j < number_of_clients - 1; j++){
			if(j == 0){start_grant = clock();
			}
			ptr[j] = A_[j]->grant_comp(req, Q[j], true);
			if(j == 0){end_grant = clock();
			}
		}
		for(int j = 0; j < number_of_clients - 1; j++){
			A_[j]->free_client();
			delete A_[j];
		}
		temp_grant += end_grant - start_grant;


		cout << "** Server-side Result Computation." << endl;
		double start_res=clock();
		Server_Result* res = serv.compute_result(ptr, B_tk, B_tIV);
		double end_res = clock();
		temp_res += end_res - start_res;
		//-----Just to free some memory----
		cout << "\n cleaning the server" << endl;
		serv.free_server();

		cout << "\n---- Client-side Result Retrieval" << endl;
		double start_intersect = clock();
#ifdef PSU
		vector<string>  final_res = B.find_union(res, sz , Q, number_of_clients, bucket_max_load);
#else
		vector<string>  final_res = B.find_intersection(res, sz , Q, number_of_clients);
#endif
		double end_intersect = clock();
		temp_intersect += end_intersect - start_intersect;
		// cout << "======= Result =======" << endl;
		// for (int i = 0; i < final_res.size(); i++) {
		// 	cout << "* Final_res " << i + 1 << ": " << final_res[i] << endl;
		// }
	}
	// cout << "====================" << endl;
	cout << "\n============= Run time ===================" << endl;
	double out = diff_b  /number_of_experiments;
	float out_time = out / (double) CLOCKS_PER_SEC;
	cout << "* Outsourcing time:\t" << out_time << endl;
	// double com_req = temp_req / number_of_experiments;
	// float req_time = com_req / (double) CLOCKS_PER_SEC;
	// cout << "* Computation Request time:\t" << req_time << endl;
	double grant = temp_grant / number_of_experiments;
	float grant_time = grant / (double) CLOCKS_PER_SEC;
	cout << "* Computation Grant time:\t" << grant_time << endl;
	double res_= temp_res / number_of_experiments;
	float res_time = res_ / (double) CLOCKS_PER_SEC;
	cout << "* Server Computation time:\t" << res_time << endl;
	double inter = temp_intersect / number_of_experiments;
	float inter_time = inter / (double) CLOCKS_PER_SEC;
#ifdef PSU
	cout << "* Find union time:\t" << inter_time << endl;
#else
	cout << "* Find intersection time:\t" << inter_time << endl;
#endif
	cout << "============================================" << endl;

	// -----------End of Set intersection------------

	return 0;

}
//**********************************************************************
