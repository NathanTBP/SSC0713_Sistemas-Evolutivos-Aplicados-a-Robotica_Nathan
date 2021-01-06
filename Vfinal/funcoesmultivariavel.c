//Codigo por Nathan Rodrigues de Oliveira

//Qualquer dúvida e, principalmente, alguma recomendação, por favor, entre em contato por nathanro@usp.br

// Esse programa busca encontrar mínimos e/ou máximos de funções dadas, desde que elas possam ser calculadas em um intervalo de forma evolutiva -> esse programa busca ser o mais simples e didático possível
// A evolução do código (com cada vez mais "features" está disponível em: https://github.com/NathanTBP/SSC0713_Sistemas-Evolutivos-Aplicados-a-Robotica_Nathan)

//Inclusão das bibliotecas padrão de C.
#include <stdio.h> // Para entrada/saida (leitura e escrita via teclado monitor e arquivos)
#include <stdlib.h> // Para alocação dinâmica
#include <string.h> // Para manipular strings e setar memória
#include <time.h> // Para conseguir o tempo atual do sistema como uma chave randômica
#include <math.h> // Para calcular mais fácilmente algumas funções
#include "tinyexpr.h" // Para avaliar expressoes

#define GNUPLOT "gnuplot -persist" // Para abrir o GNU PLOT: caso não esteja instalado usar: sudo apt install gnuplot

//Definição de um tipo booleano para C -> Unsigned char é a menor unidade de dados possível na linguagem.
#define boolean unsigned char 
#define TRUE 1
#define FALSE 0

//definição de máximos e mínimos para 2 valores
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

#define TAMANHO_MAXIMO_DA_EXPRESSAO 500 // Define um tamanho máximo para a expressao a ser computada
#define TAMANHO_MAXIMO_POR_VARIAVEL 10 // Define um tamanho máximo para o nome de cada variável.

//Definições das condições iniciais do sistema
#define INICIAL_TAMANHO_DA_POPULACAO 5 // Numero inicial de individuos em cada população
#define INICIAL_PORCENTAGEM_DE_MUTACAO 2.0 // Porcentagem inicial de alteração no valor a cada mutação
#define INICIAL_NUMERO_DE_GERACOES_PARA_MUTAR 10 // Numero inicial de Gerações para alterar a taxa de mutação
#define INICIAL_NUMERO_DE_GERACOES_PARA_PREDAR 10 // Numero inicial de Gerações para ocorrer uma nova predação (do pior elemento)

//Definições de números para switch para melhor legibilidade do código
#define ELITISMO 1
#define TORNEIODE2 2

#define SEMPREDAR 1
#define PREDARANDOMICA 2
#define PREDAPORSINTESE 3

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
#define MENU_PLOTA_GRAFICO 15
#define MENU_AVANCAX_GERACOES 16
#define MENU_AUTOPLOT 17

#define MAX 1
#define MIN 0

//Essa estrutura permite que uma função retorne 3 informações importantíssimas para o algoritmo
struct mediaefit_{
    double* medias;     // A media dos elementos de cada variável populacao. (para cada variável =Somatorio de todos os individuos/numero de elementos)
    double* fitness;    // Contem todos os fitness dos elementos da populacao
    double fitmedia;    // O Fitness médio da população (=Somatório de todos os fitness/numero de elementos da população)
    int indicemelhor;   // O indice do melhor indivíduo da população 
    int indicepior;     // O indice do pior indivíduo da população 
};

struct qualquerfuncao_{                 // Estrutura que aloca tudo necessário para computar uma função de n variáveis

    int quantasvariaveis;               // Armazena o número de variáveis de uma função.
    char** nomesdasvariaveis;           // Armazena o nome de cada uma das variáveis (x,y,z,a,j,ab,etc)
    char* expressao;                    // Armazena a expressão a ser computada: Ex: X+Y ou 42-|X|;
    double* limitesinferiores;          // Vetor de limites inferiores para convergência das variáveis
    double* limitessuperiores;          // Vetor de limites superiores para convergência das variáveis
    boolean minmax;                     // Variavel para escolher se o algoritmo vai buscar mínimos ou máximos{TRUE = MAX, FALSE = MIN}
    te_variable* variaveisdabiblioteca; // Armazena um vetor de variáveis aos modelos da biblioteca tinyexpr
    te_expr* expressaodabiblio;         // Armazena a expressão aos modelos da biblioteca tinyexpr
    int errofuncao;                     // Variável que armazena se ocorreu um erro ao computadar uma função.
};

struct ponteirosfile_{                  //Estrutura de ponteiros para lidar com arquivos

    /*4 ponteiros do tipo file para escrever os dados em 4 arquivos diferentes: 1 com o numero da geração, 
    Outro com o melhor individuo outro com o fitmedio e outro com o fitdomelhor. 
    Com esses dados formam-se os gráficos necessários para analisar a eficiencia do algoritmo. */

    FILE* plotgeracoes;
    FILE* plotmedia;
    FILE* plotmelhor;
    FILE* plotfitmelhor;

    // Files para executar comandos no terminal a fim de plotar os gráficos em tempo real
    FILE* comandosnormais;
    FILE* comandosgnuplot; 

};

typedef struct mediaefit_ MEDIAEFIT; // Definição de um tipo dessa estrutura. Ao invés de chamar ela de struct mediaefit_, eu chamo só de MEDIAEFIT e ta tudo bem

typedef struct qualquerfuncao_ FUNCAO; // Idem para função

typedef struct ponteirosfile_ PONTEIROSFILE;  // E o mesmo para os ponteiros file

//Obs: Como o programa busca encontrar minimos/máximos em funções, é fácil relacionar (e as vezes confundir) o fitness com o elemento em si.
//O elemento nesse caso seriam os X's, Y's... da função, e seu fitness seria o valor de f(x,y,...), sendo a F a escolhida na execução do programa.

//Variáveis GLOBAIS do meu código. Ou seja, variáveis que podem ser lidas por qualquer função (não precisam ser passadas como parâmetro)
//Elas estão sendo usadas para facilitar as coisas, já que todas, ou quase todas, as funções do código precisam desses dados sempre atualizados;
//Elas também não são um problema, pois é garantido que, quando uma delas for alterada ela devia ser alterada e TODAS as funções querem esse valor atualizado

int TAMANHO_DA_POPULACAO = INICIAL_TAMANHO_DA_POPULACAO ;                       // O tamanho da população é importantíssimo pois, ao percorrer os elementos, ele deve ir de 0 até esse tamanho
double PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO ;                // A porcentagem de mutação também é muito importante, pois representa o quanto cada indivíduo deve mutar, independente do método.
int NUMERO_DE_GERACOES_PARA_MUTAR = INICIAL_NUMERO_DE_GERACOES_PARA_MUTAR;      // Numero de Gerações para alterar a taxa de mutação
int NUMERO_DE_GERACOES_PARA_PREDAR = INICIAL_NUMERO_DE_GERACOES_PARA_PREDAR;    // Numero de Gerações para ocorrer uma nova predação (do pior elemento)

FUNCAO* funcao = NULL;     //Ponteiro para uma estrutura de funcao. Essa que será alocada no início da execução do programa e esse ponteiro estará sempre atrelado a ela.

