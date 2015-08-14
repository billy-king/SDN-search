all:
	gcc FindController_S.c -o server
	gcc FindController_C.c -o client
clean:
	rm server
	rm client