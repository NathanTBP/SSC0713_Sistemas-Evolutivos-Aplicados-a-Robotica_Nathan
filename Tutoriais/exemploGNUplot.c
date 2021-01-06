//Bibliotecas padrão
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinyexpr.h"

int main(void){

    FILE* meuarquivo;
    FILE* gnuplot;
    FILE* comandos;

    meuarquivo=fopen("meuarquivo.txt","w");
    comandos=popen("export DISPLAY=localhost:0","w");
    fclose(comandos);
    gnuplot=popen("gnuplot -persist","w");
    
    fprintf(meuarquivo,"1 1\n2 2\n3 3\n5 5\n8 8\n13 13\n");
    fflush(meuarquivo);

    fprintf(gnuplot,"set title 'Fibonacci e seno'\nset key above\nset xlabel 'Valores de x'\nset ylabel 'Valores de y'\n");
    fprintf(gnuplot,"plot 'meuarquivo.txt' title 'Fibo' with lines, sin(x) title 'Sen' with lines \n");

    return 0;
}