//Definições das funções (e o que elas devem fazer) -> Facilita o trabalho do compilador e resume tudo que você precisa saber da função só de olhar aqui em cima.

double* AlocaVetordouble(int tamanhomaximo); //Função que aloca um vetor double de tamanho "tamanhomaximo".
double** AlocaVetordouble2(int tamanhomaximo); //Função que aloca um vetor de ponteiros para double de tamanho "tamanhomaximo".
char* AlocaVetorchar(int tamanhomaximo); //Função que aloca um vetor de chars de tamanho "tamanhomaximo".
char** AlocaVetorchar2(int tamanhomaximo); //Função que aloca um vetor de ponteiros para chars de tamanho "tamanhomaximo".


PONTEIROSFILE* inicia_ponteirosfile(void); // Função que abre os arquivos FILE e os deixa prontos para a escrita.

void anota_resultados(PONTEIROSFILE* files,MEDIAEFIT* mediaefit, int geracao); // Função que pega os resultados guardados em MEDIAEFIT e "anotam" eles nos arquivos em disco.

void plota_grafico(PONTEIROSFILE* files); // Função que pega os arquivos, salva eles definitivamente e usa os arquivos gerados para plotar um gráfico usando o GNUplot.

void fecha_arquivos(PONTEIROSFILE** files); // Função que fecha os arquivos que estiverem abertos.

unsigned char menu(char opcoespalavra[50]); // Converte uma string de entrada em uma opção de menu.

void pega_funcao_inicial(void); // Função que recebe os dados do usuário e os aloca e coloca no ponteiro GLOBAL funcao.

double** inicia_populacao(void); // Função que inicializa a população com indivíduos aleatórios utilizando a variavel global de TAMANHO_DA_POPULACAO 

void genocida(double** populacao, double* melhordetodos, int* contadorpredacao); //Função bem semelhante a de iniciar a população, porém, mantém o melhor de todos no 1 slot e considera um vetor já alocado. Além de resetar o contador de predacao

double** redefine_tamanho(double** populacao, MEDIAEFIT* mediaefit, int novotamanhodapop);  /* Função perigosa!

                    Recebe um vetor já alocado de indivíduos e tamanho TAMANHO_DA_POPULACAO que deve ser redefinido para um vetor de tamanho novotamanhodapop
                    Detalhes: O valor da variável TAMANHO_DA_POPULACAO é alterado para novotamanhodapop
                    Se novotamanhodapop>TAMANHO_DA_POPULACAO(inicial) o vetor será extendido, sendo adicionado, em seu fim, novos elementos aleatórios
                    Se novotamanhodapop==TAMANHO_DA_POPULACAO(inicial) nada acontece
                    Se novotamanhodapop<TAMANHO_DA_POPULACAO(inicial) o melhor de todos atual é armazenado na posição 0 e os elementos finais do vetor são excluidos
                    */

void redefine_funcao(void); // Redefine a função escolhida (via entrada padrão e ponteiro file global)
  
MEDIAEFIT* busca_melhor(double** populacao, MEDIAEFIT* mediaefitantigo);  /* 
Função de avaliação: Ela percorre a população avaliando cada indivíduo, armazenando na estrutura MEDIAEFIT os seguintes dados:
     O fitness dew cada indivíduo da população
     O valor médio de cada variável da função (para fazer coisas como predação por síntese)
     O Fitness médio da população (=Somatório de todos os fitness/numero de elementos da população)
     O indice do vetor do melhor de todos, ou seja, qual dos elementos da população é o melhor?
     O indice do vetor do pior de todos, ou seja, qual dos elementos da população é o pior?
     Para isso ela recebe os individuos da população, a função a ser avaliada e a avaliação antiga (para reutilizar a memória, se possível).
*/    

double avalia_funcao(double* valoresdasvariaveis); // Função que recebe um elemento da população (com n variáveis) e usa a funcao global para avaliar o valor da função nesse ponto.

void imprimefits(double* fits) // Imprime todos os fits de uma populacao

void preda(double** populacao,int* contadorpredacao, int indicepiorelemento, double* mediadapopulacao, int tipodepredacao); // Aqui é feita a predação, excluindo o pior elemento da população (a cada n gerações) e o substituindo por um novo aleatório

double* da_uma_mutada(double* variaveisparamutar); // Função que recebe um individuo da populacao e aplica uma mutação em porcentagem definida pela variavel global PORCENTAGEM_DE_MUTACAO

// Utiliza um contador para atualizar a mutação dinâmicamente recebendo o melhor da geração atual, o melhor da geração passada e devolvendo a atual melhor de todos.
// Alem disso, também cuida de que, caso gerações suficientes tenham ocorrido, ocorre um genocídio para reiniciar a população.
void mutaEmata(double** populacao, double* melhor, double fitmelhor , double* fitantigomelhor, int* contador,int* contadorpredacao, boolean mutacaoadaptativa, boolean genocidio); 

void reproducao(double** populacao, MEDIAEFIT* mediaefit, int mododereproducao);

double geraaleatorio(double limiteinferior, double limitesuperior); // Retorna um valor aleatório dentro de um intervalo e com até 5 casas decimais

double coloca_devolta_no_intervalo(double valoraseralterado, double limiteinferior, double limitesuperior); // Pega um valor fora do intervalo definido e o joga devolta tipo pac-man (passou da direita aparece na esquerda e vice versa).

double* AlocaVetordouble(int tamanhomaximo){
    double* resultado=NULL;

    resultado=(double*)malloc(sizeof(double)*tamanhomaximo);
    if(resultado==NULL){printf("Erro na alocação do vetor double - Espaço Insuficiente");exit(1);}
    

    return resultado;
}

double** AlocaVetordouble2(int tamanhomaximo){
    double** resultado=NULL;

    resultado=(double**)malloc(sizeof(double*)*tamanhomaximo);
    if(resultado==NULL){printf("Erro na alocação do vetor double - Espaço Insuficiente");exit(1);}
    

    return resultado;
}

char* AlocaVetorchar(int tamanhomaximo){
    char* resultado=NULL;

    resultado=(char*)malloc(sizeof(char)*tamanhomaximo);
    if(resultado==NULL){printf("Erro na alocação do vetor char - Espaço Insuficiente");exit(1);}
    

    return resultado;
}

char** AlocaVetorchar2(int tamanhomaximo){
    char** resultado=NULL;

    resultado=(char**)malloc(sizeof(char*)*tamanhomaximo);
    if(resultado==NULL){printf("Erro na alocação do vetor char - Espaço Insuficiente");exit(1);}
    

    return resultado;
}

PONTEIROSFILE* inicia_ponteirosfile(void){

    PONTEIROSFILE* files;

    files=(PONTEIROSFILE*)malloc(sizeof(PONTEIROSFILE));

    // Abre os arquivos para leitura
    files->plotgeracoes=fopen("GeracoesparaPlot.txt","w");
    files->plotmedia=fopen("MediafitparaPlot.txt","w");
    files->plotmelhor=fopen("MelhorparaPlot.txt","w");
    files->plotfitmelhor=fopen("MelhorfitparaPlot.txt","w");

    // Abre o terminal para visualização dos gráficos (necessario no meu PC, que usa VcXServer no windows WSL para funções gráficas - explicado melhor no manual)
    files->comandosnormais=popen("export DISPLAY=localhost:0","w");
    fclose(files->comandosnormais);

    //Abre o GNUPLOT
    files->comandosgnuplot=popen("gnuplot -persist","w");
    if (files->comandosgnuplot == NULL) { printf("Erro ao abrir pipe para o GNU plot.\nInstale com 'sudo apt-get install gnuplot'\n"); exit(1);}

    return files;
}

