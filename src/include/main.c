#include <stdio.h>
#include <stdlib.h>

#define MAX_LINHAS 10
#define MAX_CHAR 100

//preciso: lb, sb, add, and,  ori, sll, bne

typedef struct {
    int opcode, rd, rs1, rs2, funct3, funct7;
}R_tipo; //add, and, sll

typedef struct{
    int opcode, rd, rs1, funct3, imediate;
}I_tipo; //lb, ori

typedef struct{
    int opcode, rs1, rs2, funct3, imediate;
}S_tipo; //sb

typedef struct{
    int opcode, rs1, rs2, funct3, imediate;
}B_tipo; //bne

//função que le o arquivo recebido e armazena as instruções numa string
void le_arquivo(FILE *entrada, char guardaString[MAX_LINHAS][MAX_CHAR]){
    entrada = fopen("exemplo.asm", "r");
    if(entrada == NULL){
        printf("erro ao abrir o arquivo :(");
        return;
    }

    int i = 0;
    while(i < MAX_LINHAS && fgets(guardaString, MAX_CHAR, entrada) != NULL){
        guardaString[i][strcspn(guardaString[i], "\n")] = '\0';
        i++;
    }

    fclose(entrada);
}

void atribuidor(char *instrucao, char *op0, char *op1, char *op2);

//quebrando a guardaString em tokens para identificar qual é a instrução
void separador(char guardaString[MAX_LINHAS][MAX_CHAR]){
    for (int i = 0; i < MAX_LINHAS; i++) {
        //verifica se a linha está vazia (fim das instruções)
        if (strlen(guardaString[i]) == 0) continue;

        //copia a linha para uma string temporária, pois strtok altera a original
        char linha[MAX_CHAR];
        strcpy(linha, guardaString[i]);

        //separando os tokens
        char *instrucao = strtok(linha, " ,");
        char *op0 = strtok(NULL, " ,");
        char *op1 = strtok(NULL, " ,");
        char *op2 = strtok(NULL, " ,");

        atribuidor(instrucao, op0, op1, op2);
    }
}

//atribuindo os tipos (R, I, S ou B) e passando os valores em DECIMAL 
void atribuidor(char *instrucao, char *op0, char *op1, char *op2){
    if(strcmp(instrucao, "add") == 0){
        R_tipo inst; //"inst" é abreviação de instrucao kkkk
        inst.opcode = 51;
        inst.funct3 = 0;
        inst.funct7 = 0;
        inst.rd = atoi(op0 + 1);
        inst.rs1 = atoi(op1 + 1);
        inst.rs2 = atoi(op2 + 1);
    }

    //a função atoi remove o "x" em, por exemplo, "x5"

    if(strcmp(instrucao, "and") == 0){
        R_tipo inst;
        inst.opcode = 51;
        inst.funct3 = 7;
        inst.funct7 = 0;
        inst.rd = atoi(op0 + 1);
        inst.rs1 = atoi(op1 + 1);
        inst.rs2 = atoi(op2 + 1);
    }

    if(strcmp(instrucao, "sll") == 0){
        R_tipo inst; 
        inst.opcode = 51;
        inst.funct3 = 1;
        inst.funct7 = 0;
        inst.rd = atoi(op0 + 1);
        inst.rs1 = atoi(op1 + 1);
        inst.rs2 = atoi(op2 + 1);
    }

    if(strcmp(instrucao, "bne") == 0){
        B_tipo inst; 
        inst.opcode =  99;
        inst.funct3 = 1;
        inst.rs1 = atoi(op1 + 1);
        inst.rs2 = atoi(op2 + 1);
        //inst.imediate = entender como funciona o imediate e implementar 
    }

    //ana: implementar as instruções do tipo I e S

}

//implementar função dec_para_bin 

int main(){
   char guardaString[MAX_LINHAS][MAX_CHAR];
    //chamar funções
}