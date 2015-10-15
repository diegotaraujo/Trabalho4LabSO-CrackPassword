#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <crypt.h>

#define TAM_HASH 256
#define TAM_SENHA 4
#define N_ITENS 10

int achou=0;
char senha[TAM_SENHA + 1];
char encontrar[TAM_SENHA + 1];

sem_t bloqueio, pos_vazia, pos_ocupada;

void teste(){
	printf("entrou aqui!\n");
}
void calcula_hash_senha(const char *senha, char *hash);
void incrementa_senha(char *senha);
void testa_senha(const char *hash_alvo, const char *senha);
void* produtor(void *v);
void* consumidor(void *v);

int main(int argc, char *argv[]) {
	int i;
  
	pthread_t thr_produtor, thr_consumidor;

	sem_init(&bloqueio, 0, 1);
	sem_init(&pos_vazia, 0, N_ITENS);
	sem_init(&pos_ocupada, 0, 0);
	
	strcpy(encontrar, argv[1]);

	printf("%s\n", encontrar);

	//inicializa a senha com aaaa
	for (i = 0; i < TAM_SENHA; i++) {
   		senha[i] = 'a';
  	}
  	senha[TAM_SENHA] = '\0';

	printf("%s\n", senha);

	pthread_create(&thr_produtor, NULL, produtor, NULL);
	pthread_create(&thr_consumidor, NULL, consumidor, NULL);

	pthread_join(thr_produtor, NULL); 
	pthread_join(thr_consumidor, NULL);

	return 0;
}

void* produtor(void *v) {
	int i;
	
	printf("VOU PRODUZIR!\n");	

	sem_wait(&pos_vazia);
	sem_wait(&bloqueio);
	
	//produz
	//teste();
	//incrementa_senha(senha);	
	
	while(!achou){
	  	i = TAM_SENHA - 1;
	  	while (i >= 0) {
			if (senha[i] != 'z') {
		  		senha[i]++;
		  		i = -2;
			} 
			else {
		  		senha[i] = 'a';
		  		i--;
			}
	  	}
	  	
		if (i < 0) {
			printf("Não achou!\n");
			exit(1);
	  	}
		printf("%d", i);
	}
	printf("\n");

	sem_post(&bloqueio);
	sem_post(&pos_ocupada);
  
	return NULL;
}

void* consumidor(void *v) {
	
	printf("VOU CONSUMIR!\n");	

    sem_wait(&pos_ocupada);
    sem_wait(&bloqueio);
	
	// consume	
	//teste();
	//testa_senha(encontrar, senha);	
	
	char hash_calculado[TAM_HASH + 1];

  	calcula_hash_senha(senha, hash_calculado);
  	if (!strcmp(encontrar, hash_calculado)&& achou == 0) {
    	printf("Achou! %s\n", senha);
    	achou=1;
  	}  	
   
    sem_post(&bloqueio);
    sem_post(&pos_vazia);

	return NULL;
}

void testa_senha(const char *hash_alvo, const char *senha) {
  char hash_calculado[TAM_HASH + 1];

  calcula_hash_senha(senha, hash_calculado);
  if (!strcmp(hash_alvo, hash_calculado)) {
    printf("Achou! %s\n", senha);
    exit(0);
  }
}

void incrementa_senha(char *senha) { 
}

void calcula_hash_senha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}
