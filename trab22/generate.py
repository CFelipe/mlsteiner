from subprocess import run

for size in [50, 100]:
    for colors in [0.25, 0.5, 1, 1.25]:
        for density in [20, 50, 80]:
            for basic in [0.2, 0.4]:
                print("s{} | c{} | d{} | b{}".format(size, int(colors * size), density, int(basic * size)))
                run("./generate testes/n{n}-l{l}-b{b}-d{d}.in {n} {l} {b} {d}".format(n=size,
                                                                                      l=int(size * colors),
                                                                                      b=int(size * basic),
                                                                                      d=density),
                                                                                      shell=True)
