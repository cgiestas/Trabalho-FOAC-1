#include <stdio.h>

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

void conversor(char guardaString[MAX_LINHAS][MAX_CHAR]){
    
}

int main(){
   char guardaString[MAX_LINHAS][MAX_CHAR];

}