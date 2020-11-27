//Codigo por Nathan Rodrigues de Oliveira

//Qualquer dúvida e, principalmente, alguma recomendação, por favor, entre em contato por nathanro@usp.br

// Esse programa busca encontrar mínimos e/ou máximos de funções dadas, desde que elas possam ser calculadas em um intervalo de forma evolutiva -> esse programa busca ser o mais simples e didático possível

//Inclusão das bibliotecas padrão de C.
#include <stdio.h> // Para entrada/saida (leitura e escrita via teclado monitor e arquivos)
#include <stdlib.h> // Para alocação dinâmica
#include <time.h> // Para conseguir o tempo atual do sistema como uma chave randômica
#include <math.h> // Para calcular mais fácilmente as funções complexas
//#include <GL/glut.h>

//Definição de um tipo booleano para C -> Unsigned char é a menor unidade de dados possível na linguagem.
#define boolean unsigned char 
#define TRUE 1
#define FALSE 0

//Definições das condições iniciais do sistema
#define INICIAL_TAMANHO_DA_POPULACAO 5 // Numero de individuos em cada população
#define INICIAL_PORCENTAGEM_DE_MUTACAO 2.0 // Porcentagem de alteração no valor a cada mutação
#define NUMERO_DE_GERACOES_PARA_MUTAR 5

//Definições de números para switch para melhor legibilidade do código
#define ELITISMO 1
#define TORNEIODE2 2

#define MAX 1
#define MIN 0

//Essa estrutura permite que uma função retorne 3 informações importantíssimas para o algoritmo
struct mediaefit_{
    double fitmedia;    // O Fitness médio da população (=Somatório de todos os fitness/numero de elementos da população)
    double melhor;      // O cara que de fato é o melhor de todos, o X que alcança o ponto máximo na função
    double fitmelhor;   // O Fitness do melhor de todos 
};

typedef struct mediaefit_ MEDIAEFIT; // Definição de um tipo dessa estrutura. Ao invés de chamar ela de struct mediaefit_, eu chamo só de MEDIAEFIT e ta tudo bem

//Obs: Como o programa busca encontrar minimos/máximos em funções, é fácil relacionar (e as vezes confundir) o fitness com o elemento em si.
//O elemento nesse caso seria o X da função, e seu fitness seria o valor de f(x), sendo a F a escolhida abaixo. 

//Variáveis GLOBAIS do meu código. Ou seja, variáveis que podem ser lidas por qualquer função (não precisam ser passadas como parâmetro)
//Elas estão sendo usadas para facilitar as coisas, já que todas, ou quase todas, as funções do código precisam desses dados sempre atualizados;
//Elas também não são um problema, pois é garantido que, quando uma delas for alterada

int TAMANHO_DA_POPULACAO = INICIAL_TAMANHO_DA_POPULACAO ; // O tamanho da população é importantíssimo pois, ao percorrer os elementos, ele deve ir de 0 até esse tamanho
double PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO ; // A porcentagem de mutação também é muito importante, pois representa o quanto cada indivíduo deve mutar, independente do método.
int LIMITE_INFERIOR_DO_INTERVALO = 0; // Esses limites devem ser trocados ao mudar de função e servem como um intervalo que a função é definida (em que região eu posso trabalhar)
int LIMITE_SUPERIOR_DO_INTERVALO = 2700;
boolean minmax = TRUE; //Variavel para escolher se o algoritmo vai buscar mínimos ou máximos


//Definições das funções (e o que elas devem fazer) -> Facilita o trabalho do compilador e resume tudo que você precisa saber da função só de olhar aqui em cima.

double* AlocaVetordouble(int tamanhomaximo); //Função que aloca um vetor double de tamanho "tamanhomaximo"

double* inicia_populacao(void); // Função que inicializa a população utilizando a variavel global de TAMANHO_DA_POPULACAO

