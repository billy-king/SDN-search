all:
	mkdir server_fold
	mkdir client_fold
	gcc FindController_S.c -o ./server_fold/server
	gcc FindController_C.c -o ./client_fold/client
clean:
	rm -rf server_fold
	rm -rf server_fold