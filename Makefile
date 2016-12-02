all:
	gcc -o k_sh k_sh.c -lpthread

clean:
	rm k_sh
