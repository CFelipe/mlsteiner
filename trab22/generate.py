from subprocess import run

for n in [100, 500]:
    for l in [25, 50, 100, 125]:
        for d in [80, 50, 20]:
            #run(["generate"]);
            run("./generate testes/n{}-l{}-b{}-d{} {} {} {} {}".format(n, l, n//5, d, n, l, n//5, d), shell=True)
