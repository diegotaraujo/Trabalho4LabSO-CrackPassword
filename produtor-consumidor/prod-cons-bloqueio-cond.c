#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_ITENS 10
int buffer[N_ITENS];

int inicio = 0, final = 0, cont = 0;

pthread_mutex_t bloqueio;
pthread_cond_t nao_vazio, nao_cheio;

void* produtor(void *v) {
  int i, aux;

  for (i = 0; i < 2 * N_ITENS; i++) {
    pthread_mutex_lock(&bloqueio);
    while (cont == N_ITENS) {
      pthread_cond_wait(&nao_cheio, &bloqueio);
    }
    final = (final + 1) % N_ITENS;
    buffer[final] = i;
    printf("Produtor, item = %d, posição = %d.\n", i, final);     
    aux = cont; 
    printf("Produtor leu total = %d.\n", cont);     
    sleep(random() % 3);   /* Permite que a outra thread execute */
    cont = aux + 1;
    printf("Produtor atualizou total = %d.\n", cont);     
    pthread_cond_signal(&nao_vazio);
    pthread_mutex_unlock(&bloqueio);
    sleep(random() % 3);   /* Permite que a outra thread execute */
  }
  printf("Produção encerrada.\n");     
  return NULL;
}

void* consumidor(void *v) {
  int i, aux;

  for (i = 0; i < 2 * N_ITENS; i++) {
    pthread_mutex_lock(&bloqueio);
    while (cont == 0) {
      pthread_cond_wait(&nao_vazio, &bloqueio);
    }
    inicio = (inicio + 1) % N_ITENS;
    aux = buffer[inicio]; /* Item é consumido */
    printf("Consumidor, item = %d, posição = %d.\n", aux, inicio);
    aux = cont; 
    printf("Consumidor leu total = %d.\n", cont);     
    sleep(random() % 3);  /* Permite que a outra thread execute */  
    cont = aux - 1; 
    printf("Consumidor atualizou total = %d.\n", cont);     
    pthread_cond_signal(&nao_cheio);
    pthread_mutex_unlock(&bloqueio);
    sleep(random() % 3);   /* Permite que a outra thread execute */
  }
  printf("Consumo encerrado.\n");     
  return NULL;
}

int main() {
  int i;
  
  pthread_t thr_produtor, thr_consumidor;

  pthread_mutex_init(&bloqueio, NULL);
  pthread_cond_init(&nao_cheio, NULL);
  pthread_cond_init(&nao_vazio, NULL);

  /* Limpa o buffer */
  for (i = 0; i < N_ITENS; i++)
    buffer[i] = 0;
  
  pthread_create(&thr_produtor, NULL, produtor, NULL);
  pthread_create(&thr_consumidor, NULL, consumidor, NULL);

  pthread_join(thr_produtor, NULL); 
  pthread_join(thr_consumidor, NULL);

  return 0;
}
