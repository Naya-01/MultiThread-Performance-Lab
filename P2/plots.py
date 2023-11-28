import numpy as np
import matplotlib.pyplot as plt
import csv

def read_data(filename):
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        next(reader)
        data = list(reader)
    return np.array(data, dtype=int)

def plot_data(program_name, data):
    threads = np.unique(data[:, 0])
    means = [np.mean(data[data[:, 0] == t, 1]) for t in threads]
    std_devs = [np.std(data[data[:, 0] == t, 1]) for t in threads]

    plt.errorbar(threads, means, yerr=std_devs, fmt='o-', capsize=5, label=program_name)
    plt.xlabel('Nombre de Threads')
    plt.ylabel('Temps d\'exécution (ms)')
    plt.title('Performance de ' + program_name)
    plt.ylim(bottom=0)
    plt.legend()
    plt.grid(True)
    plt.savefig(f"./plots/{program_name}.pdf")
    plt.close()


for program in ["philosophers", "producer_consumer", "writer_reader"]:
    data = read_data("./data/" + program + "_performance.csv")
    plot_data(program, data)

