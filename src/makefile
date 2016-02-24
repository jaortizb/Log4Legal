TARGET=test.exe

test.exe: libl4l.a test.c compcfg.h
	gcc test.c -o test.exe -L. -ll4l -lcrypto  -lpthread -lrt -lcurl -std=gnu99

libl4l.a: l4l.o timestamp.o  log.cfg
	ar -cvq libl4l.a l4l.o timestamp.o

l4l.o timestamp.o: l4l.c timestamp.c
	gcc -c l4l.c timestamp.c 

clean:
	rm -fr test.exe libl4l.a l4l.o timestamp.o