void genocida(double* populacao, double melhordetodos); //Função bem semelhante a de iniciar a população, porém, mantém o melhor de todos no 1 slot e considera um vetor já alocado

                    /* Função perigosa! Recebe um vetor já alocado de indivíduos e tamanho TAMANHO_DA_POPULACAO que deve ser redefinido para um vetor de tamanho novotamanhodapop
                    Detalhes: O valor da variável TAMANHO_DA_POPULACAO é alterado para novotamanhodapop
                    Se novotamanhodapop>TAMANHO_DA_POPULACAO(inicial) o vetor será extendido, sendo adicionado, em seu fim, novos elementos aleatórios
                    Se novotamanhodapop==TAMANHO_DA_POPULACAO(inicial) nada acontece
                    Se novotamanhodapop<TAMANHO_DA_POPULACAO(inicial) o melhor de todos atual é armazenado na posição 0 e os elementos finais do vetor são excluidos
                    */

double* redefine_tamanho(double* populacao, int novotamanhodapop, int qualfuncao, int* contadorpredacao, boolean predacao); 
                    
MEDIAEFIT busca_melhor_e_preda(double* populacao, int qualfuncao, int* contadorpredacao, boolean predacao); // Função que busca o melhor indivíduo na população. O outro parâmetro escolhe se você quer o máximo ou o mínimo. Além disso, se a predacao estiver ligada essa função também é responsável por isso.

double da_uma_mutada(double valorasermutado); // Função que recebe um individuo da populacao e aplica uma mutação em porcentagem definida pela variavel global PORCENTAGEM_DE_MUTACAO

// Utiliza um contador para atualizar a mutação dinâmicamente recebendo o melhor da geração atual, o melhor da geração passada e devolvendo a atual melhor de todos.
// Alem disso, também cuida de que, caso gerações suficientes tenham ocorrido, ocorre um genocídio para reiniciar a população
// E, por fim, mata individuos e os substitui por um aleatório a cada 5 gerações
void mutaEmata(double* populacao, double melhor , double* antigomelhor, int* contador, boolean mutacaoadaptativa, boolean genocidio); 

void elitismo(double* populacao, double melhor); // Função que altera todos os indivíduos da população (tierando o melhor), através de uma média com o melhor indivíduo + uma mutação aleatória definida pelo define

void torneio_de_dois (double* populacao, double melhor); // Função que altera todos os indivíduos da população (tierando o melhor), através de competições entre dois indivíduos da população + uma mutação aleatória definida pelo define

double max (double a, double b); // Função que retorna o maior valor entre a e b;

double geraaleatorio(); // Gera um valor aleatório dentro do intervalo.

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

double geraaleatorio(){
    return (double)(LIMITE_INFERIOR_DO_INTERVALO + rand()%(LIMITE_SUPERIOR_DO_INTERVALO - LIMITE_INFERIOR_DO_INTERVALO) + (double)((rand()%10000)/(10000.0f)));
}

double* inicia_populacao(void){

    int i;

    double* populacao=NULL;

    populacao=AlocaVetordouble(TAMANHO_DA_POPULACAO);

    for(i=0;i<TAMANHO_DA_POPULACAO;i++){ //Define os primeiros valores como aleatórios entre 0 e 1k com 2 casas decimais
        populacao[i]= geraaleatorio();
          printf("Populacao Inicial [%d] = %lf\n",i,populacao[i]);
    }

    return populacao;

}

void genocida(double* populacao, double melhordetodos){

     int i;

     populacao[0]=melhordetodos;
     printf("Melhor de todos da geração passada = %lf\n",melhordetodos);

    for(i=1;i<TAMANHO_DA_POPULACAO;i++){ //Define os primeiros valores como aleatórios entre 0 e 1k com 2 casas decimais
        populacao[i]= geraaleatorio();
          printf("Populacao Pós genocidio [%d] = %lf\n",i,populacao[i]);
    }

    return;

}

double* redefine_tamanho(double* populacao, int novotamanhodapop, int qualfuncao,int* contadorpredacao, boolean predacao){
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

        novapopulacao[0]=busca_melhor_e_preda(populacao,qualfuncao,contadorpredacao,predacao).melhor;

        for(i=1;i<novotamanhodapop;i++){ 
            if(populacao[i]!=novapopulacao[0]) novapopulacao[i]=populacao[i]; // Não duplica o melhor de todos, invés disso coloca um próximo filho
            else novapopulacao[i] = populacao[i+1]; 
        }
    }

    TAMANHO_DA_POPULACAO=novotamanhodapop;

    free(populacao);

    return novapopulacao;
}

