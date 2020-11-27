//Codigo por Nathan Rodrigues de Oliveira

//Qualquer dúvida e, principalmente, alguma recomendação, por favor, entre em contato por nathanro@usp.br

// Esse programa busca encontrar mínimos e/ou máximos de funções dadas, desde que elas possam ser calculadas em um intervalo de forma evolutiva -> esse programa busca ser o mais simples e didático possível
// A evolução do código (com cada vez mais "features" está disponível em: https://github.com/NathanTBP/SSC0713_Sistemas-Evolutivos-Aplicados-a-Robotica_Nathan)

//Inclusão das bibliotecas padrão de C.
#include <stdio.h> // Para entrada/saida (leitura e escrita via teclado monitor e arquivos)
#include <stdlib.h> // Para alocação dinâmica
#include <string.h> // Para manipular strings e setar memória
#include <time.h> // Para conseguir o tempo atual do sistema como uma chave randômica
#include <math.h> // Para calcular mais fácilmente as funções complexas
//#include <GL/glut.h> // Para mostrar o gráfico da função.

//Definição de um tipo booleano para C -> Unsigned char é a menor unidade de dados possível na linguagem.
#define boolean unsigned char 
#define TRUE 1
#define FALSE 0

//Definições das condições iniciais do sistema
#define INICIAL_TAMANHO_DA_POPULACAO 5 // Numero de individuos em cada população
#define INICIAL_PORCENTAGEM_DE_MUTACAO 2.0 // Porcentagem de alteração no valor a cada mutação
#define INICIAL_NUMERO_DE_GERACOES_PARA_MUTAR 10 // Numero de Gerações para alterar a taxa de mutação
#define INICIAL_NUMERO_DE_GERACOES_PARA_PREDAR 10 // Numero de Gerações para ocorrer uma nova predação (do pior elemento)

//Definições de números para switch para melhor legibilidade do código
#define ELITISMO 1
#define TORNEIODE2 2

#define MENU_SEGUIR 255
#define MENU_SAIDA 1
#define MENU_MANUAL 2
#define MENU_TAXA_DE_MUTACAO 3
#define MENU_TAMANHO_DA_POPULACAO 4
#define MENU_IMPRIME_POPULACAO 5
#define MENU_MUTACAO_ADAPTATIVA 6
#define MENU_GENOCIDIO 7
#define MENU_PREDACAO 8
#define MENU_MODO_DE_REPRODUCAO 9
#define MENU_ESCOLHER_FUNCAO 10
#define MENU_ESCOLHER_INTERVALO 11
#define MENU_ESCOLHER_NUMERO_DE_GERACOES_PARA_MUTAR 12
#define MENU_ESCOLHER_NUMERO_DE_GERACOES_PARA_PREDAR 13
#define MENU_IMPRIME_FITS 14

#define MAX 1
#define MIN 0

//Essa estrutura permite que uma função retorne 3 informações importantíssimas para o algoritmo
struct mediaefit_{
    double fitmedia;    // O Fitness médio da população (=Somatório de todos os fitness/numero de elementos da população)
    double melhor;      // O cara que de fato é o melhor de todos, o X que alcança o ponto máximo na função
    double indicepior;  // O indice do vetor do pior de todos, ou seja, qual dos elementos da população é o pior?
    double fitmelhor;   // O Fitness do melhor de todos 
};

typedef struct mediaefit_ MEDIAEFIT; // Definição de um tipo dessa estrutura. Ao invés de chamar ela de struct mediaefit_, eu chamo só de MEDIAEFIT e ta tudo bem

//Obs: Como o programa busca encontrar minimos/máximos em funções, é fácil relacionar (e as vezes confundir) o fitness com o elemento em si.
//O elemento nesse caso seria o X da função, e seu fitness seria o valor de f(x), sendo a F a escolhida abaixo. 

//Variáveis GLOBAIS do meu código. Ou seja, variáveis que podem ser lidas por qualquer função (não precisam ser passadas como parâmetro)
//Elas estão sendo usadas para facilitar as coisas, já que todas, ou quase todas, as funções do código precisam desses dados sempre atualizados;
//Elas também não são um problema, pois é garantido que, quando uma delas for alterada ela devia ser alterada e TODAS as funções querem esse valor atualizado