void anota_resultados(PONTEIROSFILE* files,MEDIAEFIT* mediaefit, int geracao){

        int i;

        //Escreve Nr de gerações - Fit do melhor de todos - Média da população e o X do melhor de todos nos respectivos arquivos, um em cada linha.
        fprintf(files->plotgeracoes,"%d\n",geracao);
        fprintf(files->plotmedia,"%lf\n",mediaefit->fitmedia);
        fprintf(files->plotfitmelhor,"%lf\n",mediaefit->fitness[mediaefit->indicemelhor]);     
        for(i=0;i<funcao->quantasvariaveis;i++) fprintf(files->plotmelhor,"%lf ",mediaefit->medias[i]); 
        fprintf(files->plotmelhor,"\n"); 

}

void plota_grafico(PONTEIROSFILE* files){

    fflush(files->plotgeracoes);
    fflush(files->plotmedia);
    fflush(files->plotfitmelhor);

    fprintf(files->comandosgnuplot,"set title 'Melhor fitness e fitness medio'\nset key above\nset xlabel 'Numero de Geracoes'\nset ylabel 'Fitness [F(x)]'\n");
    fprintf(files->comandosgnuplot,"plot 'MediafitparaPlot.txt' title 'Mediafit' with lines, 'MelhorfitparaPlot.txt' title 'Fitdomelhor' with lines \n");
    fflush(files->comandosgnuplot);

    return;
}

void fecha_arquivos(PONTEIROSFILE** files){

    PONTEIROSFILE* file= *files;

    if(file->plotgeracoes!=NULL)       fclose(file->plotgeracoes);
    if(file->plotmedia!=NULL)          fclose(file->plotmedia);
    if(file->plotfitmelhor!=NULL)      fclose(file->plotfitmelhor);
    if(file->plotmelhor!=NULL)         fclose(file->plotmelhor);
    if(file->comandosnormais!=NULL)    fclose(file->comandosnormais);
    if(file->comandosgnuplot!=NULL)    fclose(file->comandosgnuplot);

    free(*files);
    *files=NULL;

    return;
}

void pega_funcao_inicial(void){ // Essa Função recebe todos os dados referentes a qual será a função inicialmente computada pela entrada padrão, aloca uma estrutura com esses dados e os endereça pelo ponteiro global funcao.

    int i;
    int qualfuncao; // Variavel para escolher qual função será escolhida
    boolean valorvalido=FALSE; // Variavel que libera o laço de repetição para inserir valores válidos vai entrada padrão.

    funcao=(FUNCAO*)malloc(sizeof(FUNCAO)); // Aloca uma estrutura para computar a expressão.

    while(valorvalido==FALSE){ //Enquanto o valor não é valido:

    printf("Simulador evolutivo para encontrar mínimos e máximos de funções. Por favor, selecione a função que deseja procurar\n");
    printf("1- F(x)= 5-|x-42| - Obs: O melhor valor possível é X=42.00 pois y=0 e o mínimo é -infinito\n");
    printf("2- F(x)= (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500\n");
    printf("3- F(x)= 10-|x-42|-|y-5| - Obs: Aqui x deve convergir para 42 e y para 5\n");
    printf("4- Inserir função manualmente\n");

    scanf("%d",&qualfuncao);

    switch (qualfuncao){

    case 1: // A função é 5-|x-42|

        funcao->quantasvariaveis=1; // Tem 1 variável

        funcao->nomesdasvariaveis=AlocaVetorchar2(1);
        funcao->nomesdasvariaveis[0]=AlocaVetorchar(2);

        strcpy(funcao->nomesdasvariaveis[0],"x"); // O nome dela é x

        funcao->limitesinferiores=AlocaVetordouble(funcao->quantasvariaveis); // Preciso de 1 elemento para guardar cada limite do intervalo
        funcao->limitessuperiores=AlocaVetordouble(funcao->quantasvariaveis);
        
         printf("Agora insira o intervalo de x em que deseja buscar os ótimos:\nLIMITE INFERIOR DO INTERVALO:\n");
         scanf("%lf",&funcao->limitesinferiores[0]);
         printf("LIMITE SUPERIOR DO INTERVALO:\n");
         scanf("%lf",&funcao->limitessuperiores[0]);

        // A função é 5-|x-42|
        funcao->expressao=AlocaVetorchar(15);
        strcpy(funcao->expressao,"5-abs(x-42)");

        valorvalido=TRUE;
        break;

    case 2:

        funcao->quantasvariaveis=1; // Tem 1 variável

        funcao->nomesdasvariaveis=AlocaVetorchar2(1); // Aloca espaço para 1 variável
        funcao->nomesdasvariaveis[0]=AlocaVetorchar(2); // De tamanho 2

        strcpy(funcao->nomesdasvariaveis[0],"x"); // O nome dela é x

        funcao->limitesinferiores=AlocaVetordouble(funcao->quantasvariaveis); // Preciso de 1 elemento para guardar cada limite do intervalo
        funcao->limitessuperiores=AlocaVetordouble(funcao->quantasvariaveis);

         printf("Agora insira o intervalo de x em que deseja buscar os ótimos:\nLIMITE INFERIOR DO INTERVALO:\n");
         scanf("%lf",&funcao->limitesinferiores[0]);
         printf("LIMITE SUPERIOR DO INTERVALO:\n");
         scanf("%lf",&funcao->limitessuperiores[0]);

        // A funcao é (2*cos(0.039*x)+5*sin(0.05*x)+0.5*cos(0.01*x)+10*sin(0.07*x)+5*sin(0.1*x)+5*sin(0.035*x))*10+500
        funcao->expressao=AlocaVetorchar(100);
        strcpy(funcao->expressao,"(2*cos(0.039*x)+5*sin(0.05*x)+0.5*cos(0.01*x)+10*sin(0.07*x)+5*sin(0.1*x)+5*sin(0.035*x))*10+500");

        valorvalido=TRUE;
        break;

    case 3: // F(x)= 10-|x-42|-|y-5|

        funcao->quantasvariaveis=2; // Tem 2 variáveis

        funcao->nomesdasvariaveis=AlocaVetorchar2(2);
        funcao->nomesdasvariaveis[0]=AlocaVetorchar(2);
        strcpy(funcao->nomesdasvariaveis[0],"x");// Uma se chama x
        funcao->nomesdasvariaveis[1]=AlocaVetorchar(2);
        strcpy(funcao->nomesdasvariaveis[1],"y"); // A outra se chama y    

        funcao->limitesinferiores=AlocaVetordouble(funcao->quantasvariaveis); // Preciso de 2 elementos para guardar cada limite do intervalo
        funcao->limitessuperiores=AlocaVetordouble(funcao->quantasvariaveis);

        for(i=0;i<funcao->quantasvariaveis;i++){ // Para cada uma das variaveis
         printf("Agora insira o intervalo de %s em que deseja buscar os ótimos:\nLIMITE INFERIOR DO INTERVALO:\n",funcao->nomesdasvariaveis[i]); 
         scanf("%lf",&funcao->limitesinferiores[i]);
         printf("LIMITE SUPERIOR DO INTERVALO:\n"); // Pede os valores do intervalo.
         scanf("%lf",&funcao->limitessuperiores[i]);
        }

        // A função é 10-|x-42|-|y-5|
        funcao->expressao=(char*)malloc(sizeof(char)*25);
        strcpy(funcao->expressao,"10-abs(x-42)-abs(y-5)");

        valorvalido=TRUE;
        break;

    case 4: // Se a função será inserida a mão, os valores devem ser alocados.

        do{
            printf("Quantas variaveis tem a sua função?\n"); // Pega o numero de variáveis (n)
            scanf("%d",&funcao->quantasvariaveis);
        }while(funcao->quantasvariaveis<=0);

        funcao->nomesdasvariaveis=AlocaVetorchar2(funcao->quantasvariaveis); // Aloca espaço para n variáveis

        for(i=0;i<funcao->quantasvariaveis;i++){    //Para cada uma das n variaveis:
            funcao->nomesdasvariaveis[i]=AlocaVetorchar(TAMANHO_MAXIMO_POR_VARIAVEL);
            printf("Qual o nome da sua variavel de numero %d?\n",i+1); // recebe o nome da variavel
            scanf("%s",funcao->nomesdasvariaveis[i]);
        }

        funcao->expressao=AlocaVetorchar(TAMANHO_MAXIMO_DA_EXPRESSAO); // Aloca espaço para a expressão

        printf("Qual expressão deve ser computada?\nFunções suportadas: +,-,*,/,%%,^,abs,acos,asin,atan,ceil,floor,sin,cos,tan,ln,log\n"); // Adquire a expressao
        scanf(" %[^\n]s",funcao->expressao);
        //    printf(".%s.",funcao->expressao);

        funcao->limitesinferiores=AlocaVetordouble(funcao->quantasvariaveis); // Preciso de n elementos para guardar cada limite do intervalo
        funcao->limitessuperiores=AlocaVetordouble(funcao->quantasvariaveis);

        for(i=0;i<funcao->quantasvariaveis;i++){ // Para cada uma das variaveis
         printf("Agora insira o intervalo de %s em que deseja buscar os ótimos:\nLIMITE INFERIOR DO INTERVALO:\n",funcao->nomesdasvariaveis[i]); 
         scanf("%lf",&funcao->limitesinferiores[i]);
         printf("LIMITE SUPERIOR DO INTERVALO:\n"); // Pede os valores do intervalo.
         scanf("%lf",&funcao->limitessuperiores[i]);
        }

        valorvalido=TRUE;
        break;
    
    default:

        printf("Valor inválido, tente novamente");
        break;
    }
    }    

    printf("O valor a ser buscado deve ser o mínimo ou máximo da função? 0 para minimo outra coisa para máximo\n"); // Define se o programa vai buscar minimos ou maximos
    scanf("%d",&qualfuncao);
    if(qualfuncao==0) funcao->minmax = MIN;
    else funcao->minmax = MAX;

    funcao->variaveisdabiblioteca=(te_variable*)malloc(sizeof(te_variable)*funcao->quantasvariaveis); // Aloca espaço para a variáveis de biblioteca

    return;
}

