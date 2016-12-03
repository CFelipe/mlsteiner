import numpy as np
import matplotlib.pyplot as plt
import matplotlib.transforms as transforms
import matplotlib.patches as mpatches
from subprocess import check_output

plt_colors=["#A60628", "#348ABD", "#467821", "#7A68A6",  "#CF4457"]

#plt.style.use("ggplot")
fig1 = plt.figure(figsize=(12, 6))
ax1 = fig1.add_subplot(111)
fig2 = plt.figure(figsize=(12, 6))
ax2 = fig2.add_subplot(111)
lt1 = ax1.transData + transforms.ScaledTranslation(-7/72, 0, fig1.dpi_scale_trans)
rt1 = ax1.transData + transforms.ScaledTranslation(7/72, 0, fig1.dpi_scale_trans)
lt2 = ax2.transData + transforms.ScaledTranslation(-7/72, 0, fig2.dpi_scale_trans)
rt2 = ax2.transData + transforms.ScaledTranslation(7/72, 0, fig2.dpi_scale_trans)
vns_patch = mpatches.Patch(color=plt_colors[0], label='VNS')
grasp_patch = mpatches.Patch(color=plt_colors[1], label='GRASP')
gvns_patch = mpatches.Patch(color=plt_colors[2], label='GRASP-VNS')
best_patch = plt.Line2D((0,1),(0,0),    color='k', marker='v', linestyle='', label="Melhor")
worst_patch = plt.Line2D((0,1),(0,0),   color='k', marker='^', linestyle='', label="Pior")
median_patch = plt.Line2D((0,1),(0,0),  color='k', marker='s', linestyle='', label="Mediana")
mean_patch = plt.Line2D((0,1),(0,0),    color='k', marker='*', linestyle='', label="Média")
ax1.grid(True, which='minor')
ax2.grid(True, which='minor')
ax1.legend(handles=[vns_patch, grasp_patch, gvns_patch, best_patch, worst_patch, median_patch, mean_patch], loc=2, fontsize=10)
ax2.legend(handles=[vns_patch, grasp_patch, gvns_patch, best_patch, worst_patch, median_patch, mean_patch], loc=2, fontsize=10)

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
        sizes = [50, 100]
        colors = [0.25, 0.5, 1]
        densities = [50, 20]
        basics = [0.2, 0.4]
        labels = []

        for size in sizes:
            f1.write("\multirow{" + str(len(colors) * len(densities) * len(basics)) + "}{*}{" + str(size) + "} ")
            f2.write("\multirow{" + str(len(colors) * len(densities) * len(basics)) + "}{*}{" + str(size) + "} ")
            for color in colors:
                #f1.write("& \multirow{" + str(len(densities) * len(basics)) + "}{*}{" + str(color) + "} ")
                for density in densities:
                    #f1.write("& \multirow{" + str(len(basics)) + "}{*}{" + str(density) + "} ")
                    for basic in basics:
                        #f1.write("& " + str(basic)" )
                        print("---")
                        print("s{} | c{} | d{} | b{}".format(size, int(color * size), density, int(basic * size)))
                        grasp_results = []
                        vns_results = []
                        gvns_results = []
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
                            gvns_output = check_output("./vns -f testes/n{n}-l{l}-b{b}-d{d}.in".format(n=size,
                                                                                                    l=int(size * color),
                                                                                                    b=int(size * basic),
                                                                                                    d=density),
                                                                                                    shell=True)
                            exact_results = [float(x) for x in exact_output.decode().split()]
                            grasp_results.append([float(x) for x in grasp_output.decode().split()])
                            vns_results.append([float(x) for x in vns_output.decode().split()])
                            gvns_results.append([float(x) for x in gvns_output.decode().split()])
                        grasp_result_mean = np.mean(    [result[0] for result in grasp_results])
                        grasp_result_median = np.median([result[0] for result in grasp_results])
                        grasp_result_best = np.amin(    [result[0] for result in grasp_results])
                        grasp_result_worst = np.amax(   [result[0] for result in grasp_results])
                        vns_result_mean = np.mean(      [result[0] for result in vns_results])
                        vns_result_median = np.median(  [result[0] for result in vns_results])
                        vns_result_best = np.amin(      [result[0] for result in vns_results])
                        vns_result_worst = np.amax(     [result[0] for result in vns_results])
                        gvns_result_mean = np.mean(      [result[0] for result in gvns_results])
                        gvns_result_median = np.median(  [result[0] for result in gvns_results])
                        gvns_result_best = np.amin(      [result[0] for result in gvns_results])
                        gvns_result_worst = np.amax(     [result[0] for result in gvns_results])

                        f2.write("& {l} & {d} & {b} & {exact:6.2f} & {grasp:6.2f} & {vns:6.2f} \\\\\n".format(n=size,
                                                                                                    l=int(size * color),
                                                                                                    d=density,
                                                                                                    b=int(size * basic),
                                                                                                    exact=exact_results[0],
                                                                                                    grasp=grasp_result_mean,
                                                                                                    vns=vns_result_mean))

                        grasp_time_mean = np.mean(      [result[1] for result in grasp_results])
                        grasp_time_median = np.median(  [result[1] for result in grasp_results])
                        grasp_time_best = np.amin(      [result[1] for result in grasp_results])
                        grasp_time_worst = np.amax(     [result[1] for result in grasp_results])
                        vns_time_mean = np.mean(        [result[1] for result in vns_results])
                        vns_time_median = np.median(    [result[1] for result in vns_results])
                        vns_time_best = np.amin(        [result[1] for result in vns_results])
                        vns_time_worst = np.amax(       [result[1] for result in vns_results])
                        gvns_time_mean = np.mean(        [result[1] for result in gvns_results])
                        gvns_time_median = np.median(    [result[1] for result in gvns_results])
                        gvns_time_best = np.amin(        [result[1] for result in gvns_results])
                        gvns_time_worst = np.amax(       [result[1] for result in gvns_results])

                        f1.write("& {l} & {d} & {b} & {exact:6.2f} & {grasp:6.2f} & {vns:6.2f} \\\\\n".format(n=size,
                                                                                                    l=int(size * color),
                                                                                                    d=density,
                                                                                                    b=int(size * basic),
                                                                                                    exact=exact_results[1],
                                                                                                    grasp=grasp_time_mean,
                                                                                                    vns=vns_time_mean))


                        labels.append("n{n}-l{l}-d{d}-b{b}".format(n=size,
                                                                   l=int(size * color),
                                                                   b=int(size * basic),
                                                                   d=density))

                        ax1.plot(len(labels), vns_result_median,    's', color=plt_colors[0], markeredgecolor="white")
                        ax1.plot(len(labels), vns_result_mean,      '*', color=plt_colors[0], markeredgecolor="white")
                        ax1.plot(len(labels), vns_result_best,      'v', color=plt_colors[0], markeredgecolor="white")
                        ax1.plot(len(labels), vns_result_worst,     '^', color=plt_colors[0], markeredgecolor="white")

                        ax1.plot(len(labels), grasp_result_median,  's', color=plt_colors[1], markeredgecolor="white", transform=lt1)
                        ax1.plot(len(labels), grasp_result_mean,    '*', color=plt_colors[1], markeredgecolor="white", transform=lt1)
                        ax1.plot(len(labels), grasp_result_best,    'v', color=plt_colors[1], markeredgecolor="white", transform=lt1)
                        ax1.plot(len(labels), grasp_result_worst,   '^', color=plt_colors[1], markeredgecolor="white", transform=lt1)

                        ax1.plot(len(labels), gvns_result_median,   's', color=plt_colors[2], markeredgecolor="white", transform=rt1)
                        ax1.plot(len(labels), gvns_result_mean,     '*', color=plt_colors[2], markeredgecolor="white", transform=rt1)
                        ax1.plot(len(labels), gvns_result_best,     'v', color=plt_colors[2], markeredgecolor="white", transform=rt1)
                        ax1.plot(len(labels), gvns_result_worst,    '^', color=plt_colors[2], markeredgecolor="white", transform=rt1)
                        #if(exact_results[1] != -1):
                        #    plt.plot(len(labels), exact_results[0], 'g*')

                        ax2.plot(len(labels), vns_time_median,      's', color=plt_colors[0], markeredgecolor="white")
                        ax2.plot(len(labels), vns_time_mean,        '*', color=plt_colors[0], markeredgecolor="white")
                        ax2.plot(len(labels), vns_time_best,        'v', color=plt_colors[0], markeredgecolor="white")
                        ax2.plot(len(labels), vns_time_worst,       '^', color=plt_colors[0], markeredgecolor="white")

                        ax2.plot(len(labels), grasp_time_median,    's', color=plt_colors[1], markeredgecolor="white", transform=lt2)
                        ax2.plot(len(labels), grasp_time_mean,      '*', color=plt_colors[1], markeredgecolor="white", transform=lt2)
                        ax2.plot(len(labels), grasp_time_best,      'v', color=plt_colors[1], markeredgecolor="white", transform=lt2)
                        ax2.plot(len(labels), grasp_time_worst,     '^', color=plt_colors[1], markeredgecolor="white", transform=lt2)

                        ax2.plot(len(labels), gvns_time_median,     's', color=plt_colors[2], markeredgecolor="white", transform=rt2)
                        ax2.plot(len(labels), gvns_time_mean,       '*', color=plt_colors[2], markeredgecolor="white", transform=rt2)
                        ax2.plot(len(labels), gvns_time_best,       'v', color=plt_colors[2], markeredgecolor="white", transform=rt2)
                        ax2.plot(len(labels), gvns_time_worst,      '^', color=plt_colors[2], markeredgecolor="white", transform=rt2)
                        #if(exact_results[1] != -1):
                        #    plt.plot(len(labels), exact_results[1], 'g*')

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

#labels.insert(0, "")
ax1.set_xticks(range(1, len(labels) + 1), minor=False)
ax1.set_xticks([x - 0.5 for x in range(1, len(labels) + 2)], minor=True)
ax1.set_ylabel('$ | C | $')
ax2.set_xticks(range(1, len(labels) + 1), minor=False)
ax2.set_xticks([x - 0.5 for x in range(1, len(labels) + 2)], minor=True)
ax2.set_ylabel('Tempo (ms)')
ax1.set_xticklabels(labels, rotation="vertical")
ax2.set_xticklabels(labels, rotation="vertical")
# Pad margins so that markers don't get clipped by the axes
ax1.margins(0.05)
ax2.margins(0.05)
# Tweak spacing to prevent clipping of tick-labels
fig1.subplots_adjust(bottom=0.30)
fig2.subplots_adjust(bottom=0.30)
#plt.show()
fig1.savefig("cardcplot.pdf")
fig2.savefig("timeplot.pdf")