int TAMANHO_DA_POPULACAO = INICIAL_TAMANHO_DA_POPULACAO ; // O tamanho da população é importantíssimo pois, ao percorrer os elementos, ele deve ir de 0 até esse tamanho
double PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO ; // A porcentagem de mutação também é muito importante, pois representa o quanto cada indivíduo deve mutar, independente do método.
int LIMITE_INFERIOR_DO_INTERVALO; // Esses limites devem ser trocados ao mudar de função e servem como um intervalo que a função é definida (em que região eu posso trabalhar)
int LIMITE_SUPERIOR_DO_INTERVALO;
int NUMERO_DE_GERACOES_PARA_MUTAR = INICIAL_NUMERO_DE_GERACOES_PARA_MUTAR; // Numero de Gerações para alterar a taxa de mutação
int NUMERO_DE_GERACOES_PARA_PREDAR = INICIAL_NUMERO_DE_GERACOES_PARA_PREDAR; // Numero de Gerações para ocorrer uma nova predação (do pior elemento)
boolean minmax = TRUE; //Variavel para escolher se o algoritmo vai buscar mínimos ou máximos{ TRUE = MAX, FALSE = MIN}

//Definições das funções (e o que elas devem fazer) -> Facilita o trabalho do compilador e resume tudo que você precisa saber da função só de olhar aqui em cima.

double* AlocaVetordouble(int tamanhomaximo); //Função que aloca um vetor double de tamanho "tamanhomaximo". Usada para alocar novas populações.

unsigned char menu(char opcoespalavra[50]); // Converte uma string de entrada em uma opção de menu.

double* inicia_populacao(void); // Função que inicializa a população com indivíduos aleatórios utilizando a variavel global de TAMANHO_DA_POPULACAO 

void genocida(double* populacao, double melhordetodos, int* contadorpredacao); //Função bem semelhante a de iniciar a população, porém, mantém o melhor de todos no 1 slot e considera um vetor já alocado. Além de resetar o contador de predacao

double* redefine_tamanho(double* populacao, int novotamanhodapop, int qualfuncao);  /* Função perigosa!

                    Recebe um vetor já alocado de indivíduos e tamanho TAMANHO_DA_POPULACAO que deve ser redefinido para um vetor de tamanho novotamanhodapop
                    Detalhes: O valor da variável TAMANHO_DA_POPULACAO é alterado para novotamanhodapop
                    Se novotamanhodapop>TAMANHO_DA_POPULACAO(inicial) o vetor será extendido, sendo adicionado, em seu fim, novos elementos aleatórios
                    Se novotamanhodapop==TAMANHO_DA_POPULACAO(inicial) nada acontece
                    Se novotamanhodapop<TAMANHO_DA_POPULACAO(inicial) o melhor de todos atual é armazenado na posição 0 e os elementos finais do vetor são excluidos
                    */
                    
MEDIAEFIT busca_melhor(double* populacao, int qualfuncao); /* Função de avaliação: Ela busca o melhor indivíduo na população. 
O outro parâmetro escolhe se você quer o máximo ou o mínimo. */

void imprimefits(double* populacao, int qualfuncao); // Imprime todos os fits de uma populacao (faz uma avaliação)

void preda(double* populacao,int* contadorpredacao, int indicepiorelemento); // Aqui é feita a predação, excluindo o pior elemento da população (a cada ) e o substituindo por um novo aleatório

double da_uma_mutada(double valorasermutado); // Função que recebe um individuo da populacao e aplica uma mutação em porcentagem definida pela variavel global PORCENTAGEM_DE_MUTACAO

// Utiliza um contador para atualizar a mutação dinâmicamente recebendo o melhor da geração atual, o melhor da geração passada e devolvendo a atual melhor de todos.
// Alem disso, também cuida de que, caso gerações suficientes tenham ocorrido, ocorre um genocídio para reiniciar a população
// E, por fim, mata individuos e os substitui por um aleatório a cada 5 gerações
void mutaEmata(double* populacao, double melhor , double* antigomelhor, int* contador,int* contadorpredacao, boolean mutacaoadaptativa, boolean genocidio); 

void elitismo(double* populacao, double melhor); // Função que altera todos os indivíduos da população (tierando o melhor), através de uma média com o melhor indivíduo + uma mutação aleatória definida pelo define

void torneio_de_dois (double* populacao, double melhor); // Função que altera todos os indivíduos da população (tierando o melhor), através de competições entre dois indivíduos da população + uma mutação aleatória definida pelo define

double max (double a, double b); // Função que retorna o maior valor entre a e b;

double geraaleatorio(); // Gera um valor aleatório dentro do intervalo.

double coloca_devolta_no_intervalo(double valoraseralterado); // Pega um valor fora do intervalo definido e o joga devolta tipo pac-man (passou da direita aparece na esquerda e vice versa).

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

double geraaleatorio(){ // Retorna um valor aleatório dentro do intervalo atual da função com até 5 casas decimais
    return (double)(LIMITE_INFERIOR_DO_INTERVALO + rand()%(LIMITE_SUPERIOR_DO_INTERVALO - LIMITE_INFERIOR_DO_INTERVALO) + (double)((rand()%100000)/(100000.0f)));
}