double geraaleatorio(double limiteinferior, double limitesuperior){ // Retorna um valor aleatório dentro de um intervalo e com até 5 casas decimais
  //  printf("Limite inferior = %lf, limite superior = %lf\n",limiteinferior,limitesuperior);
    return limiteinferior + rand()%((int)(limitesuperior - limiteinferior)) + (double)((rand()%100000)/(100000.0f));
}

double coloca_devolta_no_intervalo(double valoraseralterado, double limiteinferior, double limitesuperior){ // Pega um valor fora do intervalo definido e o joga devolta tipo pac-man (passou da direita aparece na esquerda e vice versa).

    double decimal=valoraseralterado-floor(valoraseralterado);

    if(valoraseralterado>limitesuperior) return limiteinferior + (int)valoraseralterado%(int)limitesuperior + decimal; // Tratamento para caso a mutação caia para fora do intervalo (ela vai para o outro extremo do intervalo)
    else if (valoraseralterado<limiteinferior) return limitesuperior - abs((int)valoraseralterado%(int)(limitesuperior - limiteinferior)) + decimal; 
    else return valoraseralterado;
}

unsigned char menu(char opcoespalavra[50]){ // Função que retorna um valor para o switch do menu, baseado em uma string de entrada

if(strcmp(opcoespalavra,"q")==0)                            return MENU_SAIDA;
if(strcmp(opcoespalavra,"m")==0)                            return MENU_MANUAL;
if(strcmp(opcoespalavra,"taxamutacao")==0)                  return MENU_TAXA_DE_MUTACAO;
if(strcmp(opcoespalavra,"tamanhopopulacao")==0)             return MENU_TAMANHO_DA_POPULACAO;
if(strcmp(opcoespalavra,"imprimirpopulacao")==0)            return MENU_IMPRIME_POPULACAO;
if(strcmp(opcoespalavra,"mutacaoadaptativa")==0)            return MENU_MUTACAO_ADAPTATIVA;
if(strcmp(opcoespalavra,"genocidio")==0)                    return MENU_GENOCIDIO;
if(strcmp(opcoespalavra,"predacao")==0)                     return MENU_PREDACAO;
if(strcmp(opcoespalavra,"reproducao")==0)                   return MENU_MODO_DE_REPRODUCAO;
if(strcmp(opcoespalavra,"funcao")==0)                       return MENU_ESCOLHER_FUNCAO;
if(strcmp(opcoespalavra,"intervalo")==0)                    return MENU_ESCOLHER_INTERVALO;
if(strcmp(opcoespalavra,"ngeracoesmutar")==0)               return MENU_ESCOLHER_NUMERO_DE_GERACOES_PARA_MUTAR;
if(strcmp(opcoespalavra,"ngeracoespredar")==0)              return MENU_ESCOLHER_NUMERO_DE_GERACOES_PARA_PREDAR;
if(strcmp(opcoespalavra,"imprimirfits")==0)                 return MENU_IMPRIME_FITS;
if(strcmp(opcoespalavra,"plotargrafico")==0)                return MENU_PLOTA_GRAFICO;
if(strcmp(opcoespalavra,"avancargeracoes")==0)              return MENU_AVANCAX_GERACOES;
if(strcmp(opcoespalavra,"autoplot")==0)                     return MENU_AUTOPLOT;

return MENU_SEGUIR;
}

double avalia_funcao(double* valoresdasvariaveis){ // Função que, a partir dos valores obtidos, devolve o valor da f(x)

    int j;

    double valordaf;

    for(j=0;j<funcao->quantasvariaveis;j++){ // Coloca na variável da biblioteca o nome e o endereço do valor das variáveis
        funcao->variaveisdabiblioteca[j].name=funcao->nomesdasvariaveis[j];
        funcao->variaveisdabiblioteca[j].address=&valoresdasvariaveis[j];
    }
    //Compila a expressão com as variáveis
    funcao->expressaodabiblio=te_compile(funcao->expressao,funcao->variaveisdabiblioteca,funcao->quantasvariaveis,&funcao->errofuncao); // Compila a expressão, guardando todos os dados uteis dela em expressaodabiblioteca

    //Avalia a função
    valordaf=te_eval(funcao->expressaodabiblio);

    //printf("O valor avaliado foi : %lf\n",valordaf);

    //Liberaa os valores
    te_free(funcao->expressaodabiblio);

    return valordaf;
}

