# Define o compilador que será usado
CC = gcc

# Define as flags de compilação:
# -Wall: Mostra todos os avisos (warnings)
# -g:    Inclui informações de depuração.
# -std=c11: Garante que o código segue o padrão C11.
CFLAGS = -Wall -g -std=c11

# Define o nome do arquivo de código-fonte
SRCS = main.c

# Define o nome do programa executável final
TARGET = montador

# Regra padrão: é executada quando você digita apenas "make"
# Ela diz que para criar tudo ("all"), o objetivo é criar o programa $(TARGET)
all: $(TARGET)

# Regra principal para criar o programa executável
# Diz que o "TARGET" depende dos arquivos em "SRCS"
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Regra "clean": para limpar os arquivos gerados pela compilação
# .PHONY evita que o make confunda o "clean" com um arquivo de mesmo nome
.PHONY: clean
clean:
	-del /f $(TARGET).exe
	-rm -f $(TARGET)