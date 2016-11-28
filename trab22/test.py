import numpy as np
import matplotlib.pyplot as plt
import matplotlib.transforms as transforms
import matplotlib.patches as mpatches
from subprocess import check_output

fig = plt.figure()
ax = fig.add_subplot(111)
lt = ax.transData + transforms.ScaledTranslation(-5/72, 0, fig.dpi_scale_trans)
rt = ax.transData + transforms.ScaledTranslation(5/72, 0, fig.dpi_scale_trans)
red_patch = mpatches.Patch(color='red', label='VNS')
blue_patch = mpatches.Patch(color='blue', label='GRASP')
green_patch = mpatches.Patch(color='green', label='Exato')
best_patch = plt.Line2D((0,1),(0,0), color='k', marker='v', linestyle='', label="Melhor")
worst_patch = plt.Line2D((0,1),(0,0), color='k', marker='^', linestyle='', label="Pior")
median_patch = plt.Line2D((0,1),(0,0), color='k', marker='s', linestyle='', label="Mediana")
mean_patch = plt.Line2D((0,1),(0,0), color='k', marker='*', linestyle='', label="Média")
plt.legend(handles=[red_patch, blue_patch, green_patch, best_patch, worst_patch, median_patch, mean_patch], loc=2)

with open("times_table.tex", "w+") as f1:
    with open("results_table.tex", "w+") as f2:
        f1.write("\\begin{tabular}{ |c|c|c|c||c c c| }\n")
        f1.write("\hline\n")
        f1.write("$ n $ & $ l $ & $ d $ & $ b $ & Exato & GRASP & VNS \\\\\n")
        f1.write("\hline\n")
        f2.write("\\begin{tabular}{ |c|c|c|c||c c c| }\n")
        f2.write("\hline\n")
        f2.write("$ n $ & $ l $ & $ d $ & $ b $ & Exato & GRASP & VNS \\\\\n")
        f2.write("\hline\n")
        sizes = [100]
        colors = [0.25, 0.5, 1, 1.25]
        densities = [80, 50, 20]
        basics = [0.2, 0.4]
        labels = []

        for size in sizes:
            f1.write("\multirow{" + str(len(colors) * len(densities) * len(basics)) + "}{*}{" + str(size) + "} ")
            f2.write("\multirow{" + str(len(colors) * len(densities) * len(basics)) + "}{*}{" + str(size) + "} ")
            for color in [0.25, 0.5, 1, 1.25]:
                #f1.write("& \multirow{" + str(len(densities) * len(basics)) + "}{*}{" + str(color) + "} ")
                for density in densities:
                    #f1.write("& \multirow{" + str(len(basics)) + "}{*}{" + str(density) + "} ")
                    for basic in basics:
                        #f1.write("& " + str(basic)" )
                        print("---")
                        print("s{} | c{} | d{} | b{}".format(size, int(color * size), density, int(basic * size)))
                        grasp_results = []
                        vns_results = []
                        exact_output = check_output("./exact -f testes/n{n}-l{l}-b{b}-d{d}.in".format(n=size,
                                                                                                l=int(size * color),
                                                                                                b=int(size * basic),
                                                                                                d=density),
                                                                                                shell=True)
                        for i in range(10):
                            grasp_output = check_output("./grasp -f testes/n{n}-l{l}-b{b}-d{d}.in".format(n=size,
                                                                                                        l=int(size * color),
                                                                                                        b=int(size * basic),
                                                                                                        d=density),
                                                                                                        shell=True)
                            vns_output = check_output("./vns -f testes/n{n}-l{l}-b{b}-d{d}.in".format(n=size,
                                                                                                    l=int(size * color),
                                                                                                    b=int(size * basic),
                                                                                                    d=density),
                                                                                                    shell=True)
                            exact_results = [float(x) for x in exact_output.decode().split()]
                            grasp_results.append([float(x) for x in grasp_output.decode().split()])
                            vns_results.append([float(x) for x in vns_output.decode().split()])
                        print("Média (solução):")
                        grasp_result_mean = np.mean(    [result[0] for result in grasp_results])
                        grasp_result_median = np.median([result[0] for result in grasp_results])
                        grasp_result_best = np.amin(    [result[0] for result in grasp_results])
                        grasp_result_worst = np.amax(   [result[0] for result in grasp_results])
                        vns_result_mean = np.mean(      [result[0] for result in vns_results])
                        vns_result_median = np.median(  [result[0] for result in vns_results])
                        vns_result_best = np.amin(      [result[0] for result in vns_results])
                        vns_result_worst = np.amax(     [result[0] for result in vns_results])
                        print(grasp_result_mean)

                        print("Média (tempo):")
                        grasp_time_mean = np.mean(      [result[1] for result in grasp_results])
                        grasp_time_median = np.median(  [result[1] for result in grasp_results])
                        grasp_time_best = np.amin(      [result[1] for result in grasp_results])
                        grasp_time_worst = np.amax(     [result[1] for result in grasp_results])
                        vns_time_mean = np.mean(        [result[1] for result in vns_results])
                        vns_time_median = np.median(    [result[1] for result in vns_results])
                        vns_time_best = np.amin(        [result[1] for result in vns_results])
                        vns_time_worst = np.amax(       [result[1] for result in vns_results])
                        print(grasp_time_mean)
                        f1.write("& {l} & {d} & {b} & {exact:6.2f} & {grasp:6.2f} & {vns:6.2f} \\\\\n".format(n=size,
                                                                                                    l=int(size * color),
                                                                                                    d=density,
                                                                                                    b=int(size * basic),
                                                                                                    exact=exact_results[1],
                                                                                                    grasp=grasp_time_mean,
                                                                                                    vns=vns_time_mean))
                        f2.write("& {l} & {d} & {b} & {exact:6.2f} & {grasp:6.2f} & {vns:6.2f} \\\\\n".format(n=size,
                                                                                                    l=int(size * color),
                                                                                                    d=density,
                                                                                                    b=int(size * basic),
                                                                                                    exact=exact_results[0],
                                                                                                    grasp=grasp_result_mean,
                                                                                                    vns=vns_result_mean))


                        plt.plot(len(labels), vns_time_mean, 'r*', transform=lt)
                        plt.plot(len(labels), vns_time_median, 'rs', transform=lt)
                        plt.plot(len(labels), vns_time_best, 'rv', transform=lt)
                        plt.plot(len(labels), vns_time_worst, 'r^', transform=lt)
                        plt.plot(len(labels), grasp_time_mean, 'b*', transform=rt)
                        plt.plot(len(labels), grasp_time_median, 'bs', transform=rt)
                        plt.plot(len(labels), grasp_time_best, 'bv', transform=rt)
                        plt.plot(len(labels), grasp_time_worst, 'b^', transform=rt)
                        if(exact_results[1] != -1):
                            plt.plot(len(labels), exact_results[1], 'g*')

                        labels.append("n{n}-l{l}-d{d}-b{b}".format(n=size,
                                                                   l=int(size * color),
                                                                   b=int(size * basic),
                                                                   d=density))

                    f1.write("\cline{3-4}")
                    f2.write("\cline{3-4}")
                f1.write("\cline{2-2}")
                f2.write("\cline{2-2}")
            f1.write("\cline{1-1}")
            f2.write("\cline{1-1}")
        f1.write("\hline\n")
        f2.write("\hline\n")
        f1.write("\end{tabular}\n")
        f2.write("\end{tabular}\n")

plt.xticks(range(len(labels)), labels, rotation="vertical")
# Pad margins so that markers don't get clipped by the axes
plt.margins(0.2)
# Tweak spacing to prevent clipping of tick-labels
plt.subplots_adjust(bottom=0.23)
plt.show()