unsigned char menu(char opcoespalavra[50]){

if(strcmp(opcoespalavra,"q")==0) return MENU_SAIDA;
if(strcmp(opcoespalavra,"m")==0) return MENU_MANUAL;
if(strcmp(opcoespalavra,"taxamutacao")==0) return MENU_TAXA_DE_MUTACAO;
if(strcmp(opcoespalavra,"tamanhopopulacao")==0) return MENU_TAMANHO_DA_POPULACAO;
if(strcmp(opcoespalavra,"imprimirpopulacao")==0) return MENU_IMPRIME_POPULACAO;
if(strcmp(opcoespalavra,"mutacaoadaptativa")==0) return MENU_MUTACAO_ADAPTATIVA;
if(strcmp(opcoespalavra,"genocidio")==0) return MENU_GENOCIDIO;
if(strcmp(opcoespalavra,"predacao")==0) return MENU_PREDACAO;
if(strcmp(opcoespalavra,"reproducao")==0) return MENU_MODO_DE_REPRODUCAO;
if(strcmp(opcoespalavra,"funcao")==0) return MENU_ESCOLHER_FUNCAO;
if(strcmp(opcoespalavra,"intervalo")==0) return MENU_ESCOLHER_INTERVALO;
if(strcmp(opcoespalavra,"ngeracoesmutar")==0) return MENU_ESCOLHER_NUMERO_DE_GERACOES_PARA_MUTAR;
if(strcmp(opcoespalavra,"ngeracoespredar")==0) return MENU_ESCOLHER_NUMERO_DE_GERACOES_PARA_PREDAR;
if(strcmp(opcoespalavra,"imprimirfits")==0) return MENU_IMPRIME_FITS;

return MENU_SEGUIR;
}

double* inicia_populacao(void){

    int i;

    double* populacao=NULL;

    populacao=AlocaVetordouble(TAMANHO_DA_POPULACAO);

    for(i=0;i<TAMANHO_DA_POPULACAO;i++){ //Define os primeiros valores como aleatórios válidos
        populacao[i]= geraaleatorio();
          printf("Populacao Inicial [%d] = %lf\n",i,populacao[i]);
    }

    return populacao;

}

void genocida(double* populacao, double melhordetodos, int* contadorpredacao){

     int i;

     populacao[0]=melhordetodos;
     printf("Melhor de todos da geração passada = %lf\n",melhordetodos);

    for(i=1;i<TAMANHO_DA_POPULACAO;i++){ //Define os primeiros valores como aleatórios entre 0 e 1k com 2 casas decimais
        populacao[i]= geraaleatorio();
          printf("Populacao Pós genocidio [%d] = %lf\n",i,populacao[i]);
    }
    (*contadorpredacao)=0; // Reseta o contador de predação

    return;

}

double* redefine_tamanho(double* populacao, int novotamanhodapop, int qualfuncao){
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
            novapopulacao[i] = geraaleatorio();
        }
    }

    else{

        novapopulacao=AlocaVetordouble(novotamanhodapop); //Novo vetor de populacao é criado

        novapopulacao[0]=busca_melhor(populacao,qualfuncao).melhor;

        for(i=1;i<novotamanhodapop;i++){ 
            if(populacao[i]!=novapopulacao[0]) novapopulacao[i]=populacao[i]; // Não duplica o melhor de todos, invés disso coloca um próximo filho
            else novapopulacao[i] = populacao[i+1]; 
        }
    }

    TAMANHO_DA_POPULACAO=novotamanhodapop;

    free(populacao);

    return novapopulacao;
}

MEDIAEFIT busca_melhor(double* populacao, int qualfuncao){

    int i,indice=0,indicepior=0;

    double atual,fitpior;

    MEDIAEFIT mediaefit;

    mediaefit.fitmelhor=-1*INFINITY; 
    mediaefit.fitmedia=0;

    for(i=0;i<TAMANHO_DA_POPULACAO;i++){ //Escolhe o melhor adaptado baseado no maior valor na função f(x)=5-abs(x-42)

        switch (qualfuncao)
        {
        case 1:
            atual=5-(fabs(populacao[i]-42)); //Função que deve convergir para 42 (um bico)
            break;
        case 2:
            atual=(2*cos(0.039*populacao[i]) + 5*sin(0.05*populacao[i]) + 0.5*cos(0.01*populacao[i]) + 10*sin(0.07*populacao[i]) + 5*sin(0.1*populacao[i]) + 5*sin(0.035*populacao[i]))*10+500;
           //Função maluca com imagem feita pelo GeoGebra anexada.
           break;
        default:
            break;
        }
        
        mediaefit.fitmedia+=atual;

        if(minmax==MAX){
            fitpior=INFINITY;
        if(atual>mediaefit.fitmelhor){
            mediaefit.fitmelhor=atual;
            indice=i;
        } 
        if(atual<fitpior){
            fitpior=atual;
            indicepior=i;
        }
        }
        else{
            fitpior=INFINITY*-1;
            if(atual<mediaefit.fitmelhor){
            mediaefit.fitmelhor=atual;
            indice=i;
        } 
        if(atual>fitpior){
            fitpior=atual;
            indicepior=i;
        }
        }
        
    }
    // Retorna o pior de todos
    mediaefit.indicepior=indicepior;

    // Retorna o melhor de todos e a media (o fit ja foi calculado acima)
    mediaefit.melhor=populacao[indice]; 
    mediaefit.fitmedia=mediaefit.fitmedia/TAMANHO_DA_POPULACAO; 
    

    return mediaefit;
}

