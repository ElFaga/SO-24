#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 100

// Estrutura para os parâmetros das threads
struct ThreadData{
    int* numbers;
    int start;
    int end;
    int result;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Função para calcular a soma de um subvetor
void* sum_subvector(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    int sum = 0;
    for (int i = data->start; i < data->end; i++) {
        sum += data->numbers[i];
    }

    pthread_mutex_lock(&mutex);
    data->result = sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char* argv[]) {
    // Verifica a quantidade de arquivos para a execução
    if (argc != 3) {
        printf("Uso: %s <nome_do_arquivo> <numero_de_threads>\n", argv[0]);
        return 1;
    }

	// Verifica se o número de threads é válido
    char* file_name = argv[1];
    int num_threads = atoi(argv[2]);
    if (num_threads <= 0 || num_threads > MAX_THREADS) {
        printf("Invalid number of threads.\n");
        return 1;
    }

    // Abertura do arquivo caso seja válido
    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Verifica o tamanho do arquivo e a quantidade de numeros
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    int num_integers = file_size / sizeof(int);
    rewind(file);

    // Aloca memória para os números
    int* numbers = (int*)malloc(num_integers * sizeof(int));
    if (numbers == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // Lê os números presentes no arquivo
    fread(numbers, sizeof(int), num_integers, file);
    fclose(file);

    // Cria as threads
    pthread_t threads[num_threads];
    struct ThreadData thread_data[num_threads];
    int subvector_size = num_integers / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].numbers = numbers;
        thread_data[i].start = i * subvector_size;
        thread_data[i].end = (i == num_threads - 1) ? num_integers : (i + 1) * subvector_size;
        pthread_create(&threads[i], NULL, sum_subvector, (void*)&thread_data[i]);
    }

    // Aguarda as threads finalizarem
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Calcula a soma total
    int total_sum = 0;
    for (int i = 0; i < num_threads; i++) {
        total_sum += thread_data[i].result;
    }

    // Apresenta o resultado
    printf("Total sum: %d\n", total_sum);

    // Limpa memória alocada
    free(numbers);

    return 0;
}

