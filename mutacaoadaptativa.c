//Codigo por Nathan Rodrigues de Oliveira
//Qualquer dúvida pode entrar em contato por nathanro@usp.br

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <GL/glut.h>

#define boolean unsigned char //Definição de um tipo booleano para C
#define TRUE 1
#define FALSE 0

#define INICIAL_TAMANHO_DA_POPULACAO 5 // Numero de individuos em cada população
#define INICIAL_PORCENTAGEM_DE_MUTACAO 2.0 // Porcentagem de alteração no valor a cada mutação
#define NUMERO_DE_GERACOES_PARA_MUTAR 5

#define ELITISMO 1
#define TORNEIODE2 2

int TAMANHO_DA_POPULACAO = INICIAL_TAMANHO_DA_POPULACAO ;
double PORCENTAGEM_DE_MUTACAO = INICIAL_TAMANHO_DA_POPULACAO ;

double* AlocaVetordouble(int tamanhomaximo); //Função que aloca um vetor double de tamanho "tamanhomaximo"

double* inicia_populacao(int tamanhodapopulacao); // Função que inicializa a população de tamanho especificado

double* redefine_tamanho(double* populacao, int novotamanhodapop); /* Função perigosa! Recebe um vetor já alocado de indivíduos e tamanho TAMANHO_DA_POPULACAO que deve ser redefinido para um vetor de tamanho novotamanhodapop
                                                                      Detalhes: O valor da variável TAMANHO_DA_POPULACAO é alterado para novotamanhodapop
                                                                      Se novotamanhodapop>TAMANHO_DA_POPULACAO(inicial) o vetor será extendido, sendo adicionado, em seu fim, novos elementos aleatórios
                                                                      Se novotamanhodapop==TAMANHO_DA_POPULACAO(inicial) nada acontece
                                                                      Se novotamanhodapop<TAMANHO_DA_POPULACAO(inicial) o melhor de todos atual é armazenado na posição 0 e os elementos finais do vetor são excluidos
                                                                   */

double busca_melhor(double* populacao,int tamanhodapopulacao); // Função que busca o melhor indivíduo na população (nesse caso o melhor é o que possui o maior valor na função F(x)=5-|x-42|)

double da_uma_mutada(double valorasermutado); // Função que recebe um double e aplica uma mutação em porcentagem definida pelo define PORCENTAGEM_DE_MUTACAO

void elitismo(double* populacao, double melhor,int tamanhodapopulacao); // Função que altera todos os indivíduos da população (tierando o melhor), através de uma média com o melhor indivíduo + uma mutação aleatória definida pelo define

void torneio_de_dois (double* populacao, double melhor,int tamanhodapopulacao); // Função que altera todos os indivíduos da população (tierando o melhor), através de competições entre dois indivíduos da população + uma mutação aleatória definida pelo define

double max (double a, double b);

double* AlocaVetordouble(int tamanhomaximo){
    double* resultado=NULL;

    resultado=(double*)malloc(sizeof(double)*tamanhomaximo);
    if(resultado==NULL){printf("Erro na alocação do vetor double - Espaço Insuficiente");exit(1);}

    return resultado;
}

