CC = g++
CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal\
-Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum\
-Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy\
-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers\
-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing\
-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

all: StackRes.exe

StackRes.exe: Main.o Stack.o Logfile.o Protection.o
	$(CC) $(CFLAGS) $^ -o $@

Main.o: Main.cpp Stack.h Types.h Logfile.h 
	$(CC) $(CFLAGS) $< -c -o $@

Stack.o: Stack.cpp Stack.h Types.h Protection.h Logfile.h
	$(CC) $(CFLAGS) $< -c -o $@

Logfile.o: Logfile.cpp Logfile.h
	$(CC) $(CFLAGS) $< -c -o $@

Protection.o: Protection.cpp Protection.h Stack.h Types.h Logfile.h 
	$(CC) $(CFLAGS) $< -c -o $@


.PHONY: all clean

clean:
	rm *.o
	