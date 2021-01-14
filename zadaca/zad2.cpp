#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>

#include <pthread.h>


int N, P; // a = vektor duljine N, P = broj radnih dretvi
std::vector<double> A;
std::vector<std::vector<std::pair<double, int>>> M; // tu ce se spremati max elementi
std::vector<pthread_t> threads;


void *find_maxi(void *threadId){
    long long tid, mask = 1;
    int i, start, end, rc;
    tid = (long long)threadId;
    while((tid & mask) == 0 && (tid | mask) < P){
        mask <<= 1;
    }
    for(mask >>= 1; mask > 0; mask >>= 1){
        rc = pthread_create(&threads[tid | mask], NULL, find_maxi, (void*)(tid | mask));
        if(rc){
            std::cout << "Error in pthread_create()! Return code " << rc << std::endl;;
            exit(-1);
        }
    }


    start = tid * N / P;
    end = (tid + 1) * N / P;
    for(i = start; i < end; i++){
        if(M[tid].size() == 0){
            M[tid].push_back({A[i], i});
        } else if(std::abs(M[tid][0].first) < std::abs(A[i])){
            M[tid].resize(0);
            M[tid].push_back({A[i], i});
        } else if(std::abs(M[tid][0].first) == std::abs(A[i])){
            M[tid].push_back({A[i], i});
        }
    }

    for(mask = 1; (tid & mask) == 0 && (tid | mask) < P; mask <<= 1){
        rc = pthread_join(threads[tid | mask], NULL);
        if(rc){
            std::cout << "Error in pthread_join(), error code " << rc << std::endl;
            exit(-1);
        }

        if(M[tid|mask].size() == 0){
            continue;
        } else if(M[tid].size() == 0){
            for(i = 0; i < M[tid|mask].size(); i++){
                M[tid].push_back(M[tid|mask][i]);
            }
        } else if(std::abs(M[tid][0].first) == std::abs(M[tid|mask][0].first)){
            for(i = 0; i < M[tid|mask].size(); i++){
                M[tid].push_back(M[tid|mask][i]);
            }
        } else if(std::abs(M[tid][0].first) < std::abs(M[tid|mask][0].first)){
            M[tid].resize(0);
            for(i = 0; i < M[tid|mask].size();i++){
                M[tid].push_back(M[tid|mask][i]);
            }
        }
    }

    pthread_exit(NULL);
    return NULL;
}



int main(void){
    
    std::string F, H; // A u datoteci F, B u datoteci G, C u datoteci 
    int i, rc;
    //void *status;
    std::ifstream if1;
    std::ofstream out;


    std::cin >> N >> P >> F >> H;

    A.resize(N);
    M.resize(P);
    threads.resize(P);

    if1.open(F);
    for(i = 0; i < N; i++){
        if1 >> A[i];
    }
    if1.close();

    
    // racun
    rc = pthread_create(&threads[0], NULL, find_maxi, (void*)0);
    if(rc){
        printf("Error in pthread_create()! Return code %d\n", rc);
        exit(-1);
    }

    rc = pthread_join(threads[0], NULL);
    if(rc){
        printf("Error in pthread_join()! Return code %d\n", rc);
        exit(-1);
    }


    // output
    out.open(H);
    for(i = 0; i<M[0].size(); i++){ 
        out << M[0][i].first << " " << std::abs(M[0][i].first) << " " << M[0][i].second + 1 << "\n";
    } // da zapise indeks koji pocinje od 1, a ne od 0

    return 0;
}