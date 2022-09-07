#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>


void listuj(char *dirName);

int main(int argc, char** argv){
	if(argc!=2){
		printf("Zla liczba argumentow\n");
	}
	else {
		listuj(argv[1]);
	}
	return 0;
}

void listuj(char* dirName){
	struct stat sb;
	if(stat(dirName, &sb) == 0){
		printf("Rozmiar w bajtach: %ld\n", sb.st_size);	
		printf("Uprawnienia wlasciciela: ");
		if (sb.st_mode & S_IREAD)
			printf("R");
		if (sb.st_mode & S_IWRITE)
			printf("W");
		if (sb.st_mode & S_IEXEC)
			printf("X");
		printf("-\n");
		if (sb.st_nlink==1)
			printf("Tylko ten plik ma ten numer i-wezla\n");	
		else
			printf("Nie tylko ten plik ma ten numer i-wezla\n");		
	}
	else
	printf("Nieprawidlowa sciezka\n");
}
