#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <complex>
#include <sstream>

#define MKL_Complex16 std::complex<double>

#include <pthread.h>
#include "mkl.h"


int N; // A i B su NxN matrice, P = broj radnih dretvi
int P;
std::vector<MKL_Complex16> A, B, C;


void *mult_part(void *threadId){
    long long tid;
    int i, j, a1, a2, b1, b2, size, int1 = 1, start, end, tend;
    tid = (long long)threadId;
    int num_per_proc = (N*N*N*N + P - 1) / P;
    start = tid * num_per_proc;
    if(N*N*N*N < start){
        start = N*N*N*N;
    }
    end = (tid + 1) * num_per_proc;
    if(N*N*N*N < end){
        end = N*N*N*N;
    }

    while(start/N <= end/N && start != end){ // tend i ovo da ne izadjemo out of bounds na B
        tend = (start/N + 1)*N;
        if(tend > end){
            tend = end;
        }
        size = tend - start;

        i = start % (N*N); // C[start] = C[i,j] = C[j*N*N + i]
        j = start / (N*N);
        a1 = i / N; // C[i,j] = A[i/N, j/N] * B[i%N, j%N]
        a2 = j / N;
        b1 = i % N;
        b2 = j % N;
        
        //printf("tid %d start %d tend %d end %d A(%d, %d), B(%d, %d)\n", tid, start, tend, end, a1, a2, b1, b2);


        zaxpy(&size, &(A[a2*N + a1]), &(B[b2*N + b1]), &int1, &(C[start]), &int1);
        start = tend;
    }

    pthread_exit(NULL);
    return NULL;
}



int main(void){
    
    std::string F, G, H; // A u datoteci F, B u datoteci G, C u datoteci 
    int i, j, rc;
    //void *status;
    std::ifstream if1, if2;
    std::ofstream out;


    std::cin >> N >> P >> F >> G >> H;

    std::vector<pthread_t> threads(P);

    A.resize(N*N);
    B.resize(N*N);
    C.resize(N*N*N*N);
    

    if1.open(F);
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            if1 >> A[j*N + i];
        }
    }
    if1.close();

    if2.open(G);
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            if2 >> B[j*N + i];
        }
    }
    if2.close();


    // racun
    for(i = 0; i < P; ++i){
        rc = pthread_create(&threads[i], NULL, mult_part, (void*)i);
        if(rc){
            std::cout << "Error in pthread_create()! Return code " << rc << std::endl;
            exit(-1);
        }
    }

    for(i = 0; i < P; ++i){
        rc = pthread_join(threads[i], NULL);
        if(rc){
            std::cout << "Error in pthread_join()! Return code " << rc << std::endl;
            exit(-1);
        }
    }

    // output
    out.open(H);
    for(i = 0; i < N*N; i++){
        for(j = 0; j < N*N; j++){
            out << C[j*N*N + i] << " ";
        }
        out << "\n";
    }


    return 0;
}