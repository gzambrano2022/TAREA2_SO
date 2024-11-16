#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAMES 10
#define MAX_PAGES 100

typedef struct Page {
    int number;
    struct Page* next;
} Page;

typedef struct {
    Page* head;
} HashTable;

HashTable* createHashTable() {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable) * MAX_FRAMES);
    for (int i = 0; i < MAX_FRAMES; i++) {
        table[i].head = NULL;
    }
    return table;
}

int hashFunction(int pageNumber) {
    return pageNumber % MAX_FRAMES;
}

void insertPage(HashTable* table, int pageNumber) {
    int hashIndex = hashFunction(pageNumber);
    Page* newPage = (Page*)malloc(sizeof(Page));
    newPage->number = pageNumber;
    newPage->next = table[hashIndex].head;
    table[hashIndex].head = newPage;
}

int searchPage(HashTable* table, int pageNumber) {
    int hashIndex = hashFunction(pageNumber);
    Page* current = table[hashIndex].head;
    while (current != NULL) {
        if (current->number == pageNumber) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void freeHashTable(HashTable* table) {
    for (int i = 0; i < MAX_FRAMES; i++) {
        Page* current = table[i].head;
        while (current != NULL) {
            Page* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(table);
}

int FIFO(int frames[], int numFrames, int pages[], int numPages) {
    int pageFaults = 0;
    int index = 0;
    for (int i = 0; i < numPages; i++) {
        int found = 0;
        for (int j = 0; j < numFrames; j++) {
            if (frames[j] == pages[i]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            frames[index] = pages[i];
            index = (index + 1) % numFrames;
            pageFaults++;
        }
    }
    return pageFaults;
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Uso: %s -m <num_marcos> -a <algoritmo> -f <archivo_referencias>\n", argv[0]);
        return 1;
    }

    int numFrames = atoi(argv[2]);
    char* algorithm = argv[4];
    char* filename = argv[6];

    int frames[MAX_FRAMES];
    for (int i = 0; i < numFrames; i++) {
        frames[i] = -1;
    }

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("No se puede abrir el archivo %s\n", filename);
        return 1;
    }

    int pages[MAX_PAGES];
    int numPages = 0;
    while (fscanf(file, "%d", &pages[numPages]) != EOF) {
        numPages++;
    }
    fclose(file);

    int pageFaults = 0;
    if (strcmp(algorithm, "FIFO") == 0) {
        pageFaults = FIFO(frames, numFrames, pages, numPages);
    } else {
        printf("Algoritmo no soportado\n");
        return 1;
    }

    printf("Número de fallos de página: %d\n", pageFaults);
    return 0;
}
