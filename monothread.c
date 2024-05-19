#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PROCESSES 100

// Estrutura para os parâmetros dos processos
struct ProcessArgs {
    int *numbers; 
    int start;     
    int end;       
};

// Função para calcular a soma de um subvetor
int calculate_sum(int *numbers, int start, int end) {
    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += numbers[i];
    }
    return sum;
}

// Função para calcular a soma total das somas parciais
int calculate_total_sum(int num_processes, int *sums) {
    int total_sum = 0;
    for (int i = 0; i < num_processes; i++) {
        total_sum += sums[i];
    }
    return total_sum;
}

// Função para calcular a soma total de todos os elementos das somas parciais
int calculate_final_sum(int num_processes, struct ProcessArgs *args) {
    int final_sum = 0;
    for (int i = 0; i < num_processes; i++) {
        final_sum += calculate_sum(args[i].numbers, args[i].start, args[i].end);
    }
    return final_sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <nome_arquivo> <quantidade_processos>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Abrir o arquivo
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return EXIT_FAILURE;
    }

    // Calcular o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Calcular a quantidade de inteiros no arquivo
    int num_integers = file_size / sizeof(int);

    // Ler os números do arquivo para um vetor
    int *numbers = (int *)malloc(num_integers * sizeof(int));
    fread(numbers, sizeof(int), num_integers, file);
    fclose(file);

    // Número de processos
    int num_processes = atoi(argv[2]);
    if (num_processes <= 0 || num_processes > MAX_PROCESSES) {
        printf("Número inválido de processos\n");
        return EXIT_FAILURE;
    }

    // Criar processos
    int pid;
    int status;
    struct ProcessArgs args[num_processes];
    int sums[num_processes]; // Array para armazenar as somas parciais de cada processo

    // Distribuir os números entre os processos
    int nums_per_process = num_integers / num_processes;
    int remainder = num_integers % num_processes;
    int index = 0;
    for (int i = 0; i < num_processes; i++) {
        args[i].numbers = numbers;
        args[i].start = index;
        args[i].end = index + nums_per_process + (i < remainder ? 1 : 0);
        index = args[i].end;

        // Criar um novo processo
        pid = fork();

        if (pid < 0) {
            perror("Erro ao criar processo");
            return EXIT_FAILURE;
        } else if (pid == 0) {  // Processo filho
            int sum = calculate_sum(args[i].numbers, args[i].start, args[i].end);
            printf("Processo filho %d: Soma parcial: %d\n", getpid(), sum);
            exit(sum); // Retorna a soma parcial como resultado do processo filho
        } else { // Processo pai
            // Aguarda o processo filho terminar e armazena sua soma parcial
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                sums[i] = WEXITSTATUS(status); // Armazena a soma parcial retornado pelo processo filho
            } else {
                printf("Processo filho terminou inesperadamente\n");
            }
        }
    }

    // Calcular a soma total de todos os elementos das somas parciais
    int final_sum = calculate_final_sum(num_processes, args);

    // Imprimir o resultado
    printf("Soma total: %d\n", final_sum);

    // Liberar a memória alocada
    free(numbers);

    return EXIT_SUCCESS;
}