void imprimefits(double* populacao,int qualfuncao){

    int i;

       for(i=0;i<TAMANHO_DA_POPULACAO;i++){ //Escolhe o melhor adaptado baseado no maior valor na função f(x)=5-abs(x-42)

        switch (qualfuncao)
        {
        case 1:
            printf("Fit[%d] = %lf\n",i,5-(fabs(populacao[i]-42))); //Função que deve convergir para 42 (um bico)
            break;
        case 2:
            printf("Fit[%d] = %lf\n",i,(2*cos(0.039*populacao[i]) + 5*sin(0.05*populacao[i]) + 0.5*cos(0.01*populacao[i]) + 10*sin(0.07*populacao[i]) + 5*sin(0.1*populacao[i]) + 5*sin(0.035*populacao[i]))*10+500);
           //Função maluca com imagem feita pelo GeoGebra anexada.
           break;
        default:
            break;
        }
       }
    return;
}

void preda(double* populacao,int* contadorpredacao, int indicepiorelemento){

    //Se a predação estiver ligada, o contador da predação é atualizado e, caso já tenha dado tempo da população se estabilizar (10 gerações) o pior indivíduo é predado.
    if((*contadorpredacao)==NUMERO_DE_GERACOES_PARA_PREDAR){
        (*contadorpredacao)=0;
        printf("Predei um aqui\n");
        populacao[indicepiorelemento]=geraaleatorio(); // Ser predado significa ser substituido por um novo aleatório
    }
    else (*contadorpredacao) = (*contadorpredacao) + 1;

    return;
}

double coloca_devolta_no_intervalo(double valoraseralterado){

    double decimal=valoraseralterado-floor(valoraseralterado);

    if(valoraseralterado>LIMITE_SUPERIOR_DO_INTERVALO) return LIMITE_INFERIOR_DO_INTERVALO + (int)valoraseralterado%LIMITE_SUPERIOR_DO_INTERVALO + decimal; // Tratamento para caso a mutação caia para fora do intervalo (ela vai para o outro extremo do intervalo)
    else if (valoraseralterado<LIMITE_INFERIOR_DO_INTERVALO) return LIMITE_SUPERIOR_DO_INTERVALO - abs((int)valoraseralterado%(LIMITE_SUPERIOR_DO_INTERVALO - LIMITE_INFERIOR_DO_INTERVALO)) + decimal; 
    else return valoraseralterado;
}

double da_uma_mutada(double valorasermutado){

            //mutacao=   original        cast    50% de chance para + ou-            X       %

    double novovalor=valorasermutado + ((double) (rand()%1000)-500)*PORCENTAGEM_DE_MUTACAO/100.0f;

    novovalor=coloca_devolta_no_intervalo(novovalor);

    return novovalor;
}

void mutaEmata(double* populacao, double melhor , double* antigomelhor, int* contador,int* contadorpredacao, boolean mutacaoadaptativa, boolean genocidio){
// Utiliza um contador para atualizar a mutação dinâmicamente recebendo o melhor da geração atual, o melhor da geração passada e devolvendo a atual melhor de todos.               
    /* Trecho de código referente a mutação adaptativa. Ele deve:
    1- Sempre verificar a quantas gerações não existe um novo melhor de todos, se esse numero for maior que NUMERO_DE_GERACOES_PARA_MUTAR, ele deve:
            Primeiramente vai buscar diminuir a taxa de mudação (para procurar por ótimos locais)
            Se ele encontrar, vai continuar nessa faixa de mutação para ver se existem ainda mais otimos locais
            Se não:
                Segundamente vai aumentar essa taxa
                Dessa vez, se encontrar, é um outro ponto de ótimo local (com bastante sorte um global), então volta a taxa de mutação para o valor original
    */ 
 

        if((*antigomelhor)!=melhor){ // Se o melhor mudou, quer dizer que melhorou (existe um novo melhor de todos), então reseta os contadores. Se não melhorou, o contador é incrementado
            if((*contador)<4*NUMERO_DE_GERACOES_PARA_MUTAR + 1) (*contador)= (*contador) - (*contador)% + 1; //Se o programa ainda está buscando por ótimos locais, é retirado apenas as iterações desse local, o +1 é para ele não modificar a taxa novamente
            else {
                (*contador)=0; 
                if(mutacaoadaptativa==TRUE) PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO;
            }

            (*antigomelhor)=melhor;

        }
        else (*contador)++;

        printf("Contador = %d - Taxa de mutação = %f\n",(*contador),PORCENTAGEM_DE_MUTACAO);

        //Avalia o valor do contador

        if(((*contador)==NUMERO_DE_GERACOES_PARA_MUTAR || (*contador)==2*NUMERO_DE_GERACOES_PARA_MUTAR || (*contador) == 3*NUMERO_DE_GERACOES_PARA_MUTAR)&&mutacaoadaptativa==TRUE) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO/10; // Diminui para procurar ótimos locais
        else if ((*contador)==4*NUMERO_DE_GERACOES_PARA_MUTAR&&mutacaoadaptativa==TRUE) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO * 10000; // 
        else if ((*contador)==8*NUMERO_DE_GERACOES_PARA_MUTAR){ // Se buscou até o 80, para e volta a porcentagem inicial e, caso necessário realiza um genocidio
            (*contador)=0;
            if(genocidio==TRUE) genocida(populacao,melhor,contadorpredacao); 
            if(mutacaoadaptativa==TRUE)PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO;
        }
        else if ((*contador)%NUMERO_DE_GERACOES_PARA_MUTAR==0 && contador!=0) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO * 10; // Vai aumentando para buscar outros ótimos

        return;
    }

