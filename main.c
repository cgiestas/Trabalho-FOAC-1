#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHAS 50
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

//transforma o decimal para binario
void dec_para_bin(int decimal, char *binario_str, int num_bits){
    if (decimal < 0) {
        decimal = (1 << num_bits) + decimal;
    }

    binario_str[num_bits] = '\0'; 
    
    for (int i = num_bits - 1; i >= 0; i--) {
        binario_str[i] = (decimal % 2) + '0';
        decimal /= 2;
    }
}

void escrever_saida(const char* instrucao_binaria, FILE* saida) {
    fprintf(saida, "%s\n", instrucao_binaria);
}

//função que le o arquivo recebido e armazena as instruções numa string
void le_arquivo(FILE *entrada, char guardaString[MAX_LINHAS][MAX_CHAR]){
    int i = 0;
    while(i < MAX_LINHAS && fgets(guardaString[i], MAX_CHAR, entrada) != NULL){
        guardaString[i][strcspn(guardaString[i], "\n")] = '\0';
        i++;
    }
}

//atribuindo os tipos (R, I, S ou B) e passando os valores em DECIMAL 
void atribuidor(char *instrucao, char *op0, char *op1, char *op2, FILE* arquivo_saida){
    char instrucao_final[33];   //buffer para a string binária completa (32 chars + '\0')
    instrucao_final[0] = '\0';  //inicializando como vazia

    //atributos binarios a serem usados
    char bin_opcode[8], bin_rd[6], bin_rs1[6], bin_rs2[6], bin_funct3[4], bin_funct7[8], bin_imm[13];

    //caso tipo R: add, and e sll
    if(strcmp(instrucao, "add") == 0 || strcmp(instrucao, "and") == 0 || strcmp(instrucao, "sll") == 0){
        R_tipo inst; //"inst" é abreviação de instrucao kkkk
        inst.opcode = 51;
        inst.funct7 = 0;
        inst.rd = atoi(op0 + 1);
        inst.rs1 = atoi(op1 + 1);
        inst.rs2 = atoi(op2 + 1); //a função atoi remove o "x" em, por exemplo, "x5"

        //identificando qual instrução do tipo R ela é, determinada pelo funct3
        if (strcmp(instrucao, "add") == 0) inst.funct3 = 0;
        if (strcmp(instrucao, "and") == 0) inst.funct3 = 7;
        if (strcmp(instrucao, "sll") == 0) inst.funct3 = 1;

        //transformando para binario
        dec_para_bin(inst.opcode, bin_opcode, 7);
        dec_para_bin(inst.rd,     bin_rd,     5);
        dec_para_bin(inst.rs1,    bin_rs1,    5);
        dec_para_bin(inst.rs2,    bin_rs2,    5);
        dec_para_bin(inst.funct3, bin_funct3, 3);
        dec_para_bin(inst.funct7, bin_funct7, 7);

        //formato R: funct7 + rs2 + rs1 + funct3 + rd + opcode
        sprintf(instrucao_final, "%s%s%s%s%s%s", bin_funct7, bin_rs2, bin_rs1, bin_funct3, bin_rd, bin_opcode);

    }
    else if (strcmp(instrucao, "sb") == 0) {
        S_tipo inst;
        inst.opcode = 35; 
        inst.funct3 = 0;  
        inst.rs2 = atoi(op0 + 1);
        inst.rs1 = atoi(op1 + 1);
        inst.imediate = atoi(op2);

        //o imediato de 12 bits é quebrado em duas partes
        int imm11_5 = (inst.imediate >> 5) & 0x7F; // 7 bits mais significativos
        int imm4_0  = inst.imediate & 0x1F;       // 5 bits menos significativos

        char bin_imm11_5[8], bin_imm4_0[6];
        dec_para_bin(inst.opcode, bin_opcode, 7);
        dec_para_bin(inst.rs1, bin_rs1, 5);
        dec_para_bin(inst.rs2, bin_rs2, 5);
        dec_para_bin(inst.funct3, bin_funct3, 3);
        dec_para_bin(imm11_5, bin_imm11_5, 7);
        dec_para_bin(imm4_0, bin_imm4_0, 5);

        // Formato S: imm[11:5] + rs2 + rs1 + funct3 + imm[4:0] + opcode
        sprintf(instrucao_final, "%s%s%s%s%s%s", bin_imm11_5, bin_rs2, bin_rs1, bin_funct3, bin_imm4_0, bin_opcode);
    }
    
    //ana: implementar as instruções do tipo I e B

    //pega a instrução final e escreve no arquivo de saida
    if (strlen(instrucao_final) > 0) {
        escrever_saida(instrucao_final, arquivo_saida);
    }
}

//quebrando a guardaString em tokens para identificar qual é a instrução
void separador(char guardaString[MAX_LINHAS][MAX_CHAR], FILE* arquivo_saida){
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

       if (instrucao && op0 && op1 && op2) {
            atribuidor(instrucao, op0, op1, op2, arquivo_saida);
        }
    }
}

int main(int argc, char *argv[]) {
    //previnindo erro: caso o tipo do arquivo (asm) não for especificado
    if (argc < 2) {
        fprintf(stderr, "uso: %s <arquivo_entrada.asm> [-o <arquivo_saida>]\n", argv[0]);
        return 1; 
    }

    FILE *entrada = fopen(argv[1], "r");
    if (entrada == NULL) {
        perror("erro ao abrir o arquivo de entrada :(");
        return 1;
    }

    FILE *saida = stdout; //padrão: saída no terminal
    if (argc == 4 && strcmp(argv[2], "-o") == 0) {
        saida = fopen(argv[3], "w");
        if (saida == NULL) {
            perror("erro ao criar o arquivo de saída :(");
            fclose(entrada);
            return 1;
        }
    }

    char guardaString[MAX_LINHAS][MAX_CHAR] = {0}; 

    le_arquivo(entrada, guardaString);
    separador(guardaString, saida);

    printf("Montagem concluida com sucesso :D\n");
    fclose(entrada);
    if (saida != stdout) {
        fclose(saida);
    }

    return 0;
}