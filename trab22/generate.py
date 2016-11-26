from subprocess import run

for d in [80, 50, 20]:
	for n in [100]:
	    for l in [25, 50, 100, 125]:
		    run("./generate testes/n{}-l{}-b{}-d{}.in {} {} {} {}".format(n, l, n//5, d, n, l, n//5, d), shell=True)
	for n in [500]:
	    for l in [125, 250, 500, 625]:
		    run("./generate testes/n{}-l{}-b{}-d{}.in {} {} {} {}".format(n, l, n//5, d, n, l, n//5, d), shell=True)
