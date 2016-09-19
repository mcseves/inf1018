/* Maria Carolina Marinho 1312063  */
/* Matheus de Sousa Suknaic 1413200 */


#include "cria_func.h"

void libera_func (void* func) {
    free(func);
}

void *cria_func(void *f,DescParam params[], int n)
{
    unsigned int *aux;
    int cont,i,j,qtd;
    unsigned char inicio[3]={0x55,0x89,0xe5}; // dando push %ebp, passando %esp para %ebp
    unsigned char final[4]={0x89,0xec,0x5d,0xc3}; //movendo %ebp para %esp, dando pop %ebp e ret
    unsigned char byte;
    unsigned char *nova = (unsigned char *)malloc(sizeof(unsigned char)*512); //nova funcao a ser retornada

    qtd = 0;

    for(i=0; i<n; i++) //contando a quantidade de parametros nao amarrados
    {
        if(params[i].orig_val == PARAM)
        {
            if(params[i].tipo_val == DOUBLE_PAR)
            {
                qtd = qtd+2;
            }
            else
            {
                qtd++;
            }
        }
    }

    qtd = 8 + (qtd*4);
    
    
    //passando o codigo do inicio da funcao
    for(i=0; i<3; i++)
    {
        nova[i] = inicio[i];
    }


    //percorrendo o vetor params
    for(j=n-1; j>=0; j--)
    {
        // Se o valor for fixado diretamente
        if(params[j].orig_val == FIX_DIR)
        {
            nova[i] = 0xba; //mov %edx
            i++;
            
            // caso int
            if(params[j].tipo_val == INT_PAR)
            {
                aux = (unsigned int *) &(params[j].valor.v_int);
            }

            else
            {
                // caso float
                if(params[j].tipo_val == FLOAT_PAR)
                {
                    aux = (unsigned int*) &(params[j].valor.v_float);
                }

                else
                {
                    // caso ponteiro
                    if(params[j].tipo_val==PTR_PAR)
                    {
                        aux = (unsigned int *) &(params[j].valor.v_ptr);
                    }
                    
                    // caso double
                    else
                    {
                        aux= (unsigned int*)&(params[j].valor.v_double);

                        for(cont=0; cont<4; cont++,i++)
                        {
                            byte = *(aux+1)>>(8*cont);
                            byte &= 0XFF;
                            nova[i] = byte;
                        }

                        nova[i] = 0x52; //push %edx
                        nova[i+1] = 0xba; //mov %edx

                        i = i + 2;
                    }
                }
            }

            for(cont=0; cont<4; cont++,i++)
            {
                byte = *(aux)>>(8*cont);
                byte&=0XFF; //pegando byte a byte
                nova[i] = byte; // passando para o codigo da nova funcao
            }

            nova[i] = 0x52; //push %edx

            i++;

        }
        else
        {
            // Se o valor for fixado indiretamente
            if(params[j].orig_val == FIX_IND)
            {

                aux = (unsigned int*) &params[j].valor.v_ptr;

                nova[i] = 0xba; //mov %edx
                i++;
                
                for(cont=0; cont<4; cont++,i++)
                {
                    byte = *(aux)>>(8*cont);
                    byte &= 0XFF;
                    nova[i] = byte;
                }
                
                // caso double
                if(params[j].tipo_val==DOUBLE_PAR)
                {
                    nova[i]=0xff;
                    nova[i+1]=0x72;
                    nova[i+2]=0x04;
                    i=i+3;
                }

                nova[i] = 0xff;
                nova[i+1] = 0x32;
                i = i + 2;
            }
            // se for passado por fora
            else
            {
                qtd = qtd - 4;
                nova[i] = 0xff;
                nova[i+1] = 0x75;
                nova[i+2]= qtd;
                i=i+3;
                
                // caso double
                if(params[j].tipo_val == DOUBLE_PAR)
                {
                    qtd = qtd - 4;
                    nova[i] = 0xff;
                    nova[i+1] = 0x75;
                    nova[i+2]= qtd;
                    i=i+3;
                }
            }
        }
    }

    nova[i] = 0xb8; //mov %eax
    i++;
    aux = (unsigned int*) &(f);

    for(cont=0; cont<4; cont++,i++) //passando o codigo da chamada da funcao
    {
        byte = *(aux)>>(8*cont);
        byte &= 0xff;
        nova[i] = byte;
    }

    nova[i] = 0xff; //fazendo call *eax nessas duas linhas
    nova[i+1] = 0xd0;
    i=i+2;

    for(cont=0; cont<4; cont++,i++) //passando o codigo da finalizacao da funcao
    {
        nova[i] = final[cont];
    }

    
    for(cont=0;cont<i;cont++)
    {
        printf("%02x ",nova[cont]);
    }

    return nova;
}