void elitismo(double* populacao, double melhor){
    
    int i;

    for(i=0;i<TAMANHO_DA_POPULACAO;i++){ 
        
        populacao[i] = (melhor+populacao[i])/2.0; //Novo filho é uma média do atual com o melhor -> não altera o melhor de todos pois a media dele com ele mesmo é ele mesmo

        if(populacao[i]!=melhor) populacao[i]=da_uma_mutada(populacao[i]);

        } 
    
    return;
}

void torneio_de_dois (double* populacao, double melhor){

    int i,j;

    double pai,mae;

    double* filhos;

    filhos=AlocaVetordouble(TAMANHO_DA_POPULACAO-1); // Aloca um vetor para armazenar os filhos

    for(i=0;i<TAMANHO_DA_POPULACAO-1;i++){ //Sortear para fazer filhos (o mesmo pai pode ser usado todas as vezes)
        
        pai=max(populacao[rand()%TAMANHO_DA_POPULACAO],populacao[rand()%TAMANHO_DA_POPULACAO]); // Sorteia dois individuos, escolhe o melhor deles para ser pai
        mae=max(populacao[rand()%TAMANHO_DA_POPULACAO],populacao[rand()%TAMANHO_DA_POPULACAO]); // Sorteia dois individuos, escolhe o melhor deles para ser mae

        filhos[i]=(pai+mae)/2; // Tem um filho

        printf("Pai %f - Mae %f - Filho %f\n",pai,mae,filhos[i]);

        filhos[i]=da_uma_mutada(filhos[i]); // Da uma mutada

        } 

        for(i=0,j=0;i<TAMANHO_DA_POPULACAO;i++){ // Substitui a população com os novos filhos (garantindo que não vai perder o melhor)
            if(populacao[i]!=melhor){
                populacao[i]=filhos[j];
                j++;
            }
        }

        free(filhos);
    
    return;

}

