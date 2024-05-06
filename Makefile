CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS =
all: folders orchestrator client
orchestrator: bin/orchestrator
client: bin/client
folders:
	@mkdir -p src include obj bin tmp pipes

bin/orchestrator: obj/orchestrator.o obj/Prog.o obj/Task.o obj/Queue.o obj/Auxiliares.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/client: obj/client.o obj/Prog.o obj/Task.o obj/Auxiliares.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

r:
	rm -rf output_folder

clean:
	rm -f obj/* tmp/* bin/* pipes/*
