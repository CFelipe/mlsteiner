import numpy as np
from subprocess import check_output

with open("table.tex", "w+") as f:
    f.write("\\begin{tabular}{ |c|c|c|c||c|c|c| }\n")
    f.write("\hline\n")
    f.write("$ n $ & $ l $ & $ d $ & $ b $ & Exato & GRASP & VNS \\\\\n")
    f.write("\hline\n")
    sizes = [50, 100]
    colors = [0.25, 0.5, 1, 1.25]
    densities = [20, 50, 80]
    basics = [0.2, 0.4]

    for size in sizes:
        #f.write("\multirow{" + str(len(colors)) + "}{*}{" + str(size) + "}")
        for color in [0.25, 0.5, 1, 1.25]:
            for density in densities:
                for basic in basics:
                    print("---")
                    print("s{} | c{} | d{} | b{}".format(size, int(color * size), density, int(basic * size)))
                    grasp_results = []
                    vns_results = []
                    for i in range(1):
                        grasp_output = check_output("./grasp -f testes/n{n}-l{l}-b{b}-d{d}.in {n} {l} {b} {d}".format(n=size,
                                                                                                                l=int(size * color),
                                                                                                                b=int(size * basic),
                                                                                                                d=density),
                                                                                                                shell=True)
                        vns_output = check_output("./vns -f testes/n{n}-l{l}-b{b}-d{d}.in {n} {l} {b} {d}".format(n=size,
                                                                                                                  l=int(size * color),
                                                                                                                  b=int(size * basic),
                                                                                                                  d=density),
                                                                                                                  shell=True)
                        grasp_results.append([float(x) for x in grasp_output.decode().split()])
                        vns_results.append([float(x) for x in vns_output.decode().split()])
                    print("Média (solução):")
                    grasp_result_mean = np.mean([result[0] for result in grasp_results])
                    print(grasp_result_mean)

                    print("Média (tempo):")
                    grasp_time_mean = np.mean([result[1] for result in grasp_results])
                    vns_time_mean = np.mean([result[1] for result in vns_results])
                    print(grasp_time_mean)
                    f.write("{n} & {l} & {d} & {b} & {grasp:6.2f} & {grasp:6.2f} & {vns:6.2f} \\\\\n".format(n=size,
                                                                                                l=int(size * color),
                                                                                                b=int(size * basic),
                                                                                                d=density,
                                                                                                grasp=grasp_time_mean,
                                                                                                vns=vns_time_mean))
    f.write("\hline\n")
    f.write("\end{tabular}\n")
