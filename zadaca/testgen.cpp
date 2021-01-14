#include <fstream>
#include <iostream>
#include <string>
#include <complex>
#include <random>

int main(void){
    int zad, N;
    std::cin >> zad;
    if(zad == 1){
        std::string F, G;
        std::cin >> N >> F >> G;
        std::ofstream A(F), B(G);
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(-50,50);
        double re, im;
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                re = dist(mt); im = dist(mt);
                A << std::complex<double>(re, im) << " ";
                re = dist(mt); im = dist(mt);
                B << std::complex<double>(re, im) << " ";
            }
            A << "\n";
            B << "\n";
        }

    } else if(zad == 2 || zad == 3){
        std::string F;
        std::cin >> N >> F;
        std::ofstream A(F);
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(-50,50);
        double re;
        for(int i = 0; i < N; i++){
            re = dist(mt);
            A << re << "\n";
        }
    } else{
        std::cout << "only tasks 1, 2 and 3 supported";
    }
}