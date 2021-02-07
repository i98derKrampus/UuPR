/*
    argv[1] = broj elemenata u nizu
    argv[2] = broj parcijalnih suma koje je potrebno izracunati
    argv[3] = broj radnih dretvi
    argv[4] = ime datoteke koja sadrzi niz
    argv[5] = ime datoteke u koju spremamo parcijalne sume
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>

#include <pthread.h>


int N, P, PP, K; // N = broj pocetnih parcijalnih suma, P = broj radnih dretvi
// K = gornje cijelo od log_2{N}, PP = 2^K
std::vector<double> A;
std::vector<double> scan;
std::vector<double> X, Y;
std::vector<pthread_t> threads;
pthread_barrier_t barrier1, barrier2;
std::vector<pthread_barrier_t> barriers, barriers2;


void *block_scan(void *threadId){
    long long tid;
    int i, j, m, step, start, end;
    tid = (long long)threadId;
    int num_per_proc = (N + P - 1) / P;
    start = tid * num_per_proc;
    if(N < start){
        start = N;
    }
    end = (tid + 1) * num_per_proc;
    if(N < end){
        end = N;
    }

    // obrada grupe [start, end>
    for(i = start; i < end; i++){
        if(i == start){
            scan[i] = A[i];
        } else{
            scan[i] = A[i] + scan[i-1];
        }
    }
    if(start < end){
        X[tid] = scan[end-1];
    }

    pthread_barrier_wait(&barrier1);
    
    m = PP/2;
    step = 1;
    if(tid < m){
        Y[2*tid+1] = X[2*tid] + X[2*tid+1];
    }

    pthread_barrier_wait(&barriers[0]);

    for(j = 2; j <= K; j++){
        m /= 2;
        step *= 2;
        for(i = 1; i <= m; i++){
            if(tid == i*step - 1){
                Y[2*tid + 1] = Y[2*tid + 1 - step] + Y[2*tid + 1];
            }
        }
        pthread_barrier_wait(&barriers[j-1]);
    }

    
    for(j = 0; j < K-2; j++){
        m <<= 1;
        step /= 2;
        for(i = 1; i <= m-1; i++){
            if(tid == (i+1)*step - 1){
                Y[2*tid + 1 - step] = Y[2*tid + 1 - 2*step] + Y[2*tid + 1 - step];
            }
        }
        pthread_barrier_wait(&(barriers2[j]));
    }

    m = 2*m;
    if(tid == 0){
        Y[0] = X[0];
    } else{
        for(i = 1; i <= m-1; i++){
            if(tid == i){
                Y[2*i] = Y[2*i-1] + X[2*i];
            }
        }
    }

    pthread_barrier_wait(&barrier2); // Y gotov

    if(tid > 0){
        for(i = start; i < end; i++){
            scan[i] += Y[tid-1];
        }
    }

    pthread_exit(NULL);
    return NULL;
}



int main(int argc, char *argv[]){
    if(argc != 6){
        std::cout << "Krivi broj argumenata komandne linije";
        return 0;
    }

    int M; // (ne koristi se) duljina niza A u datoteci (nama treba samo prvih N elemenata)
    std::string F, G; // A u datoteci F, rezultat u G 
    int i, j, k = 0, m, rc;
    //void *status;
    std::ifstream if1;
    std::ofstream out;

    M = atoi(argv[1]);
    N = atoi(argv[2]);
    P = atoi(argv[3]);
    F = std::string(argv[4]);
    G = std::string(argv[5]);

    A.resize(N);
    scan.resize(N);
    j = P-1;
    PP = 1;
    for(; j > 0; j >>= 1){
        PP <<= 1;
        ++k;
    }
    K = k;
    X.resize(PP);
    Y.resize(PP);
    threads.resize(P);
    barriers.resize(K);
    if(K-2 > 0){
        barriers2.resize(K-2);
    }

    if1.open(F);
    for(i = 0; i < N; i++){
        if1 >> A[i];
    }
    if1.close();

    
    // initializing barriers
    pthread_barrier_init(&barrier1, NULL, P);
    for(i = 0; i < K; i++){
        pthread_barrier_init(&(barriers[i]), NULL, P);
    }
    pthread_barrier_init(&barrier2, NULL, P);
    for(i = 0; i < K-2; i++){
        pthread_barrier_init(&(barriers2[i]), NULL, P);
    }

    // racun
    for(i = 0; i < P; ++i){
        rc = pthread_create(&threads[i], NULL, block_scan, (void*)i);
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
    out.open(G);
    for(i = 0; i < scan.size(); i++){ 
        out << scan[i] << "\n";
    }

    return 0;
}