double max (double a, double b){
    if(a>=b) return a;
    else return b;
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

double* redefine_tamanho(double* populacao, int novotamanhodapop){
    /* 
    Função perigosa! Recebe um vetor já alocado de indivíduos e tamanho TAMANHO_DA_POPULACAO que deve ser redefinido para um vetor de tamanho novotamanhodapop
    Detalhes: O valor da variável TAMANHO_DA_POPULACAO é alterado para novotamanhodapop
    Se novotamanhodapop>TAMANHO_DA_POPULACAO(inicial) o vetor será extendido, sendo adicionado, em seu fim, novos elementos aleatórios
    Se novotamanhodapop==TAMANHO_DA_POPULACAO(inicial) nada acontece
    Se novotamanhodapop<TAMANHO_DA_POPULACAO(inicial) o melhor de todos atual é armazenado na posição 0 e os elementos finais do vetor são excluidos
    */

    int i;

    double* novapopulacao;

    if(novotamanhodapop==TAMANHO_DA_POPULACAO) return populacao; // Nada acontece

    else if(novotamanhodapop>TAMANHO_DA_POPULACAO){ 
        novapopulacao=AlocaVetordouble(novotamanhodapop); //Novo vetor de populacao é criado
        for(i=0;i<TAMANHO_DA_POPULACAO;i++){
            novapopulacao[i]=populacao[i]; // Copia os elementos do vetor antigo
        }
        for(;i<novotamanhodapop;i++){ // Termina inserindo elementos aleatorios
            novapopulacao[i] = (rand()%1000) + (double)((rand()%100)/(100.0f));
        }
    }

    else{

        novapopulacao=AlocaVetordouble(novotamanhodapop); //Novo vetor de populacao é criado

        novapopulacao[0]=busca_melhor(populacao,TAMANHO_DA_POPULACAO);

        for(i=1;i<novotamanhodapop;i++){ 
            if(populacao[i]!=novapopulacao[0]) novapopulacao[i]=populacao[i]; // Não duplica o melhor de todos, invés disso coloca um próximo filho
            else novapopulacao[i] = populacao[i+1]; 
        }
    }

    TAMANHO_DA_POPULACAO=novotamanhodapop;

    free(populacao);

    return novapopulacao;
}

double busca_melhor(double* populacao,int tamanhodapopulacao){

    int i,indice=0;

    double melhor,atual;

    melhor=-1*INFINITY; 

    for(i=0;i<tamanhodapopulacao;i++){ //Escolhe o melhor adaptado baseado no maior valor na função f(x)=5-abs(x-42)
        atual=5-(fabs(populacao[i]-42));
        if(atual>melhor){
            melhor=atual;
            indice=i;
        } 
        
    }
    return populacao[indice];
}



double da_uma_mutada(double valorasermutado){
//mutacao=      original        cast    50% de chance para + ou-            X%
    return valorasermutado + ((double) (rand()%1000)-500)*PORCENTAGEM_DE_MUTACAO/100.0f;
}

void elitismo(double* populacao, double melhor, int tamanhodapopulacao){
    
    int i;

    for(i=0;i<tamanhodapopulacao;i++){ 
        
        populacao[i] = (melhor+populacao[i])/2.0; //Novo filho é uma média do atual com o melhor -> não altera o melhor de todos pois a media dele com ele mesmo é ele mesmo

        if(populacao[i]!=melhor) populacao[i]=da_uma_mutada(populacao[i]);

        } 
    
    return;
}

void torneio_de_dois (double* populacao, double melhor,int tamanhodapopulacao){

    int i,j;

    double pai,mae;

    double* filhos;

    filhos=AlocaVetordouble(tamanhodapopulacao-1); // Aloca um vetor para armazenar os filhos

    for(i=0;i<tamanhodapopulacao-1;i++){ //Sortear para fazer filhos (o mesmo pai pode ser usado todas as vezes)
        
        pai=max(populacao[rand()%tamanhodapopulacao],populacao[rand()%tamanhodapopulacao]); // Sorteia dois individuos, escolhe o melhor deles para ser pai
        mae=max(populacao[rand()%tamanhodapopulacao],populacao[rand()%tamanhodapopulacao]); // Sorteia dois individuos, escolhe o melhor deles para ser mae

        filhos[i]=(pai+mae)/2; // Tem um filho

        printf("Pai %f - Mae %f - Filho %f\n",pai,mae,filhos[i]);

        filhos[i]=da_uma_mutada(filhos[i]); // Da uma mutada

        } 

        for(i=0,j=0;i<tamanhodapopulacao;i++){ // Substitui a população com os novos filhos (garantindo que não vai perder o melhor)
            if(populacao[i]!=melhor){
                populacao[i]=filhos[j];
                j++;
            }
        }

        free(filhos);
    
    return;

}


int main(void){

    int i;

    char saida='f'; // Char para saída do programa

    boolean loop=TRUE;

    boolean mutacaoadaptativa=FALSE; // Variavel que define se a mutacao adaptativa está ligada ou desligada

    int mododereproducao; // Variavel para armazenar o modo de reprodução

    int temp,geracao=0,contador=0,mutacoesdiferentes=0; // Variavel para armazeenar um inteiro temporarivamente, Contador de gerações, e contadores para mutacao adaptativa

    double melhor,antigomelhor; // Variáveis para armazenar o melhor individuo de uma população e compara-lo com o antigo melhor.

    double* populacao; // Vetor de doubles que armazenam a população 

    srand(time(NULL)); // Inicialização da semente para gerações randômicas

    populacao=inicia_populacao(TAMANHO_DA_POPULACAO); //Iniciza uma população com elementos aleatórios entre 0 e 1000

    antigomelhor=busca_melhor(populacao,TAMANHO_DA_POPULACAO);

    printf("Simulador evolutivo para encontrar o máximo da função F(x)=5-|x-42| - Obs: O melhor valor possível é X=42.00 pois y=0\n");

    while(saida!='q'){

        if(saida=='f'){
            while(loop){
                printf("Insira o modo da escolha de quem reproduz:\n1- O melhor tem filhos com todos os outros (elitismo)\n2- Torneio de 2\n");
                scanf("%d",&mododereproducao);
                if(mododereproducao==ELITISMO){ printf("Modo selecionado: Elistimo\n"); loop=FALSE;}
                else if (mododereproducao==TORNEIODE2){ printf("Modo selecionado: Torneio de 2\n"); loop=FALSE;}
                else printf("Modo de reprodução não válido!\n");
            }
            loop=TRUE; // Resetando o loop para uma próxima mudança
        }

        melhor=busca_melhor(populacao,TAMANHO_DA_POPULACAO); //Percorre toda a população buscando o melhor elemento (máximo da função F(x)=5-|x-42| )

        if(mutacaoadaptativa==TRUE){ /* Trecho de código referente a mutação adaptativa. Ele deve:
                                        1- Sempre verificar a quantas gerações não existe um novo melhor de todos, se esse numero for maior que NUMERO_DE_GERACOES_PARA_MUTAR, ele deve:
                                            Primeiramente vai buscar diminuir a taxa de mudação (para procurar por ótimos locais)
                                                Se ele encontrar, vai continuar nessa faixa de mutação para ver se existem ainda mais otimos locais
                                                Se não:
                                            Segundamente vai aumentar essa taxa
                                                Dessa vez, se encontrar, é um outro ponto de ótimo local (com bastante sorte um global), então volta a taxa de mutação para o valor original
                                     */ 


            if(antigomelhor!=melhor){ // Se o melhor mudou, quer dizer que melhorou (existe um novo melhor de todos), então reseta os contadores. Se não melhorou, o contador é incrementado
                if(contador<41) contador= contador - contador%10 + 1; //Se o programa ainda está buscando por ótimos locais, é retirado apenas as iterações desse local, o +1 é para ele não modificar a taxa novamente
                else contador=0; 

                antigomelhor=melhor;

            }
            else contador++;

            printf("Contador = %d - Taxa de mutação = %f\n",contador,PORCENTAGEM_DE_MUTACAO);

            //Avalia o valor do contador

            if(contador==10 || contador==20 || contador == 30) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO/10; // Diminui para procurar ótimos locais
            else if (contador==40) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO * 10000; // 
            else if (contador==100){ // Se buscou até o 100, para e volta a porcentagem inicial
                contador=0;
                PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO;
            }
            else if (contador%10==0) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO * 10; // Vai aumentando para buscar outros ótimos

            }

        printf("Geracao %d - Melhor adaptado = %lf\n",geracao,melhor); // Printa os resultados
        geracao++;

        switch(mododereproducao){

            case ELITISMO: // O melhor tem filhos com todos os outros 

                    elitismo(populacao,melhor,TAMANHO_DA_POPULACAO); // O melhor é cruzado com todos os outros (soma e divide por 2 + uma mutação definida pelos defines), gerando uma nova população

            break;

            case TORNEIODE2: // Sorteio entre 2 individuos aleatórios, eles competem e quem ganhar vai ser classificado para ter um filho, isso é repetido n-1 (1 vaga é garantida para o melhor de todos) vezes e a população é substituida
            //O efeito prático é que é garantido que quem é melhor tem maior probabilidade de ser escolhido, (Não GARANTE que eles sejam escolhidos, apenas + probabilidade)
            //Ele geram subpopulações ao redor de alguns ótimos --> Converge mais devagar porém não perde tanto a diversidade.

                    torneio_de_dois(populacao,melhor,TAMANHO_DA_POPULACAO);

            break;

        }

            printf("Pressione 'q' para sair , 'm' para abrir o manual ou 'a' para avançar para a próxima geração\n");
            
            while(loop){

            scanf(" %c",&saida); // Le o modo do usuário

            switch (saida)
            {
            case 'q': // Ao apertar 'q' laço é quebrado e o programa se encerra
                loop=FALSE;
                break; 
            
            case 'm': // Printa a mensagem do manual
                printf("Tutorial de comandos:\n'q' Para sair\n'm' Para abrir essa mensagem\n't' Para alterar o tamanho de cada geração de populações\n'p' Para alterar a porcentagem de mutação base\n's' Para imprimir a populacao atual\n'f' Para ligar/desligar a mutacao adaptativa\n");
                break;

            case 'p': // Muda a taxa de mutação através da variavel global (não mais um define)
                printf("Insira o valor em porcentagem da nova taxa de mutação - Numero atual = %f\n",PORCENTAGEM_DE_MUTACAO);
                scanf("%lf ",&melhor);
                PORCENTAGEM_DE_MUTACAO=melhor;

                break;
            
            case 't': // Muda o tamanho de cada população 
                printf("Atenção: alterar o tamanho da população para maior vai gerar novos indivíduos aleatórios, já alterar para menor vai resguardar o melhor de todos na primeira posição e perder os últimos elementos\nTem Certeza que quer fazer isso? 1 para sim, outra coisa para não\n");
                temp=0;
                scanf(" %d",&temp);
                if(temp==1){
                    printf("Então insira novo numero de elementos da população - Numero atual = %d\n",TAMANHO_DA_POPULACAO);
                    scanf("%d",&temp);
                    populacao=redefine_tamanho(populacao,temp);
                    
                    for(i=0;i<TAMANHO_DA_POPULACAO;i++){
                        printf("Nova população[%d] = %lf\n",i,populacao[i]);
                    }
                    temp=-1;
                }
                break;
            
            case 's': // Imprime a população
                for(i=0;i<TAMANHO_DA_POPULACAO;i++){
                    printf("Populacao[%d] = %lf\n",i,populacao[i]);
                }
            break;

            case 'f': // Liga/desliga a mutação adaptativa

                if(mutacaoadaptativa==FALSE){
                    mutacaoadaptativa=TRUE;
                    printf("A mutação adaptativa está agora LIGADA!\n");
                } 
                else{ 
                    mutacaoadaptativa=FALSE;
                    printf("A mutação adaptativa está agora DESLIGADA!\n");
                }

            break;
            
            default: // Caso qualquer outra coisa seja inserida (um enter ['\n'], por exemplo) o laço é quebrado e uma nova geração é feita
                loop=FALSE;
                break;
            }
            }
            
        loop=TRUE;
        } // Final do laço geral do programa, basta saida ser 'q' para sair

        if(populacao!=NULL) free(populacao);

    return 0;
}