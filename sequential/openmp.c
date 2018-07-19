#include <omp.h>
#include <stdio.h>

int main (int argc, char *argv[]) {
  
  printf("Ola 1 - FORA DA REGIAO PARALELA ... \n");

  #pragma omp parallel num_threads(32)
  {
    int id = omp_get_thread_num();
    int n_threads = omp_get_num_threads();
    printf("Sou a thread %d de um total de %d\n" , id, n_threads);
  }

  printf("Ola 2 - FORA DA REGIAO PARALELA ... \n");

  return 0;

}
