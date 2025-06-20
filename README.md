Mini Montador RISC-V em C

  Este é um projeto acadêmico que consiste em um montador simples para um subconjunto da arquitetura RISC-V de 32 bits (RV32I). 
  O programa foi desenvolvido em C e é capaz de converter um arquivo de texto contendo instruções assembly RISC-V para seu respectivo código de máquina em formato binário de 32 bits.

Funcionalidades
  Converte código assembly de um arquivo de entrada para código de máquina binário.
  Suporta os formatos de instrução R, I, S e B.
  Realiza a análise (parsing) de sintaxes diferentes para instruções lógicas/aritméticas e de acesso à memória.
  A saída pode ser exibida no terminal ou salva em um arquivo de destino.
Instruções Suportadas
  O montador atualmente suporta as seguintes instruções:

| Instrução | Tipo | Formato Esperado no Código |
| :-------- | :--: | :------------------------- |
| add     |  R   | add rd, rs1, rs2         |
| sll     |  R   | sll rd, rs1, rs2         |
| and     |  R   | and rd, rs1, rs2         |
| ori     |  I   | ori rd, rs1, imediato    |
| lb      |  I   | lb rd, offset(rs1)       |
| sb      |  S   | sb rs2, offset(rs1)      |
| bne     |  B   | bne rs1, rs2, imediato   |

Como Compilar e Executar

Pré-requisitos
  Um compilador C, como o gcc.
Compilação
  O arquivo Makefile possibilita compilar o código apenas digitando "make" no seu terminal (sem as aspas).
  Isso irá gerar um arquivo executável chamado montador.
  
Execução
  O programa pode ser executado de duas formas:
  
  Exibindo a saída no terminal: ./montador seu_arquivo.asm
  Salvando a saída em um arquivo: ./montador seu_arquivo.asm -o saida.bin

Formato do Código Assembly de Entrada
  Para que o montador funcione corretamente, o arquivo .asm de entrada deve seguir as seguintes convenções:
  
  1.Uma instrução por linha.
  2.Os operandos devem ser separados por vírgulas e/ou espaços.
  3.Os registradores devem ser nomeados no formato xN (ex: x5, x10).
  4.Instruções de Load/Store (lb, sb): Devem usar a sintaxe padrão offset(registrador_base), como em lb x10, 16(x11).
  5.Instruções de Branch (bne): O último operando deve ser um número inteiro (o deslocamento), e não um rótulo.
