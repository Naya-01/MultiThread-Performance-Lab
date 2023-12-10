import numpy as np
import matplotlib.pyplot as plt
import csv

def read_data(filename):
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        next(reader)
        data = list(reader)
    return np.array(data, dtype=int)

def plot_data(program_name, data, data_tatas):
    threads = np.unique(data[:, 0])
    means = [np.mean(data[data[:, 0] == t, 1]) for t in threads]
    std_devs = [np.std(data[data[:, 0] == t, 1]) for t in threads]
    means_tatas = [np.mean(data_tatas[data_tatas[:, 0] == t, 1]) for t in threads]
    std_devs_tatas = [np.std(data_tatas[data_tatas[:, 0] == t, 1]) for t in threads]

    if "test_and_set" in program_name:
        plt.errorbar(threads, means, yerr=std_devs, fmt='o', capsize=5, label="Test-And-Set")
        plt.errorbar(threads, means_tatas, yerr=std_devs_tatas, fmt='.r', capsize=5, label="Test-And-Test-And-Set")   
        plt.title('Performance du verrou') 
    else:
        plt.errorbar(threads, means, yerr=std_devs, fmt='o', capsize=5, label=f"{program_name} POSIX")
        plt.errorbar(threads, means_tatas, yerr=std_devs_tatas, fmt='.r', capsize=5, label=f"{program_name} TATAS")
        plt.title('Performance de ' + program_name)
    
    plt.xlabel('Nombre de Threads')
    plt.xscale('log', base=2)
    plt.xticks(threads, threads) 
    plt.ylabel('Temps d\'exécution (ms)')
    plt.ylim(bottom=0)
    plt.legend()
    plt.grid(True)
    plt.savefig(f"./plots/{program_name}.pdf")
    plt.close()

for program in ["writer_reader", "producer_consumer", "test_and_set","philosophers"]:
    if "test_and_set" in program:
        data = read_data("./data/" + program + "_performance.csv")
        data_tatas = read_data("./data/test_and_" + program + "_performance.csv")
    else:
        data = read_data("./data/" + program + "_performance.csv")
        data_tatas = read_data("./data/" + program + "_tatas_performance.csv")
    plot_data(program, data, data_tatas)