double** inicia_populacao(void){

    int i,j;

    double** populacao=NULL;

    populacao=AlocaVetordouble2(TAMANHO_DA_POPULACAO); // Aloca o numero de elementos da população

    for(i=0;i<TAMANHO_DA_POPULACAO;i++){ // Para cada elemento

        populacao[i]=AlocaVetordouble(funcao->quantasvariaveis); // Aloca o número de variáveis
    
        for(j=0;j<funcao->quantasvariaveis;j++){

            populacao[i][j]=geraaleatorio(funcao->limitesinferiores[j],funcao->limitessuperiores[j]); // Preenche cada uma delas com um valor aleatório dentro do seu intervalo
            printf("%s = %lf ",funcao->nomesdasvariaveis[j],populacao[i][j]);
        }

        printf("\n");

    }

    //Imprime as condições iniciais da população

    printf("A Populacao tem %d elementos e a funcao tem %d variaveis\n",TAMANHO_DA_POPULACAO,funcao->quantasvariaveis);

    for(j=0;j<funcao->quantasvariaveis;j++) printf("Limite inferior de %s = %lf, limite superior de %s = %lf\n",funcao->nomesdasvariaveis[j],funcao->limitesinferiores[j],funcao->nomesdasvariaveis[j],funcao->limitessuperiores[j]);

    printf("População inicial:\n");
    for(i=0;i<TAMANHO_DA_POPULACAO;i++){
        printf("Elemento %d: ",i);
        for(j=0;j<funcao->quantasvariaveis;j++){
            printf("%s = %lf ",funcao->nomesdasvariaveis[j],populacao[i][j]);
        }
        printf("\n");
    }


    return populacao;
}

void genocida(double** populacao, double* melhordetodos, int* contadorpredacao){ //Função bem semelhante a de iniciar a população, porém, mantém o melhor de todos no 1 slot e considera um vetor já alocado. Além de resetar o contador de predacao

     int i,j;

     for(j=0;j<funcao->quantasvariaveis;j++){
         populacao[0][j] = melhordetodos[j]; // Guarda o melhor de todos na posição 0
         printf("%s do melhor de todos da geração passada = %lf ",funcao->nomesdasvariaveis[j],melhordetodos[j]);
     } 
     printf("\n");

    for(i=1;i<TAMANHO_DA_POPULACAO;i++){ //Define os primeiros valores como aleatórios entre 0 e 1k com 2 casas decimais
        for(j=0;j<funcao->quantasvariaveis;j++){
          populacao[i][j]= geraaleatorio(funcao->limitesinferiores[j],funcao->limitessuperiores[j]);
          printf("%s da populacao Pós genocidio [%d] = %lf ",funcao->nomesdasvariaveis[j],i,populacao[i][j]);
        }
        printf("\n");    
    }
    (*contadorpredacao)=0; // Reseta o contador de predação

    return;
}

double** redefine_tamanho(double** populacao, MEDIAEFIT* mediaefit, int novotamanhodapop){
    /* 
    Função perigosa! Recebe um vetor já alocado de indivíduos e tamanho TAMANHO_DA_POPULACAO que deve ser redefinido para um vetor de tamanho novotamanhodapop
    Recebe um vetor já alocado de indivíduos e tamanho TAMANHO_DA_POPULACAO que deve ser redefinido para um vetor de tamanho novotamanhodapop
    Detalhes: O valor da variável TAMANHO_DA_POPULACAO é alterado para novotamanhodapop
    Se novotamanhodapop>TAMANHO_DA_POPULACAO(inicial) o vetor será extendido, sendo adicionado, em seu fim, novos elementos aleatórios
    Se novotamanhodapop==TAMANHO_DA_POPULACAO(inicial) nada acontece
    Se novotamanhodapop<TAMANHO_DA_POPULACAO(inicial) o melhor de todos atual é armazenado na posição 0 e os elementos finais do vetor são excluidos
    */

    int i,j;

    double** novapopulacao;

    if(novotamanhodapop==TAMANHO_DA_POPULACAO) return populacao; // Se o novo tamanho é igual o atual, nada acontece.

    else if(novotamanhodapop>TAMANHO_DA_POPULACAO){ 

        //Novo vetor de populacao é criado

        novapopulacao=AlocaVetordouble2(novotamanhodapop);

        for(i=0;i<TAMANHO_DA_POPULACAO;i++){ //Aloca e copia os elementos do vetor antigo
            novapopulacao[i]=AlocaVetordouble(funcao->quantasvariaveis);
            for(j=0;j<funcao->quantasvariaveis;j++){
                novapopulacao[i][j]=populacao[i][j];
            }
        }

        for(;i<novotamanhodapop;i++){ // Termina inserindo elementos aleatorios
            novapopulacao[i]=AlocaVetordouble(funcao->quantasvariaveis);
            for(j=0;j<funcao->quantasvariaveis;j++){
                novapopulacao[i][j]=geraaleatorio(funcao->limitesinferiores[j],funcao->limitessuperiores[j]);
            }
        }
    }

    else{

        //Novo vetor de populacao é criado

        novapopulacao=AlocaVetordouble2(novotamanhodapop);

        for(i=0;i<novotamanhodapop;i++){
            novapopulacao[i]=AlocaVetordouble(funcao->quantasvariaveis);
        }

        //Guarda o melhor de todos na posição 0
        for(j=0;j<funcao->quantasvariaveis;j++) novapopulacao[0][j]=populacao[mediaefit->indicemelhor][j];

        for(i=1;i<novotamanhodapop;i++){ // Preenche o resto da população menor com os membros antigos da população
            for(j=0;j<funcao->quantasvariaveis;j++) novapopulacao[i][j]=populacao[i][j];
        }
    }

    //Libera a população antiga.

    for(i=0;i<TAMANHO_DA_POPULACAO;i++){
        free(populacao[i]);
    }
    free(populacao);

    TAMANHO_DA_POPULACAO=novotamanhodapop;

    return novapopulacao;
}

void redefine_funcao(void){

    return;

}

