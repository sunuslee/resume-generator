TOP = .
CFLAGS := -Wall -std=gnu99 -g -I$(TOP)

cv_maker: cv_maker.o utility.o
	gcc -o cv_maker cv_maker.o utility.o
utility.o: ./utility.h ./utility.c
	gcc -c $(CFLAGS) ./utility.h ./utility.c
cv_maker.o: ./cv_maker.h ./utility.h ./cv_maker.c
	gcc -c $(CFLAGS) ./cv_maker.h ./utility.h ./cv_maker.c

clean :
	rm -v ./cv_maker ./cv_maker.o ./utility.o ./cv_maker.h.gch ./utility.h.gch
