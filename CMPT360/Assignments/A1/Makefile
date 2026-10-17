CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic

MAIN = procview
MAIN_SRCS = procview.c
MAIN_HDRS = pv_impl.h


all: $(MAIN)

$(MAIN): $(MAIN_SRCS) $(MAIN_HDRS)
	$(CC) $(CFLAGS) $(MAIN_SRCS) -o $(MAIN)


.PHONY: run valgrind clean		#declare as targets

# ---- run ----
run:
	./$(MAIN) 

#---- Valgrind --------

valgrind: $(MAIN)
	valgrind --leak-check=full ./$(MAIN) 

# ---- Clean ----

clean:
	rm -f $(MAIN) 

