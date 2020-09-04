//Codigo por Nathan Rodrigues de Oliveira
//Qualquer dúvida pode entrar em contato por nathanro@usp.br

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define TAMANHO_DA_POPULACAO 5 // Numero de individuos em cada população

#define PORCENTAGEM_DE_MUTACAO 2 // Porcentagem de alteração no valor a cada mutação

double* AlocaVetordouble(int tamanhomaximo); //Função que aloca um vetor double de tamanho "tamanhomaximo"

double* inicia_populacao(int tamanhodapopulacao); // Função que inicializa a população de tamanho especificado

double busca_melhor(double* populacao,int tamanhodapopulacao); // Função que busca o melhor indivíduo na população (nesse caso o melhor é o que possui o maior valor na função F(x)=5-|x-42|)

void crossovers(double* populacao, double melhor,int tamanhodapopulacao); // Função que altera todos os indivíduos da população, através de uma média com o melhor indivíduo + uma mutação aleatória de 10%

double* AlocaVetordouble(int tamanhomaximo){
    double* resultado=NULL;

    resultado=(double*)malloc(sizeof(double)*tamanhomaximo);
    if(resultado==NULL){printf("Erro na alocação do vetor double - Espaço Insuficiente");exit(1);}

    return resultado;
}

double* inicia_populacao(int tamanhodapopulacao){

    int i;

    double* populacao=NULL;

    populacao=AlocaVetordouble(tamanhodapopulacao);

    for(i=0;i<tamanhodapopulacao;i++){ //Define os primeiros valores como aleatórios entre 0 e 1k com 2 casas decimais
        populacao[i]= (rand()%1000) + (double)((rand()%100)/(100.0f));
          printf("Populacao Inicial [%d] = %lf\n",i,populacao[i]);
    }

    return populacao;

}

double busca_melhor(double* populacao,int tamanhodapopulacao){

    int i,indice=0;

    double melhor,atual;

    melhor=-1*INFINITY; 

    for(i=0;i<tamanhodapopulacao;i++){ //Escolhe o melhor adaptado baseado no maior valor na função f(x)=5-abs(x-42)
        atual=5-(abs(populacao[i]-42));
        if(atual>melhor){
            melhor=atual;
            indice=i;
        } 
        
    }
    return populacao[indice];
}

void crossovers(double* populacao, double melhor, int tamanhodapopulacao){
    
    int i;

    double mutacao;

    for(i=0;i<tamanhodapopulacao;i++){ //Define os primeiros valores como aleatórios entre 0 e 1k
        
        populacao[i] = (melhor+populacao[i])/2.0; //Crossover com o melhor

 //     mutacao=      original                    +-                        X%
        mutacao = populacao[i] + ((double) (rand()%1000)-500)*PORCENTAGEM_DE_MUTACAO/100.0f;

           if(populacao[i]!=melhor) populacao[i]=mutacao; 
        } 
    
    return;
}


int main(void){

    char saida='j'; // Char para saída do programa

    int contador=0; // Contador de gerações

    double melhor; // Variavel para armazenar o melhor individuo de uma população

    double* populacao; // Vetor de doubles que armazenam a população 

    srand(time(NULL)); // Inicialização da semente para gerações randômicas

    populacao=inicia_populacao(TAMANHO_DA_POPULACAO); //Iniciza uma população com elementos aleatórios entre 0 e 1000

    printf("Simulador evolutivo para encontrar o máximo da função F(x)=5-|x-42| - Obs: O melhor valor possível é X=42.00 pois y=0\nPressione 'q' para sair e outra coisa para avançar\n");

    while(saida!='q'){

    melhor=busca_melhor(populacao,TAMANHO_DA_POPULACAO); //Percorre toda a população buscando o melhor elemento (máximo da função F(x)=5-|x-42| )

    printf("Geracao %d - Melhor adaptado = %f",contador,melhor); // Printa os resultados
    contador++;

    crossovers(populacao,melhor,TAMANHO_DA_POPULACAO); // O melhor é cruzado com todos os outros (soma e divide por 2 + uma mutação definida pelos defines), gerando uma nova população

    saida=getchar();

    }

    return 0;
}