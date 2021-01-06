//Bibliotecas padrão
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinyexpr.h"

#define TAMANHO_MAXIMO_POR_VARIAVEL 10
#define TAMANHO_MAXIMO_DA_EXPRESSAO 100
  
int main(void) {

    int numerodevariaveis,i,erro;

    double resultado;

    double* variaveis; // Vetor para armazenar o valor das variáveis

    char** nomesdasvariaveis; // Vetor para armazenar os nomes das variáveis

    //Espaço de char para armazenar a expressão a ser avaliada
    char* expressao;

    //As variáveis da biblioteca
    te_variable* variaveisdabiblioteca;
    te_expr* expressaodabiblioteca;

    printf("Quantas variaveis tem a sua função?\n");

    scanf("%d",&numerodevariaveis);

    //Aloca espaço para nomes e valores
    nomesdasvariaveis=(char**)malloc(sizeof(char*)*numerodevariaveis);
    if(nomesdasvariaveis==NULL) exit(1);

    variaveis=(double*)malloc(sizeof(double)*numerodevariaveis);
    if(variaveis==NULL) exit(1);

    for(i=0;i<numerodevariaveis;i++){ // Para cada variável, aloca uma string de tamanho definido pelo define acima
        nomesdasvariaveis[i]=(char*)malloc(sizeof(char)*TAMANHO_MAXIMO_POR_VARIAVEL); // Aqui é usado o tamanho de cada variavel
        if(nomesdasvariaveis[i]==NULL) exit(1);
    }

    for(i=0;i<numerodevariaveis;i++){ // Pega os nomes e valores de cada variável
        printf("Qual o nome e o valor da sua variavel de numero %d?\n",i+1);
        scanf("%s %lf",nomesdasvariaveis[i],&variaveis[i]);
    }

    //Aloca as variáveis da biblioteca
    variaveisdabiblioteca=(te_variable*)malloc(sizeof(te_variable)*numerodevariaveis);

    for(i=0;i<numerodevariaveis;i++){
        //Insere, em cada uma, nome e endereço com seus valores (no caso o vetor variaveis)
        variaveisdabiblioteca[i].name=nomesdasvariaveis[i];
        variaveisdabiblioteca[i].address=&variaveis[i];
    }

    //Aloca espaço para a expressão a ser computada
    expressao=(char*)malloc(sizeof(char)*TAMANHO_MAXIMO_DA_EXPRESSAO);
    if(expressao==NULL) exit(1);

    printf("Qual expressão deve ser computada?\n");
    scanf("%s",expressao);

    //Compila a expressão com os tados usados
    expressaodabiblioteca=te_compile(expressao,variaveisdabiblioteca,numerodevariaveis,&erro);

    resultado=te_eval(expressaodabiblioteca); // Avalia e imprime o resultado

    printf("%lf\n",resultado);

    //Libera toda a memória
    free(expressao);
    for(i=0;i<numerodevariaveis;i++) free(nomesdasvariaveis[i]);
    free(nomesdasvariaveis);
    free(variaveis);
    te_free(expressaodabiblioteca);
    free(variaveisdabiblioteca);

    return 0;
}