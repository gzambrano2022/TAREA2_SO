# TAREA2_SO
### ¿Que hay que hacer en esta tarea?
- Ejecutor de productor y consumidor en un Arreglo dinamico, manejando concurrencia con monitor
- Simulador de Memoria Virtual. 

## ¿Como ejecutar el simulador de memoria virtual?
Para ejecutar SO.cpp se debe abrir la terminal y escribir lo siguiente:

>>g++ SO.cpp -o simulapc -pthread
>>./simulapc -p 10 -c 5 -s 50 -t 1 (estos son parámetros ajustables)

/* Esto muestra por la terminal como se duplica y divide por la mitad el tamaño del arreglo */

>>cat simulapc.log

Con esto se muestra como van funcionando las hebras productoras y consumidoras una a una
Si se corre de forma rápida presionando play en un editor de código convencional como Visual Studio Code el código se ejecutará con parámetros que se definen en el main.  

## ¿Como ejecutar el simulador de memoria virtual?
Para ejecutar mv.c en Linux, se debe abrir la terminal donde esté el archivo compilado y escribir lo siguiente:

./**nombre del ejecutable** -m **cantidad de marcos** -a **Tipo de algoritmo** -f **referencias.txt**.

en Tipo de algoritmo, están los siguientes:
- FIFO, para algoritmo FIFO
- LRU, para algortimo LRU
- Clock, para algoritmo LRU Reloj
- Optimal, para algoritmo Óptimo. 

