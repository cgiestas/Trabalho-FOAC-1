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
    char bin_imm12[2], bin_imm10_5[7], bin_imm4_1[5], bin_imm11[2]; // Para B-type

    //caso tipo R: add, and e sll
    if(strcmp(instrucao, "add") == 0 || strcmp(instrucao, "and") == 0 || strcmp(instrucao, "sll") == 0){
        R_tipo inst; //"inst" é abreviação de instrucao kkkk
        inst.opcode = 51; // 0110011
        inst.funct7 = 0;  // 0000000
        inst.rd = atoi(op0 + 1);
        inst.rs1 = atoi(op1 + 1);
        inst.rs2 = atoi(op2 + 1); //a função atoi remove o "x" em, por exemplo, "x5"

        //identificando qual instrução do tipo R ela é, determinada pelo funct3
        if (strcmp(instrucao, "add") == 0) inst.funct3 = 0; // 000
        else if (strcmp(instrucao, "and") == 0) inst.funct3 = 7; // 111
        else if (strcmp(instrucao, "sll") == 0) inst.funct3 = 1; // 001

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
        inst.opcode = 35; // 0100011
        inst.funct3 = 0;  // 000
        inst.rs2 = atoi(op0 + 1); // rs2 é o primeiro operando aqui (registrador a ser salvo)

        char base_reg_str[10];
        sscanf(op1, "%d(%s)", &inst.imediate, base_reg_str);
        
        // Remove o parêntese final ')' da string do registrador
        base_reg_str[strlen(base_reg_str) - 1] = '\0'; 
        inst.rs1 = atoi(base_reg_str + 1); // Converte o registrador base

        // O imediato de 12 bits é quebrado em duas partes para o Formato S
        // imm[11:5] -> 7 bits mais significativos
        int imm11_5 = (inst.imediate >> 5) & 0x7F; // 0x7F é a máscara para 7 bits (1111111)
        
        // imm[4:0] -> 5 bits menos significativos
        int imm4_0  = inst.imediate & 0x1F;       // 0x1F é a máscara para 5 bits (0011111)

        char bin_imm11_5_s[8], bin_imm4_0_s[6]; // Renomeado para evitar conflito com B-type
        dec_para_bin(inst.opcode, bin_opcode, 7);
        dec_para_bin(inst.rs1, bin_rs1, 5);
        dec_para_bin(inst.rs2, bin_rs2, 5);
        dec_para_bin(inst.funct3, bin_funct3, 3);
        dec_para_bin(imm11_5, bin_imm11_5_s, 7);
        dec_para_bin(imm4_0, bin_imm4_0_s, 5);

        // Formato S: imm[11:5] + rs2 + rs1 + funct3 + imm[4:0] + opcode
        sprintf(instrucao_final, "%s%s%s%s%s%s", bin_imm11_5_s, bin_rs2, bin_rs1, bin_funct3, bin_imm4_0_s, bin_opcode);
    }
    // Implementação das instruções do Tipo I (lb, ori) e Tipo B (bne)
    // Este bloco lida com a decodificação e montagem das instruções 'lb', 'ori' e 'bne'.
    // Cada instrução é mapeada para seu opcode e funct3 correspondente,
    // e os operandos (registradores e imediatos) são extraídos e convertidos para binário.
    // O imediato das instruções de tipo B tem um tratamento especial de reordenação de bits.
    else if (strcmp(instrucao, "lb") == 0) {
        I_tipo inst;
        inst.rd = atoi(op0 + 1);
        char base_reg_str[10]; // String para pegar o nome do registrador
        sscanf(op1, "%d(%s)", &inst.imediate, base_reg_str); // Analisa a string no formato "numero(string)"
        
        // Remove o parêntese final ')' da string do registrador
        base_reg_str[strlen(base_reg_str) - 1] = '\0'; 
        inst.rs1 = atoi(base_reg_str + 1); // Converte o registrador base
        
        inst.opcode = 3; // Opcode para Load (0000011)
        inst.funct3 = 0; // Funct3 para lb (000)

        // Converte os campos para suas representações binárias
        dec_para_bin(inst.opcode, bin_opcode, 7);
        dec_para_bin(inst.rd, bin_rd, 5);
        dec_para_bin(inst.rs1, bin_rs1, 5);
        dec_para_bin(inst.funct3, bin_funct3, 3);
        dec_para_bin(inst.imediate, bin_imm, 12); // Imediato de 12 bits

        // Monta a instrução final no formato I-type: immediate[11:0] | rs1 | funct3 | rd | opcode
        sprintf(instrucao_final, "%s%s%s%s%s", bin_imm, bin_rs1, bin_funct3, bin_rd, bin_opcode);
    }
    else if(strcmp(instrucao, "ori") ==0){
        I_tipo inst;
        inst.rd = atoi(op0 + 1); // Registrador de destino (rd)
        inst.rs1 = atoi(op1 + 1); // Registrador de origem 1 (rs1)
        inst.imediate = atoi(op2); // Valor imediato de 12 bits
        inst.opcode = 19; // Opcode para Immediate (0010011)
        inst.funct3 = 6; // Funct3 para ori (110)
    

        // Converte os campos para suas representações binárias
        dec_para_bin(inst.opcode, bin_opcode, 7);
        dec_para_bin(inst.rd, bin_rd, 5);
        dec_para_bin(inst.rs1, bin_rs1, 5);
        dec_para_bin(inst.funct3, bin_funct3, 3);
        dec_para_bin(inst.imediate, bin_imm, 12); // Imediato de 12 bits

        // Monta a instrução final no formato I-type: immediate[11:0] | rs1 | funct3 | rd | opcode
        sprintf(instrucao_final, "%s%s%s%s%s", bin_imm, bin_rs1, bin_funct3, bin_rd, bin_opcode);
    }
    else if (strcmp(instrucao, "bne") == 0) {
        B_tipo inst;
        inst.opcode = 99; // Opcode para Branch (1100011)
        inst.funct3 = 1;  // Funct3 para bne (001)
        inst.rs1 = atoi(op0 + 1); // Registrador de origem 1 (rs1)
        inst.rs2 = atoi(op1 + 1); // Registrador de origem 2 (rs2)
        
        // O imediato de branch é um offset de 12 bits assinado, mas o bit menos significativo (bit 0) é implícito e sempre 0.
        // A codificação RISC-V armazena os 12 bits mais significativos de um imediato de 13 bits (imm[12:1]).
        // Portanto, o valor do imediato lido do .asm (que é o offset completo) é dividido por 2
        // para obter os bits que serão codificados na instrução.
        inst.imediate = atoi(op2); 
        int immediate_encoded = inst.imediate / 2; 

        // Extrai os diferentes segmentos do imediato para o formato B-type,
        // que possui uma reordenação específica dos bits do imediato (imm[12|10:5] e imm[4:1|11]).
        int imm12_b   = (immediate_encoded >> 11) & 0x1;    // Bit 12 (bit de sinal)
        int imm10_5_b = (immediate_encoded >> 4) & 0x3F;    // Bits 10 a 5
        int imm4_1_b  = (immediate_encoded >> 0) & 0xF;     // Bits 4 a 1
        int imm11_b   = (immediate_encoded >> 10) & 0x1;    // Bit 11

        // Converte os campos para suas representações binárias
        dec_para_bin(inst.opcode, bin_opcode, 7);
        dec_para_bin(inst.rs1, bin_rs1, 5);
        dec_para_bin(inst.rs2, bin_rs2, 5);
        dec_para_bin(inst.funct3, bin_funct3, 3);
        
        // Converte os segmentos do imediato para binário
        dec_para_bin(imm12_b, bin_imm12, 1);
        dec_para_bin(imm10_5_b, bin_imm10_5, 6);
        dec_para_bin(imm4_1_b, bin_imm4_1, 4);
        dec_para_bin(imm11_b, bin_imm11, 1);

        // Monta a instrução final no formato B-type, seguindo a ordem específica dos bits:
        // imm[12] + imm[10:5] + rs2 + rs1 + funct3 + imm[4:1] + imm[11] + opcode
        sprintf(instrucao_final, "%s%s%s%s%s%s%s%s", 
            bin_imm12, bin_imm10_5, bin_rs2, bin_rs1, bin_funct3, bin_imm4_1, bin_imm11, bin_opcode);
    }
    
    //pega a instrução final e escreve no arquivo de saida
    if (strlen(instrucao_final) > 0) {
        escrever_saida(instrucao_final, arquivo_saida);
    } else {
        // Se a instrução não foi reconhecida, você pode adicionar um erro aqui
        fprintf(stderr, "Erro: Instrucao '%s' nao reconhecida ou nao implementada para o grupo 1.\n", instrucao);
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

        // Se encontramos pelo menos uma instrução, chame o atribuidor.
        // O atribuidor será responsável por verificar se os operandos necessários existem.
        if (instrucao != NULL) {
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