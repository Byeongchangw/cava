cava: cava.c
	gcc cava.c -o cava -lasound -lm -lfftw3 -lpthread
clean:
	rm cava

.PHONY: clean