MEDIAEFIT busca_melhor_e_preda(double* populacao, int qualfuncao, int* contadorpredacao, boolean predacao){

    int i,indice=0,indicepior=0;

    double atual,fitpior;

    MEDIAEFIT mediaefit;

    fitpior=INFINITY;
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
            if(atual<=mediaefit.fitmelhor){
            mediaefit.fitmelhor=atual;
            indice=i;
        } 
        if(atual>=fitpior){
            fitpior=atual;
            indicepior=i;
        }
        }
        
    }

    //Se a predação estiver ligada, o contador da predação é atualizado e, caso já tenha dado tempo da população se estabilizar (10 gerações) o pior indivíduo é predado.
    if((*contadorpredacao)==10){
        (*contadorpredacao)=0;
        populacao[indicepior]=geraaleatorio(); // Ser predado significa ser substituido por um novo aleatório
    }

    // Retorna o melhor de todos e a media (o fit ja foi calculado acima)
    mediaefit.melhor=populacao[indice]; 
    mediaefit.fitmedia=mediaefit.fitmedia/TAMANHO_DA_POPULACAO; 
    

    return mediaefit;
}



double da_uma_mutada(double valorasermutado){

            //mutacao=   original        cast    50% de chance para + ou-            X%

    double novovalor=valorasermutado + ((double) (rand()%1000)-500)*PORCENTAGEM_DE_MUTACAO/100.0f;

    if(novovalor>LIMITE_SUPERIOR_DO_INTERVALO) return LIMITE_INFERIOR_DO_INTERVALO + (int)novovalor%LIMITE_SUPERIOR_DO_INTERVALO; // Tratamento para caso a mutação caia para fora do intervalo (ela vai para o outro extremo do intervalo)
    else if (novovalor<LIMITE_INFERIOR_DO_INTERVALO) return LIMITE_SUPERIOR_DO_INTERVALO - (LIMITE_INFERIOR_DO_INTERVALO + (int)novovalor%(LIMITE_SUPERIOR_DO_INTERVALO - LIMITE_INFERIOR_DO_INTERVALO); 

    else return novovalor;
}

void mutaEmata(double* populacao, double melhor , double* antigomelhor, int* contador, boolean mutacaoadaptativa, boolean genocidio){
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
            if((*contador)<41) (*contador)= (*contador) - (*contador)%10 + 1; //Se o programa ainda está buscando por ótimos locais, é retirado apenas as iterações desse local, o +1 é para ele não modificar a taxa novamente
            else {
                (*contador)=0; 
                if(mutacaoadaptativa==TRUE) PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO;
            }

            (*antigomelhor)=melhor;

        }
        else (*contador)++;

        printf("Contador = %d - Taxa de mutação = %f\n",(*contador),PORCENTAGEM_DE_MUTACAO);

        //Avalia o valor do contador

        if(((*contador)==10 || (*contador)==20 || (*contador) == 30)&&mutacaoadaptativa==TRUE) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO/10; // Diminui para procurar ótimos locais
        else if ((*contador)==40&&mutacaoadaptativa==TRUE) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO * 10000; // 
        else if ((*contador)==80){ // Se buscou até o 80, para e volta a porcentagem inicial e, caso necessário realiza um genocidio
            (*contador)=0;
            if(genocidio==TRUE) genocida(populacao,melhor); 
            if(mutacaoadaptativa==TRUE)PORCENTAGEM_DE_MUTACAO = INICIAL_PORCENTAGEM_DE_MUTACAO;
        }
        else if ((*contador)%10==0 && contador!=0) PORCENTAGEM_DE_MUTACAO = PORCENTAGEM_DE_MUTACAO * 10; // Vai aumentando para buscar outros ótimos

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


int main(void){

    int i;

    char saida='f'; // Char para saída do programa

    boolean loop=TRUE;

    boolean mutacaoadaptativa=FALSE,genocidio=FALSE,predacao=FALSE; // Variavel que define se a mutacao adaptativa está ligada ou desligada

    int mododereproducao=ELITISMO; // Variavel para armazenar o modo de reprodução inicialmente definido como elitismo
    
    int qualfuncao;  // Escolhe qual das funções pré definidas o programa vai buscar

    int temp,geracao=0,contador=0,contadorpredacao=0,mutacoesdiferentes=0; // Variavel para armazeenar um inteiro temporarivamente, Contador de gerações, e contadores para mutacao adaptativa

    double antigomelhor; // Variável para armazenar o melhor individuo de uma população e compara-lo com o antigo melhor.

    double* populacao; // Vetor de doubles que armazenam a população (Os X's da função)

    MEDIAEFIT mediaefit; // Variavel do tipo MEDIAEFIT para receber os resultados da avaliação (busca_melhor), com o fitmedio,fitdomelhor e o individuo que é o melhor.

    //4 ponteiros do tipo file para escrever os dados em 3 arquivos diferentes: 1 com o numero da geração, Outro com o melhor individuo outro com o fitmedio e outro com o fitdomelhor. Com esses dados formam-se os gráficos necessários.
    FILE* plotgeracoes=NULL;
    FILE* plotmedia=NULL;
    FILE* plotmelhor=NULL;
    FILE* plotfitmelhor=NULL;

    srand(time(NULL)); // Inicialização da semente para gerações randômicas

    populacao=inicia_populacao(); //Iniciza uma população com elementos aleatórios entre 0 e 1000

    // Abre os arquivos para leitura
    plotgeracoes=fopen("GeracoesparaPlot.txt","w");
    plotmedia=fopen("MediafitparaPlot.txt","w");
    plotmelhor=fopen("MelhorparaPlot.txt","w");
    plotfitmelhor=fopen("MelhorfitparaPlot.txt","w");

    printf("Simulador evolutivo para encontrar mínimos e máximos de funções. Por favor, selecione a função que deseja procurar\n");
    printf("1- F(x)= 5-|x-42| - Obs: O melhor valor possível é X=42.00 pois y=0 e o mínimo é -infinito\n");
    printf("2- F(x)= (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500\n");

    scanf("%d",&qualfuncao);

    antigomelhor=busca_melhor_e_preda(populacao,qualfuncao,contadorpredacao,predacao).melhor; // Começa o programa inicializando o antigomelhor 

    while(saida!='q'){ // O programa inicia com tudo já inicializado

        geracao++; // Avança a geração

        mediaefit=busca_melhor_e_preda(populacao,qualfuncao,contadorpredacao,predacao); //Percorre toda a população buscando o melhor elemento (na primeira geração será igual ao antigomelhor). Lembrando que aqui também é feita a predação, caso ligada.

        //Obs: agora, com genocidio,predação e mutação adaptativa, uma nova função está sendo usada. Ela é sempre chamada e cuida das contagens de gerações.
        //Seja para alterar a taxa de mutação, genocidar ou fazer a predação

        mutaEmata(populacao,mediaefit.melhor,&antigomelhor,&contador,mutacaoadaptativa,genocidio); // Se a mutacaoadaptativa estiver ligada, essa função é chamada para comparar o novo melhor, o antigo melhor e o contador para atualizar adequadamente a taxa de mutacao

        printf("Geracao %d - Melhor adaptado = %lf com fit = %lf\n",geracao,mediaefit.melhor,mediaefit.fitmelhor); // Printa os resultados
        //Escreve Nr de gerações - Fit do melhor de todos - Média da população nos respectivos arquivos, um em cada linha.
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

            scanf(" %c",&saida); // Le o modo do usuário

            switch (saida)
            {
            case 'q': // Ao apertar 'q' laço é quebrado e o programa se encerra
                loop=FALSE;
                break; 
            
            case 'm': // Printa a mensagem do manual
                printf("Tutorial de comandos:\n'q' Para sair\n'm' Para abrir essa mensagem\n't' Para alterar o tamanho de cada geração de populações\n");
                printf("'p' Para alterar a porcentagem de mutação base\n's' Para imprimir a populacao atual\n'f' Para ligar/desligar a mutacao adaptativa\n");
                printf("'r' Para alterar o modo de reprodução\n'n' Para alterar o que o programa deve fazer\n'g' Para ligar/desligar o genocidio\n");
                printf("'c' Para ligar/desligar a predaçãon\n");

                break;

            case 'p': // Muda a taxa de mutação através da variavel global 
                printf("Insira o valor em porcentagem da nova taxa de mutação - Numero atual = %f\n",PORCENTAGEM_DE_MUTACAO);
                scanf("%lf ",&mediaefit.melhor);
                PORCENTAGEM_DE_MUTACAO=mediaefit.melhor;

                break;
            
            case 't': // Muda o tamanho de cada população 
                printf("Atenção: alterar o tamanho da população para maior vai gerar novos indivíduos aleatórios, já alterar para menor vai resguardar o melhor de todos na primeira posição e perder os últimos elementos\nTem Certeza que quer fazer isso? 1 para sim, outra coisa para não\n");
                temp=0;
                scanf(" %d",&temp);
                if(temp==1){
                    printf("Então insira novo numero de elementos da população - Numero atual = %d\n",TAMANHO_DA_POPULACAO);
                    scanf("%d",&temp);
                    populacao=redefine_tamanho(populacao,temp,qualfuncao,contadorpredacao,predacao);
                    
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

            case 'g': //Liga/desliga o genocidio

                 if(genocidio==FALSE){
                    genocidio=TRUE;
                    printf("O genocídio está agora LIGADO!\n");
                } 
                else{ 
                    genocidio=FALSE;
                    printf("O genocídio  está agora DESLIGADO!\n");
                }

            case 'c': //Liga/desliga a predacao

                 if(predacao==FALSE){
                    predacao=TRUE;
                    printf("A predacao está agora LIGADA!\n");
                    contadorpredacao=0;
                } 
                else{ 
                    predacao=FALSE;
                    printf("A predacao está agora DESLIGADa!\n");
                }
       
            case 'r': // Altera o modo de reprodução
                printf("Insira o modo da escolha de quem reproduz:\n1- O melhor tem filhos com todos os outros (elitismo)\n2- Torneio de 2\n Modo atual = %d\n",mododereproducao);
                while(loop==TRUE){
                scanf("%d",&mododereproducao);
                if(mododereproducao==ELITISMO){ printf("Modo selecionado: Elistimo\n"); loop=FALSE;}
                else if (mododereproducao==TORNEIODE2){ printf("Modo selecionado: Torneio de 2\n"); loop=FALSE;}
                else printf("Modo de reprodução não válido!\n");
                }
                loop=TRUE;
            break;

            case 'n':
                printf("Por favor, selecione a função que deseja procurar\n");
                printf("1- F(x)= 5-|x-42| - Obs: O melhor valor possível é X=42.00 pois y=0 e o mínimo é -infinito\n");
                printf("2- F(x)= (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500\n");
                while(loop==TRUE){
                scanf("%d",&qualfuncao);
                if(qualfuncao==1){ printf("Função selecionada: F(x)= 5-|x-42|\n"); loop=FALSE;}
                else if (qualfuncao==2){ printf("Função selecionada: F(x)= (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500\n"); loop=FALSE;}
                else printf("Função não válida!\n");
                }
                loop=TRUE;
                printf("Agora, digite 0 para buscar o mínimo ou outra coisa para buscar o máximo\n");
                scanf("%d",&temp);
                if(temp==0) minmax=MIN;
                else minmax=MAX;
            break;
            
            default: // Caso qualquer outra coisa seja inserida (um enter ['\n'], por exemplo) o laço é quebrado e uma nova geração é feita
                loop=FALSE;
                break;
            }
            }
            
        loop=TRUE;
        } // Final do laço geral do programa, basta saida ser 'q' para sair

        if(populacao!=NULL) free(populacao);

        fclose(plotgeracoes);
        fclose(plotmedia);
        fclose(plotfitmelhor);
        fclose(plotmelhor);

    return 0;
}