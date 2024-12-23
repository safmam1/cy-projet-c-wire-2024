# Définir le compilateur C
CC = gcc

# Définir les options de compilation
CFLAGS = -Wall -Wextra -O2

# La cible principale pour construire le programme
all: cwire

# Règle pour compiler le programme 'cwire' à partir de 'cwire.c'
cwire: cwire.c
	$(CC) $(CFLAGS) -o cwire cwire.c

# Règle pour nettoyer les fichiers compilés et les binaires
clean:
	rm -f cwire