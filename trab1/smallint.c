/*Maria Carolina Marinho 1312063  */
/*Matheus de Sousa Suknaic 1413200 */

#include <stdio.h>
#include "smallint.h"


// Função para criar vetores de pequenos inteiros

VetSmallInt vs_new(int val[])
{
	int vsAux;                         // var auxiliar para criar vetor de pequeno inteiro
	int temOF;                         // variavel auxiliar para checar existencia de overflow
	int i;                             // contador
	VetSmallInt novo = 0;              // vetor de pequenos inteiros

    //Leremos cada um dos valores do vetor
	for(i=0;i<4;i++)
    {
		temOF = 0x01;
 
		if((val[i]<-32) || (val[i]>31))       // existe overflow para valores de inteiros que nao estao no intervalo de -32 a 31
		{
			temOF = temOF << (31-i);          // movimentamos o indicador para o espaço correspondente no SmallInt
			novo |= temOF;                    // insercao do valor no novo vetor de small int
		}

		vsAux = val[i] & 0x03F;                // mask para usarmos apenas 6 bits do valor

		vsAux = vsAux << (18-6*i);            // movimentamos para o espaco correspondente

		novo |= vsAux;                          // insercao no novo vetor
	}

	return novo;
}


// Função para imprimir vetores de pequenos inteiros

void vs_print(VetSmallInt v)
{
	char ehNeg, temOF, hexad;
	int i;                        // contador
	VetSmallInt vsAux = v;           
	
	printf("Overflow: ");
	
	//checagem da existencia de overflow no armazenamento
	for(i=0;i<4;i++)
	{
		temOF = vsAux>>(31-i);                // temOF armazena verificaçao da existencia de overflow

		if((temOF & 0x01) == (0x01))          // se no espaco visitado temOF armazenar 1, existe overflow
		{
			printf("Sim ");
		}
		else
		{
			printf("Nao ");
		}
	}
	
	printf("\n");

	printf("Valores: ");

	//impressao de valores
 	for(i=0; i<4; i++)
 	{
		ehNeg = (vsAux>>(18-6*i)) & 0x03F;        //com ehNeg iremos 
	 	hexad = ehNeg;

    // checando se é negativo

    if((ehNeg & 0x20) == (0x20))
		{
			ehNeg |= 0xC0;
			printf("%d (%x) ", ehNeg, hexad);
		}

		else
		{
 			printf ("%d (%x) ", hexad, hexad);
		}
 	}
 	printf("\n\n");
	return;
}



// Função para adicionar dois vetores de pequenos inteiros

VetSmallInt vs_add(VetSmallInt v1, VetSmallInt v2)
{
    int vet[4];
    VetSmallInt soma;
    unsigned char vet1, vet2;
    int i;
    
    for(i=0;i<4;i++)
    {
        //pegando os pequenos inteiros
        
        vet1 = (v1 >> (18-6*i)) & 0x3F;
        vet2 = (v2 >> (18-6*i)) & 0x3F;
        
        //iremos checar agora se o int é negativo
        
        if((vet1 & 0x20) == (0x20))
        {
            vet1 |= 0xFFFFFFC0;      //no caso de ser negativo, iremos corrigir seus bits
        }
        
        if((vet2 & 0x20) == (0x20))
        {
            vet2 |= 0xFFFFFFC0;
        }
        
        // fazemos a soma
        
        vet[i] = vet1 + vet2;
    }
    
    // criamos então o vetor de pequenos inteiros com a soma
    
    soma = vs_new(vet);
    
    return soma;
}


// Função para deslocar n bits para esquerda

VetSmallInt vs_shl(VetSmallInt v, int n)
{
    int i, vsAux;
    VetSmallInt deslocado = 0;
    
    // deslocaremos cada small int armazenado, um a um
   
		for(i=0; i<4; i++)
    {
        vsAux = (v >> (18- 6*i)) & 0x3F;        // primeiro iremos separar apenas o valor do small int        
        vsAux = (vsAux << n) & 0x3F;            // faremos um shift a esquerda em n, e depois iremos novamente separar apenas os 6 bits necessarios
        deslocado |= vsAux << (18-6*i);         // armazenamos o valor achado no novo vetor de small int 
    }
 
    return deslocado;
}



// Função para deslocar n bits para direita

VetSmallInt vs_shr(VetSmallInt v, int n){
	
    int i, vsAux;
    VetSmallInt deslocado = 0;
    
    // iremos deslocar n bits para cada small int

    for(i=0; i<4; i++)
    {
        vsAux = (v >> (18- 6*i)) & 0x3F;    // separamos apenas o valor do small int desejado
        vsAux = (vsAux >> n) & 0x3F;        // faremos o shift a direita em n, separando depois apenas os 6 bits desejados
        deslocado |= vsAux << (18-6*i);     // esses bits serao armazenados no novo vetor de small int
    }
    
    return deslocado;
}



// Função para deslocar n bits usando shift aritmetico
VetSmallInt vs_sar(VetSmallInt v, int n){
    
    int i;
    unsigned int vsAux;                     // shifts aritmeticos sao realizados quando ha unsigned int
    VetSmallInt deslocado = 0;
    
    // iremos fazer o deslocamento para cada small int
    
		for(i=0; i<4; i++)
    {
        vsAux = (v >> (18- 6*i)) & 0x3F;       // separamos apenas o valor do small int desejado
 
        // checagem de sinal do small int
        if((vsAux & 0x20)==(0x20))
        {
            vsAux=(vsAux |0xFFFFFFC0);                // no caso de ser negativo, iremos corrigir seus bits
        } 

        vsAux = (vsAux >> n) & 0x3f;            // apos verificacao, faremos o deslocamento  
        deslocado |= vsAux << (18-6*i);         // o small int deslocado sera armazenado em um novo vetor
    }
    
    return deslocado;
}



// Escrever arquivo passando o vetor de pequenos inteiros no formato Big Endian
int vs_write(VetSmallInt v ,FILE *f)
{
    int i;
    unsigned char vsAux, vsTeste = 0;

     for(i = 0; i<4; i++)
    { 
        vsAux = (v>>(24-8*i)); //capturando valores relacionados ao vet[i] armazenado  
   
        vsTeste = fputc(vsAux,f); //colocando em formato Big Endian os valores no arquivo

        if(vsTeste != vsAux) //Testando se houve erro na escrita
        {
            return -1;
        }     
    }

    return 0;
}



//Lendo o arquivo e passando suas informações para um VetSmallInt
VetSmallInt vs_read(FILE *f)
{
  int i;
  unsigned int vsAux,vsTeste;
  VetSmallInt novo;
	novo = 0;
  vsAux = 0;

  for(i=0; i<4; i++)
	{

		vsTeste = fgetc(f); //Tirando o byte do arquivo 
		
		if(vsTeste == EOF) //Testando se houve erro de leitura
		{
		  return 0xFFFFFFFF;
		}
		
		vsTeste = (vsTeste & 0xFF); //Deixando só o byte mais significativo

 		vsAux |= (vsTeste<<(24-8*i)); //Deslocando para a posição desejada
	
		novo |=vsAux; //Colocando no vetor de pequenos inteiros
	}

	return novo;
}






