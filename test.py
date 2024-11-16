import matplotlib.pyplot as plt
import pandas as pd

# Leer el archivo de log
log_data = pd.read_csv('capacity.log', sep=" ", header=None, names=["time", "capacity"])

# Graficar capacidad de la cola a lo largo del tiempo
plt.figure(figsize=(10, 5))
plt.plot(log_data['time'], log_data['capacity'], marker='o', linestyle='-', color='b', label='Capacidad de la Cola')
plt.xlabel('Tiempo (ms)')
plt.ylabel('Capacidad')
plt.title('Capacidad de la Cola a lo Largo del Tiempo')
plt.grid(True)
plt.legend()
plt.show()