MEDIAEFIT* busca_melhor(double** populacao, MEDIAEFIT* mediaefitantigo){
/* 
     Função de avaliação: Ela percorre a população avaliando cada indivíduo, armazenando na estrutura MEDIAEFIT os seguintes dados:
     O Fitness médio da população (=Somatório de todos os fitness/numero de elementos da população)
     O indice do vetor do melhor de todos, ou seja, qual dos elementos da população é o melhor?
     O indice do vetor do pior de todos, ou seja, qual dos elementos da população é o pior?
     O Fitness do melhor de todos  
     O valor médio de cada variável da função (para fazer coisas como predação por síntese)
     Para isso ela recebe os individuos da população, a função a ser avaliada e a avaliação antiga (para reutilizar a memória, se possível).
*/ 

    int i,j;

    double fitatual,fitpior,fitmelhor;

    MEDIAEFIT* mediaefit; // Um ponteiro para a estrutura mediaefit, para armazenar todos os tados necessários no retorno da função

    if(mediaefitantigo==NULL){ // Se a estrutura recebida está vazia, cria uma nova
        mediaefit=(MEDIAEFIT*)malloc(sizeof(MEDIAEFIT)); 
        mediaefit->medias=AlocaVetordouble(funcao->quantasvariaveis);
        mediaefit->fitness=AlocaVetordouble(TAMANHO_DA_POPULACAO);
    } 
    else mediaefit=mediaefitantigo; // Se não, usa a antiga

    //Inicializações de variáveis de mediaefit

    if(funcao->minmax==MAX){ // Se estamos buscando o máximo, qualquer coisa é melhor que -inf e qualquer coisa é pior que inf
        fitmelhor=-1*INFINITY; 
        fitpior=INFINITY;
    }
    else{ // Vice versa para buscar o mínimo
        fitmelhor=INFINITY; 
        fitpior=INFINITY*-1;
    }
    //Fitmedio começa em 0, pois sera calculado através de somatorio de cada fit/Numero de indivíduos e os indices são inicializados com -1.
    mediaefit->fitmedia=0;
    mediaefit->indicemelhor=-1;
    mediaefit->indicepior=-1;

    for(j=0;j<funcao->quantasvariaveis;j++) mediaefit->medias[j]=0; // Inicializa o valor das médias para começo do cálculo

    for(i=0;i<TAMANHO_DA_POPULACAO;i++){ //Para cada elemento:

        fitatual=avalia_funcao(populacao[i]); // Avalia a f(x,y,...)

        mediaefit->fitness[i]=fitatual;
        
        mediaefit->fitmedia+=fitatual; // Vai somando os fits para fazer a média do fit

        for(j=0;j<funcao->quantasvariaveis;j++) mediaefit->medias[j]+=populacao[i][j]; // Idem para as médias dos valores individuais
         
        if(funcao->minmax==MAX){ // Se a função quer achar o máximo
        if(fitatual>fitmelhor){ //e o atual é melhor que o melhor
            fitmelhor=fitatual; //Atualiza o melhor
            mediaefit->indicemelhor=i;
        } 
        else if(fitatual<fitpior){ // Se é pior que o pior
            fitpior=fitatual; // Atualiza o pior
            mediaefit->indicepior=i;
        }
        }
        else{ // Idem, só que invertido para achar o mínimo
            if(fitatual<fitmelhor){
           fitmelhor=fitatual;
            mediaefit->indicemelhor=i;
        } 
        if(fitatual>fitpior){
            fitpior=fitatual;
            mediaefit->indicepior=i;
        }
        }

    }

    // Termina o cálculo das médias dividindo pelo tamanho da população
  
    mediaefit->fitmedia=mediaefit->fitmedia/TAMANHO_DA_POPULACAO; 

    for(j=0;j<funcao->quantasvariaveis;j++) mediaefit->medias[j]=mediaefit->medias[j]/TAMANHO_DA_POPULACAO;
    

    return mediaefit;
}

void imprimefits(double* fits){

    int i;

    for(i=0;i<TAMANHO_DA_POPULACAO;i++){
        printf("fit de %d = %lf\n",i,fits[i]);
    }   
}

void preda(double** populacao, int* contadorpredacao, int indicepiorelemento, double* mediadapopulacao, int tipodepredacao){

    int j;

    if(tipodepredacao!=SEMPREDAR){

    //Se a predação estiver ligada, o contador da predação é atualizado e, caso já tenha dado tempo da população se estabilizar (10 gerações) o pior indivíduo é predado.
    if((*contadorpredacao)==NUMERO_DE_GERACOES_PARA_PREDAR){
        (*contadorpredacao)=0;
        printf("Predei um aqui\n");
        for(j=0;j<funcao->quantasvariaveis;j++){
            // Na preda randômica, basta substituir o pior elemento por um novo randômico
            if(tipodepredacao==PREDARANDOMICA) populacao[indicepiorelemento][j]=geraaleatorio(funcao->limitesinferiores[j],funcao->limitessuperiores[j]); 
            else if (tipodepredacao==PREDAPORSINTESE){ populacao[indicepiorelemento][j]=mediadapopulacao[j];
            } 
        

        } populacao[indicepiorelemento]=mediadapopulacao;
    }
    else (*contadorpredacao) = (*contadorpredacao) + 1;

    }

    return;
}

double* da_uma_mutada(double* variaveisparamutar){

    int j;
           
    double novovalor;

    for(j=0;j<funcao->quantasvariaveis;j++){
        //mutacao=   original               cast    50% de chance para + ou-            X       %
        novovalor=variaveisparamutar[j] + ((double) (rand()%1000)-500)*PORCENTAGEM_DE_MUTACAO/100.0f;
        novovalor=coloca_devolta_no_intervalo(novovalor,funcao->limitesinferiores[j],funcao->limitessuperiores[j]);
        variaveisparamutar[j]=novovalor;
    }

    return variaveisparamutar;
}