// Aqui começa o programa principal
int main(void){

    int i;

    unsigned char opcoes='f'; // Char para opções do programa

    char opcoespalavra[50]; // String para armazenar a entrada do usuário

    boolean loop=TRUE; // Boolean para controlar loops de I/O

    boolean mutacaoadaptativa=FALSE,genocidio=FALSE,predacao=FALSE; // Variaveis que define se a mutacao adaptativa, genocidio e predação estão ligadas ou desligadas

    int mododereproducao=ELITISMO; // Variavel para armazenar o modo de reprodução inicialmente definido como elitismo
    
    int qualfuncao;  // Escolhe qual das funções pré definidas o programa vai buscar

    int temp,geracao=0,contador=0,contadorpredacao=0,mutacoesdiferentes=0; // Variavel para armazenar um inteiro temporarivamente, Contador de gerações, e contadores para mutacao adaptativa, genocidio e predações

    double antigomelhor; // Variável para armazenar o melhor individuo de uma população e compara-lo com o antigo melhor.

    double* populacao; // Vetor de doubles que armazenam a população (Os X's da função)

    MEDIAEFIT mediaefit; // Variavel do tipo MEDIAEFIT para receber os resultados da avaliação (busca_melhor), com o fitmedio,fitdomelhor, o individuo que é o melhor e o índice do pior.

    /*4 ponteiros do tipo file para escrever os dados em 4 arquivos diferentes: 1 com o numero da geração, Outro com o melhor individuo outro com o fitmedio e outro com o fitdomelhor. 
    Com esses dados formam-se os gráficos necessários para analisar a eficiencia do algoritmo. */

    FILE* plotgeracoes=NULL;
    FILE* plotmedia=NULL;
    FILE* plotmelhor=NULL;
    FILE* plotfitmelhor=NULL;

    srand(time(NULL)); // Inicialização da semente para gerações randômicas

    // Abre os arquivos para leitura
    plotgeracoes=fopen("GeracoesparaPlot.txt","w");
    plotmedia=fopen("MediafitparaPlot.txt","w");
    plotmelhor=fopen("MelhorparaPlot.txt","w");
    plotfitmelhor=fopen("MelhorfitparaPlot.txt","w");

    printf("Simulador evolutivo para encontrar mínimos e máximos de funções. Por favor, selecione a função que deseja procurar\n");
    printf("1- F(x)= 5-|x-42| - Obs: O melhor valor possível é X=42.00 pois y=0 e o mínimo é -infinito\n");
    printf("2- F(x)= (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500\n");

    scanf("%d",&qualfuncao);

    printf("Agora insira o intervalo em que deseja buscar os ótimos:\n(int)LIMITE INFERIOR DO INTERVALO:\n");
    scanf("%d",&LIMITE_INFERIOR_DO_INTERVALO);
    printf("(int)LIMITE SUPERIOR DO INTERVALO:\n");
    scanf("%d",&LIMITE_SUPERIOR_DO_INTERVALO);

    populacao=inicia_populacao(); //Iniciza uma população com elementos aleatórios

    antigomelhor=busca_melhor(populacao,qualfuncao).melhor; // Começa o programa inicializando o antigomelhor 

    while(opcoes!='q'){ // O programa inicia com tudo já inicializado

        geracao++; // Avança a geração (começando a criar dados com a geração 1)

        mediaefit=busca_melhor(populacao,qualfuncao); //Percorre toda a população avaliando os indivíduos e armazenando todos os dados importantes em mediaefit. 

        //A função mutaEmata sempre é chamada e cuida das contagens de gerações, seja para alterar a taxa de mutação ou genocidar

        //Se a mutacaoadaptativa estiver ligada, essa função é chamada para comparar o novo melhor, o antigo melhor e o contador para atualizar adequadamente a taxa de mutacao
        //Se o genocidio estiver ligado, no ponto de "reset" da mutacao adaptativa (configurado como 8*NUMERODEGERACOESPARAMUTAR)

        mutaEmata(populacao,mediaefit.melhor,&antigomelhor,&contador,&contadorpredacao,mutacaoadaptativa,genocidio); 

        //Verifica se deve ocorrer uma predação
        if(predacao==TRUE) preda(populacao,&contadorpredacao,mediaefit.indicepior);

        printf("Geracao %d - Melhor adaptado = %lf com fit = %lf\n",geracao,mediaefit.melhor,mediaefit.fitmelhor); // Printa os resultados na tela
        //Escreve Nr de gerações - Fit do melhor de todos - Média da população e o X do melhor de todos nos respectivos arquivos, um em cada linha.
        fprintf(plotgeracoes,"%d\n",geracao);
        fprintf(plotmedia,"%lf\n",mediaefit.fitmedia);
        fprintf(plotfitmelhor,"%lf\n",mediaefit.fitmelhor);     
        fprintf(plotmelhor,"%lf\n",mediaefit.melhor); 

        switch(mododereproducao){

            case ELITISMO: // O melhor tem filhos com todos os outros 

                    elitismo(populacao,mediaefit.melhor); // O melhor é cruzado com todos os outros (soma e divide por 2 + uma mutação definida pelos defines), gerando uma nova população

            break;

            case TORNEIODE2: // Sorteio entre 2 individuos aleatórios, eles competem e quem ganhar vai ser classificado para ter um filho, isso é repetido n-1 (1 vaga é garantida para o melhor de todos) vezes e a população é substituida
            //O efeito prático é que é garantido que quem é melhor tem maior probabilidade de ser escolhido, (Não GARANTE que eles sejam escolhidos, apenas + probabilidade)
            //Ele geram subpopulações ao redor de alguns ótimos --> Converge mais devagar porém não perde tanto a diversidade.

                    torneio_de_dois(populacao,mediaefit.melhor);

            break;

        }

            printf("Pressione 'q' para sair , 'm' para abrir o manual ou 'a' para avançar para a próxima geração\n");
            
            while(loop){

            scanf(" %s",opcoespalavra); // Le a string do usuario

            opcoes=menu(opcoespalavra); // A função menu vai transformar a string do usuário em um número de 1 a 255 (cada um conhecido como um define no código) após isso, um switch sera feito com as possibilidades do menu

            printf("Entrada = '%s'\n",opcoespalavra);

            switch (opcoes)
            {
            case MENU_SAIDA: // Ao apertar 'q' laço é quebrado e o programa se encerra
                loop=FALSE;
                opcoes='q';
                break; 
            
            case MENU_MANUAL: // Printa a mensagem do manual

                printf("%cq%c Para sair\n",'"','"');
                printf("%cm%c Para abrir essa mensagem\n",'"','"');
                printf("%ctamanhopopulacao%c Para alterar o tamanho de cada geração de populações\n",'"','"');
                printf("%ctaxamutacao%c Para alterar a porcentagem de mutação base\n",'"','"');
                printf("%cimprimirpopulacao%c Para imprimir a populacao atual\n",'"','"');
                printf("%cimprimirfits%c Para imprimir os fits da populacao atual\n",'"','"');
                printf("%cmutacaoadaptativa%c Para ligar/desligar a mutacao adaptativa\n",'"','"');
                printf("%cgenocidio%c Para ligar/desligar o genocidio\n",'"','"');
                printf("%cpredacao%c Para ligar/desligar a predação\n",'"','"');
                printf("%creproducao%c Para alterar o modo de reprodução \n",'"','"');
                printf("%cfuncao%c Para alterar a função que o programa deve processar\n",'"','"');
                printf("%cintervalo%c Para alterar o intervalo dos números buscados na função\n",'"','"');
                printf("%cngeracoesmutar%c Para alterar o numero de gerações sem melhoria para mutação adaptativa\n",'"','"');
                printf("%cngeracoespredar%c Para alterar o numero de gerações para que ocorra uma predação\n",'"','"');

                break;

            case MENU_TAXA_DE_MUTACAO: // Muda a taxa de mutação através da variavel global 

                printf("Insira o valor em porcentagem da nova taxa de mutação - Numero atual = %f\n",PORCENTAGEM_DE_MUTACAO); // Pergunta o valor
                scanf("%lf ",&mediaefit.melhor);
                PORCENTAGEM_DE_MUTACAO=mediaefit.melhor; // Altera a variavel global

                break;
            
            case MENU_TAMANHO_DA_POPULACAO: // Muda o tamanho de cada população 

                printf("Atenção: alterar o tamanho da população para maior vai gerar novos indivíduos aleatórios, já alterar para menor vai resguardar o melhor de todos na primeira posição e perder os últimos elementos\nTem Certeza que quer fazer isso? 1 para sim, outra coisa para não\n");
                temp=0;
                scanf(" %d",&temp); //Confirmação de segurança
                if(temp==1){
                    printf("Então insira novo numero de elementos da população - Numero atual = %d\n",TAMANHO_DA_POPULACAO);
                    scanf("%d",&temp); // Guarda o novo elemento e chama a função especializada (+ complexa, foi feita fora da main)
                    populacao=redefine_tamanho(populacao,temp,qualfuncao);
                    contadorpredacao=0;
                    
                    for(i=0;i<TAMANHO_DA_POPULACAO;i++){ // Imprime a nova população p/ o usuário
                        printf("Nova população[%d] = %lf\n",i,populacao[i]);
                    }
                }
                break;
            
            case MENU_IMPRIME_POPULACAO: // Imprime a população

                for(i=0;i<TAMANHO_DA_POPULACAO;i++){
                    printf("Populacao[%d] = %lf\n",i,populacao[i]);
                }
            break;

            case MENU_IMPRIME_FITS: // Imprime os fits da populacao (faz uma avaliação completa)
                    imprimefits(populacao,qualfuncao);
            break;

            case MENU_MUTACAO_ADAPTATIVA: // Liga/desliga a mutação adaptativa

                if(mutacaoadaptativa==FALSE){
                    mutacaoadaptativa=TRUE;
                    printf("A mutação adaptativa está agora LIGADA!\n");
                } 
                else{ 
                    mutacaoadaptativa=FALSE;
                    printf("A mutação adaptativa está agora DESLIGADA!\n");
                }

            break;

            case MENU_GENOCIDIO: //Liga/desliga o genocidio

                 if(genocidio==FALSE){
                    genocidio=TRUE;
                    printf("O genocídio está agora LIGADO!\n");
                } 
                else{ 
                    genocidio=FALSE;
                    printf("O genocídio  está agora DESLIGADO!\n");
                }
            break;

            case MENU_PREDACAO: //Liga/desliga a predacao

                 if(predacao==FALSE){
                    predacao=TRUE;
                    printf("A predacao está agora LIGADA!\n");
                    contadorpredacao=0;
                } 
                else{ 
                    predacao=FALSE;
                    printf("A predacao está agora DESLIGADa!\n");
                }
            break;
 
            case MENU_MODO_DE_REPRODUCAO: // Altera o modo de reprodução

                printf("Insira o modo da escolha de quem reproduz:\n1- O melhor tem filhos com todos os outros (elitismo)\n2- Torneio de 2\n Modo atual = %d\n",mododereproducao);
                while(loop==TRUE){
                scanf("%d",&mododereproducao);
                switch (mododereproducao) // Switch dos modos de reprodução disponiveis
                {
                case ELITISMO:
                     printf("Modo selecionado: Elistimo\n"); 
                     loop=FALSE;
                    break;
                case TORNEIODE2:
                    printf("Modo selecionado: Torneio de 2\n"); 
                    loop=FALSE;
                    break;
                default:
                    printf("Modo de reprodução não válido!\n");
                    break;
                }
                }
                loop=TRUE;

            break;

            case MENU_ESCOLHER_FUNCAO: // Altera a função a ser procurada. Obs: Isso não redefine a população!

                printf("Por favor, selecione a função que deseja procurar\n");
                printf("1- F(x)= 5-|x-42| - Obs: O melhor valor possível é X=42.00 pois y=0 e o mínimo é -infinito\n");
                printf("2- F(x)= (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500\n");
                while(loop==TRUE){
                scanf("%d",&qualfuncao);

                switch (qualfuncao)
                {
                case 1:
                    printf("Função selecionada: F(x)= 5-|x-42|\n"); 
                    loop=FALSE;
                    break;
                case 2:
                    printf("Função selecionada: F(x)= (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500\n"); 
                    loop=FALSE;
                    break;
                default:
                    printf("Função não válida!\n");
                    break;
                }
                }
                loop=TRUE;

                printf("Agora, digite 0 para buscar o mínimo ou outra coisa para buscar o máximo\n");
                scanf("%d",&temp);
                if(temp==0){
                    minmax=MIN;
                    printf("Buscando Minimo\n");
                } 
                else {
                    minmax=MAX;
                     printf("Buscando Minimo\n");
                }
            break;

            case MENU_ESCOLHER_INTERVALO: // Seleciona um novo intervalo de busca para a função (Obs: os valores fora do intervalo são redefinidos usando o reposicionamento pacman)
                while(loop==TRUE){
                    printf("Por favor, insira o novo intervalo para procurar no formato 'Limite inferior' 'Limite superior'\n");
                    scanf("%d %d",&LIMITE_INFERIOR_DO_INTERVALO,&LIMITE_SUPERIOR_DO_INTERVALO);
                    if(LIMITE_INFERIOR_DO_INTERVALO>=LIMITE_SUPERIOR_DO_INTERVALO) printf("Intervalo inválido ");
                    else{ printf("Novo intervalo = ]%d;%d[",LIMITE_INFERIOR_DO_INTERVALO,LIMITE_SUPERIOR_DO_INTERVALO); loop=FALSE;}
                }
                loop=TRUE;
                for(i=0;i<TAMANHO_DA_POPULACAO;i++){
                    populacao[i]=coloca_devolta_no_intervalo(populacao[i]);
                }
            break;

            case MENU_ESCOLHER_NUMERO_DE_GERACOES_PARA_MUTAR: // Altera o numero de gerações necessárias para variar a mutação adaptativa
                while(loop==TRUE){
                    printf("Por favor, insira o novo numero de geracoes para ocorrer uma mundaça na taxa de mutação:\n");
                    scanf("%d",&NUMERO_DE_GERACOES_PARA_MUTAR);
                    if(NUMERO_DE_GERACOES_PARA_MUTAR<2) printf("Valor inválido ");
                    else {printf("Novo numero de gerações para mutar = %d\n",NUMERO_DE_GERACOES_PARA_MUTAR); loop=FALSE;}
                }
                loop=TRUE;
            break;

            case MENU_ESCOLHER_NUMERO_DE_GERACOES_PARA_PREDAR: // Altera o numero de gerações necessárias para ocorrer uma predação
                while(loop==TRUE){
                    printf("Por favor, insira o novo numero de geracoes para ocorrer uma predação:\n");
                    scanf("%d",&NUMERO_DE_GERACOES_PARA_PREDAR);
                    if(NUMERO_DE_GERACOES_PARA_PREDAR<2) printf("Valor inválido ");
                    else{printf("Novo numero de gerações para predar = %d\n",NUMERO_DE_GERACOES_PARA_PREDAR);loop=FALSE;}
                }
                loop=TRUE;
            break;
            
            default: // Caso qualquer outra coisa seja inserida (um enter ['\n'], por exemplo) o laço é quebrado e uma nova geração é feita
                loop=FALSE;
                break;
            }
            } // Bem nesse parentesis ocorre a verifiação de se loop = TRUE, se for FALSE (caso default) o programa segue adiante
            
        loop=TRUE;
        } // Final do laço geral do programa, basta opcoes ser MENU_SAIDA para sair

        if(populacao!=NULL) free(populacao);

        fclose(plotgeracoes);
        fclose(plotmedia);
        fclose(plotfitmelhor);
        fclose(plotmelhor);

    return 0;
}