#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <atomic>
#include <chrono>
#include <cstdlib> // Para rand()
#include <random>  // Para mejores números aleatorios
#include <unistd.h> // Para getopt()

class CircularQueue {
private:
    std::vector<int> queue;
    size_t head, tail, size, capacity;
    std::mutex mtx;
    std::condition_variable not_full, not_empty;
    std::ofstream log_file; 
    std::chrono::steady_clock::time_point start_time;

public:
    CircularQueue(size_t initial_capacity) 
        : head(0), tail(0), size(0), capacity(initial_capacity), queue(initial_capacity) {
            log_file.open("capacity.log");
            if (!log_file.is_open()) {
                std::cerr << "Error al abrir el archivo capacity.log" << std::endl;
            }
            start_time = std::chrono::steady_clock::now();
        }

    ~CircularQueue() { 
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    void enqueue(int item) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this]() { return size < capacity; });

        queue[tail] = item;
        tail = (tail + 1) % capacity;
        size++;

        if (size == capacity) {
            doubleCapacity();
        }

        logCapacity(); // Registrar capacidad después de encolar
        not_empty.notify_one();
    }

    int dequeue() {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this]() { return size > 0; });

        int item = queue[head];
        head = (head + 1) % capacity;
        size--;

        if (size < capacity / 4) {
            halveCapacity();
        }

        logCapacity(); // Registrar capacidad después de desencolar
        not_full.notify_one();
        return item;
    }

    bool isEmpty() {
        std::lock_guard<std::mutex> lock(mtx);
        return size == 0;
    }

private:
    void doubleCapacity() {
        std::cout << "Doblando la capacidad de la cola." << std::endl; 
        std::vector<int> new_queue(capacity * 2);
        for (size_t i = 0; i < size; i++) {
            new_queue[i] = queue[(head + i) % capacity];
        }
        queue = std::move(new_queue);
        head = 0;
        tail = size;
        capacity *= 2;
        logCapacity(); // Registrar capacidad después de duplicarla
    }

    void halveCapacity() {
        if (capacity / 2 < 1) return; // No reducir más si la capacidad es 1.
        std::vector<int> new_queue(capacity / 2);
        std::cout << "Dividiendo a la mitad la capacidad de la cola." << std::endl; 
        for (size_t i = 0; i < size; i++) {
            new_queue[i] = queue[(head + i) % capacity];
        }
        queue = std::move(new_queue);
        head = 0;
        tail = size;
        capacity /= 2;
        logCapacity(); // Registrar capacidad después de reducirla
    }
    
    void logCapacity() { 
        auto now = std::chrono::steady_clock::now(); 
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count(); 
        if (log_file.is_open()) {
            log_file << elapsed << " " << capacity << "\n"; 
        } else {
            std::cerr << "Error: el archivo capacity.log no está abierto." << std::endl;
        }
    }
};

// Variables globales para log
std::ofstream log_file;
std::mutex log_mtx;

void producer(CircularQueue& queue, std::atomic<bool>& done_producing, int id, std::uniform_int_distribution<int>& dist, std::default_random_engine& gen) {
    for (int i = 0; i < 200; i++) { // Número arbitrario de producciones.
        int item = dist(gen);
        queue.enqueue(item);

        // Sincronizar el acceso al archivo de log
        {
            std::lock_guard<std::mutex> lock(log_mtx);
            log_file << "Producer ["<< i <<"]"<< id << " enqueued: " << item << "\n";
        }
    }
    done_producing = true;
}

void consumer(CircularQueue& queue, std::atomic<bool>& done_producing, int id, int timeout) {
    auto start_time = std::chrono::steady_clock::now();
    while (true) {
        if (!queue.isEmpty()) {
            int item = queue.dequeue();
            {
                std::lock_guard<std::mutex> lock(log_mtx);
                log_file << "Consumer " << id << " dequeued: " << item << "\n";
            }
        } else if (done_producing) {
            auto elapsed = std::chrono::steady_clock::now() - start_time;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= timeout) {
                break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int producers = 2, consumers = 2, initial_size = 10, timeout = 5;

    // Procesar argumentos
    int opt;
    while ((opt = getopt(argc, argv, "p:c:s:t:")) != -1) {
        switch (opt) {
            case 'p': producers = atoi(optarg); break;
            case 'c': consumers = atoi(optarg); break;
            case 's': initial_size = atoi(optarg); break;
            case 't': timeout = atoi(optarg); break;
            default: 
                std::cerr << "Uso: ./simulapc -p <prod> -c <cons> -s <tam> -t <time>\n";
                return EXIT_FAILURE;
        }
    }

    CircularQueue queue(initial_size);
    log_file.open("simulapc.log");

    std::atomic<bool> done_producing(false);
    std::vector<std::thread> threads;

    // Inicializar generador de números aleatorios
    std::default_random_engine gen;
    std::uniform_int_distribution<int> dist(0, 100);

    for (int i = 0; i < producers; i++) {
        threads.emplace_back(producer, std::ref(queue), std::ref(done_producing), i, std::ref(dist), std::ref(gen));
    }

    for (int i = 0; i < consumers; i++) {
        threads.emplace_back(consumer, std::ref(queue), std::ref(done_producing), i, timeout);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    log_file.close();
    return 0;
}