void mutaEmata(double** populacao, double* melhor, double fitmelhor , double* fitantigomelhor, int* contador,int* contadorpredacao, boolean mutacaoadaptativa, boolean genocidio){
// Utiliza um contador para atualizar a mutação dinâmicamente recebendo o melhor da geração atual, o melhor da geração passada e devolvendo a atual melhor de todos.               
    /* Trecho de código referente a mutação adaptativa. Ele deve:
    1- Sempre verificar a quantas gerações não existe um novo melhor de todos, se esse numero for maior que NUMERO_DE_GERACOES_PARA_MUTAR, ele deve:
            Primeiramente vai buscar diminuir a taxa de mudação (para procurar por ótimos locais)
            Se ele encontrar, vai continuar nessa faixa de mutação para ver se existem ainda mais otimos locais
            Se não:
                Segundamente vai aumentar essa taxa
                Dessa vez, se encontrar, é um outro ponto de ótimo local (com bastante sorte um global), então volta a taxa de mutação para o valor original
    */ 
 

        if((*fitantigomelhor)!=fitmelhor){ // Se o melhor mudou, quer dizer que melhorou (existe um novo melhor de todos), então reseta os contadores. Se não melhorou, o contador é incrementado
            if((*contador)<4*NUMERO_DE_GERACOES_PARA_MUTAR + 1) (*contador)= (*contador) - (*contador)%NUMERO_DE_GERACOES_PARA_MUTAR + 1; //Se o programa ainda está buscando por ótimos locais, é retirado apenas as iterações desse local, o +1 é para ele não modificar a taxa novamente
            else {
                (*contador)=0; 
                if(mutacaoadaptativa==TRUE) PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO;
            }

            (*fitantigomelhor)=fitmelhor;

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
        else if ((*contador)%NUMERO_DE_GERACOES_PARA_MUTAR==0 && contador!=0 && mutacaoadaptativa==TRUE) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO * 10; // Vai aumentando para buscar outros ótimos

        return;
    }

void reproducao(double** populacao, MEDIAEFIT* mediaefit, int mododereproducao){

    int i,j;
    int indicepai,indicemae;
    int sorteado1,sorteado2;

    double** filhos;

    switch (mododereproducao) {

    case ELITISMO: // Função que altera todos os indivíduos da população (tierando o melhor), através de uma média com o melhor indivíduo + uma mutação aleatória definida pelo define

        for(i=0;i<TAMANHO_DA_POPULACAO;i++){ 

            //Novo filho é uma média do atual com o melhor -> não altera o melhor de todos pois a media dele com ele mesmo é ele mesmo

            // Para cada variável o filho é uma média do atual com melhor de todos
            for(j=0;j<funcao->quantasvariaveis;j++) populacao[i][j] = (populacao[mediaefit->indicemelhor][j] + populacao[i][j])/2.0f;
                  
            if(i!=mediaefit->indicemelhor) populacao[i]=da_uma_mutada(populacao[i]); // Não muta o melhor de todos.

            } 
        
        break;

    case TORNEIODE2: // Função que altera todos os indivíduos da população (tierando o melhor), através de competições entre dois indivíduos da população + uma mutação aleatória definida pelo define

        // Aloca uma matriz para alocar os filhos
    filhos=AlocaVetordouble2(TAMANHO_DA_POPULACAO-1);

    for(i=0;i<TAMANHO_DA_POPULACAO-1;i++){
        filhos[i]=AlocaVetordouble(funcao->quantasvariaveis);
    }

    for(i=0;i<TAMANHO_DA_POPULACAO-1;i++){ //Devem ser feitos n-1 filhos (1 elemento é o melhor de todos)
        
        sorteado1=rand()%TAMANHO_DA_POPULACAO; // Sorteia dois individuos
        sorteado2=rand()%TAMANHO_DA_POPULACAO;

        if(mediaefit->fitness[sorteado1]>=mediaefit->fitness[sorteado2]) indicepai=sorteado1; // Escolhe o pai
        else indicepai=sorteado2;

        sorteado1=rand()%TAMANHO_DA_POPULACAO; // Sorteia dois individuos
        sorteado2=rand()%TAMANHO_DA_POPULACAO;

        if(mediaefit->fitness[sorteado1]>=mediaefit->fitness[sorteado2]) indicemae=sorteado1; // Escolhe a mãe
        else indicemae=sorteado2;

        for(j=0;j<funcao->quantasvariaveis;j++) filhos[i][j]=(populacao[indicepai][j]+populacao[indicemae][j])/2.0f; // Tem um filho

        //Imprime o resultado
        for(j=0;j<funcao->quantasvariaveis;j++){
             printf("%s do Pai = %lf %s da Mae = %lf - %s do Filho = %f\n",funcao->nomesdasvariaveis[j],populacao[indicepai][j],funcao->nomesdasvariaveis[j],populacao[indicemae][j],funcao->nomesdasvariaveis[j],filhos[i][j]);
        }

        filhos[i]=da_uma_mutada(filhos[i]); // Da uma mutada
 
    }
    for(i=0;i<TAMANHO_DA_POPULACAO;i++){ // Substitui a população com os novos filhos (garantindo que não vai perder o melhor)
            
        for(j=0;j<funcao->quantasvariaveis;j++) if(populacao[i][j]!=populacao[mediaefit->indicemelhor][j]) populacao[i][j]=filhos[i][j];

    }

    //Com os filhos ja inseridos na população, basta liberá-los.
    for(i=0;i<TAMANHO_DA_POPULACAO-1;i++) free(filhos[i]);
    free(filhos);
        break;
    
    default:
        printf("Modo de reprodução não válido\nSaindo...\n");
        exit(1);
        break;
    }
    
    return;
}

// Aqui começa o programa principal
int main(void){

    int i,j;

    unsigned char opcoes='f'; // Char para opções do programa

    char opcoespalavra[50]; // String para armazenar a entrada do usuário

    boolean loop=TRUE; // Boolean para controlar loops de I/O

    boolean autoplot=FALSE; // Boolean para ligar/deligar o autoplot

    boolean mutacaoadaptativa=FALSE,genocidio=FALSE; // Variaveis que define se a mutacao adaptativa e genocidio  estão ligadas ou desligadas

    int mododereproducao=ELITISMO; // Variavel para armazenar o modo de reprodução inicialmente definido como elitismo

    int mododepreda=SEMPREDAR;
    
    int temp,geracao=0,contador=0,contadorpredacao=0; // Variavel para armazenar um inteiro temporarivamente, Contador de gerações, e contadores para mutacao adaptativa, genocidio e predações

    int modoautomatico=0; // Variavel para avançar rapidamente por gerações

    double fitantigomelhor; // Variável para armazenar o melhor individuo de uma população e compara-lo com o antigo melhor.

    double** populacao; // Vetor de doubles que armazenam a população (Os X's, Y's, Z,s... da função)

    //Variaveis para inserir um função genérica de n variaveis:

    MEDIAEFIT* mediaefit=NULL; // Variavel do tipo MEDIAEFIT para receber os resultados da avaliação (busca_melhor), com o fitmedio,fitdomelhor, o individuo que é o melhor e o índice do pior.

    PONTEIROSFILE* files;

    srand(time(NULL)); // Inicialização da semente para gerações randômicas

    files=inicia_ponteirosfile(); // Inicializa os ponteiros para escrever os resultados.

    pega_funcao_inicial(); // Função que recebe todos os dados referentes a qual será a função inicialmente computada. Obs: a partir de agora, a variavel global "funcao" estará alocada e pronta para uso

    populacao=inicia_populacao(); //Iniciza uma população com elementos aleatórios (baseada na função escolhida)

    mediaefit=busca_melhor(populacao,NULL); // Inicia o mediaefit. (ainda não existe um mediaefit para ser substituido)

    fitantigomelhor=mediaefit->fitness[mediaefit->indicemelhor]; // Inicia o antigomelhor

    while(opcoes!='q'){ // O programa inicia com tudo já inicializado

        geracao++; // Avança a geração (começando a criar dados com a geração 1)

        mediaefit=busca_melhor(populacao,mediaefit); //Percorre toda a população avaliando os indivíduos e armazenando todos os dados importantes em mediaefit. 

        //A função mutaEmata sempre é chamada e cuida das contagens de gerações, seja para alterar a taxa de mutação ou genocidar
        //Se a mutacaoadaptativa estiver ligada, essa função é chamada para comparar o novo melhor, o antigo melhor e o contador para atualizar adequadamente a taxa de mutacao
        //Se o genocidio estiver ligado, no ponto de "reset" da mutacao adaptativa (configurado como 8*NUMERODEGERACOESPARAMUTAR)

        mutaEmata(populacao,populacao[mediaefit->indicemelhor],mediaefit->fitness[mediaefit->indicemelhor],&fitantigomelhor,&contador,&contadorpredacao,mutacaoadaptativa,genocidio); 

        //Faz a devida predação (de acordo com o tipo selecionado por modopreda)
        preda(populacao,&contadorpredacao,mediaefit->indicepior,mediaefit->medias,mododepreda);

        //Escreve os resultados de fitness em arquivos.txt
        anota_resultados(files,mediaefit,geracao);

        printf("Geracao %d - Best fit = %lf\n",geracao,mediaefit->fitness[mediaefit->indicemelhor]); // Printa os resultados na tela

        reproducao(populacao,mediaefit,mododereproducao); //Prepara a próxima geração (cruza os indivíduos)

            if(autoplot==TRUE) plota_grafico(files); // Se a função estiver ligada, plota o gráfico adequadamente.
          
            printf("Pressione 'q' para sair , 'm' para abrir o manual ou 'a' para avançar para a próxima geração\n");

            while(loop){ // Inicio do laço de menu
                
            if(modoautomatico==0)scanf(" %s",opcoespalavra); // Le a string do usuario

            else{ // Se o modo automatico estiver ligado, o programa segue em frente pelo numero de geracoes pedidas pelo usuario
                strcpy(opcoespalavra,"a");
                modoautomatico--;
            }

            opcoes=menu(opcoespalavra); // A função menu vai transformar a string do usuário em um número de 1 a 255 (cada um conhecido como um define no código) após isso, um switch sera feito com as possibilidades do menu

            //printf("Entrada = '%s'\n",opcoespalavra);

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
                printf("%cplotargrafico%c Para imprimir o grafico usando GNUplot\n",'"','"');
                printf("%cavancargeracoes%c Para avançar automaticamente um numero n de geracoes\n",'"','"');
                printf("%cautoplot%c Para ligar°desligar o autoplot\n",'"','"');

                break;

            case MENU_TAXA_DE_MUTACAO: // Muda a taxa de mutação através da variavel global 

                printf("Insira o valor em porcentagem da nova taxa de mutação - Numero atual = %f\n",PORCENTAGEM_DE_MUTACAO); // Pergunta o valor
                scanf("%lf ",&PORCENTAGEM_DE_MUTACAO);// Altera a variavel global
                break;
            
            case MENU_TAMANHO_DA_POPULACAO: // Muda o tamanho de cada população 

                printf("Atenção: alterar o tamanho da população para maior vai gerar novos indivíduos aleatórios, já alterar para menor vai resguardar o melhor de todos na primeira posição e perder os últimos elementos\nTem Certeza que quer fazer isso? 1 para sim, outra coisa para não\n");
                temp=0;
                scanf(" %d",&temp); //Confirmação de segurança
                if(temp==1){
                    printf("Então insira novo numero de elementos da população - Numero atual = %d\n",TAMANHO_DA_POPULACAO);
                    scanf("%d",&temp); // Guarda o novo elemento e chama a função especializada (+ complexa, foi feita fora da main)
                    populacao=redefine_tamanho(populacao,mediaefit,temp);
                    contadorpredacao=0;
                }
                    
                   for(i=0;i<TAMANHO_DA_POPULACAO;i++){
                    printf("Individuo %d da nova populacao: ",i);
                    for(j=0;j<funcao->quantasvariaveis;j++){
                        printf("%s = %lf ",funcao->nomesdasvariaveis[j],populacao[i][j]);
                    }
                    printf("\n");
                }
                break;
            
            case MENU_IMPRIME_POPULACAO: // Imprime a população

                for(i=0;i<TAMANHO_DA_POPULACAO;i++){
                    printf("Individuo %d: ",i);
                    for(j=0;j<funcao->quantasvariaveis;j++){
                        printf("%s = %lf ",funcao->nomesdasvariaveis[j],populacao[i][j]);
                    }
                    printf("\n");
                }
            break;

            case MENU_IMPRIME_FITS: // Imprime os fits da populacao 
                    for(i=0;i<TAMANHO_DA_POPULACAO;i++) printf("Fit de %d = %lf",i,mediaefit->fitness[i]);

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

            case MENU_PREDACAO: //Altera o modo de predação

                printf("Insira o modo de predação:\n1- O novo indivíduo é um novo aleatório (Preda Randômica)\n2- O novo individuo é a média de todos os elementos (Preda por síntese)\n Modo atual = %d\n",mododepreda);
                while(loop==TRUE){
                scanf("%d",&mododepreda);
                contadorpredacao=0;
                switch (mododepreda) // Switch dos modos de reprodução disponiveis
                {
                case SEMPREDAR:
                     printf("Modo selecionado: Sem predação!\n"); 
                     loop=FALSE;
                    break;
                case PREDARANDOMICA:
                    printf("Modo selecionado: Preda Randomica!\n"); 
                    loop=FALSE;
                    break;
                case PREDAPORSINTESE:
                    printf("Modo selecionado: Proda por Síntese\n"); 
                    loop=FALSE;
                    break;
                default:
                    printf("Modo de preda não válido!\n");
                    break;
                }
                }
                loop=TRUE;

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

                redefine_funcao();

            break;

            case MENU_ESCOLHER_INTERVALO: // Seleciona um novo intervalo de busca para a função (Obs: os valores fora do intervalo são redefinidos usando o reposicionamento pacman)
                while(loop==TRUE){
                    for(j=0;j<funcao->quantasvariaveis;j++){
                    printf("Por favor, insira o novo intervalo da variavel %s para procurar no formato 'Limite inferior' 'Limite superior'\n",funcao->nomesdasvariaveis[j]);
                    scanf("%lf %lf",&funcao->limitesinferiores[j],&funcao->limitessuperiores[j]);
                    if(funcao->limitesinferiores[j]>=funcao->limitessuperiores[j]) printf("Intervalo inválido ");
                    else{ printf("Novo intervalo = ]%lf;%lf[",funcao->limitesinferiores[j],funcao->limitessuperiores[j]); loop=FALSE;}
                    }
                }
                loop=TRUE;
                for(i=0;i<TAMANHO_DA_POPULACAO;i++){
                    for(j=0;j<funcao->quantasvariaveis;j++){
                        populacao[i][j]=coloca_devolta_no_intervalo(populacao[i][j],funcao->limitesinferiores[j],funcao->limitessuperiores[j]);
                    }
                    
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

            case MENU_PLOTA_GRAFICO: // Plota o gráfico atual

                plota_grafico(files);

            break;

            case MENU_AVANCAX_GERACOES: // Opção para avançar um numero x de gerações de uma vez, para não ter que apertar a + enter X vezes.
                while(loop==TRUE){
                    printf("Por favor, insira o novo numero de geracoes que deseja avançar:\n");
                    scanf("%d",&modoautomatico);
                    if(modoautomatico<=0) printf("Valor inválido ");
                    else{
                        loop=FALSE;
                        }
                }
                loop=TRUE;
            break;

            case MENU_AUTOPLOT: // Liga/Desliga o autoplot 
                if(autoplot==FALSE){
                    autoplot=TRUE;
                    printf("O autoplot está agora LIGADO!\n");
                } 
                else{ 
                    autoplot=FALSE;
                    printf("O autoplot está agora DESLIGADO!\n");
                }
            break;
            
            default: // Caso qualquer outra coisa seja inserida (um enter ['\n'], por exemplo) o laço é quebrado e uma nova geração é feita
                loop=FALSE;
                break;
            }
            } // Bem nesse parentesis ocorre a verifiação de se loop = TRUE, se for FALSE (caso default) o programa segue adiante
            
        loop=TRUE;
        } // Final do laço geral do programa, basta opcoes ser MENU_SAIDA para sair

        //Fecha os arquivos ainda abertos e libera os vetores ainda alocados

        for(i=0;i<TAMANHO_DA_POPULACAO;i++) if(populacao[i]!=NULL) free(populacao[i]);
        
        if(populacao!=NULL) free(populacao);

        fecha_arquivos(&files);

    return 0;
}