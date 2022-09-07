#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#define M 1024
char buf[M];

int main(int argc, char** argv){

		int fd;	
		char *myfifo = "/tmp/myfifo";
		umask(0);
		mkfifo(myfifo,0666);
		int pdesk[2];
		pipe(pdesk);
		int pid;
		pid=fork();
		if(pid==0){
			fd=open(myfifo, O_WRONLY);
			write(fd, argv[1], strlen(argv[1]));
			sleep(2);
			close(pdesk[1]);
			read(pdesk[0], buf , M );
			printf("%s\n", buf);
			close(pdesk[0]);
		}
		else {
			sleep(1);
			fd=open(myfifo, O_RDONLY);		
			read(fd, buf, M);
			close(pdesk[0]);
			struct stat sb;
			if(stat(buf, &sb) == 0 && argc==2){
				char buffor[30];
				int p=0;
				int k=sb.st_size;
				long int i =100000000000;
				while(i>k)
				i=i/10;
				for (i; i>=1; i=i/10){
					buffor[p]=((k-k%i)/i)+'0';
					k=k-((k-k%i)/i)*i;
					p++;
				}
				write(pdesk[1], "Rozmiar w bajtach: " , 19*sizeof(char));
				write(pdesk[1], buffor , strlen(buffor));
				write(pdesk[1], "\nUprawnienia wlasciciela: " , 26*sizeof(char));
				if (sb.st_mode & S_IREAD)
					write(pdesk[1], "R" , sizeof(char));
				if (sb.st_mode & S_IWRITE)
					write(pdesk[1], "W" , sizeof(char));
				if (sb.st_mode & S_IEXEC)
					write(pdesk[1], "X" , sizeof(char));
				write(pdesk[1], "-\n" , 2*sizeof(char));
				if (sb.st_nlink==1){
					write(pdesk[1], "Tylko ten plik ma ten numer i-wezla\n" , 36*sizeof(char));
				}	
				else{
					write(pdesk[1], "Nie tylko ten plik ma ten numer i-wezla\n" , 40*sizeof(char));
				}	
			}
			else{	
				if(argc!=2){
					write(pdesk[1], "Zla liczba argumentow\n" , 22*sizeof(char));
				}
				else
					write(pdesk[1], "Nieprawidlowa sciezka\n" , 22*sizeof(char));
			}
			close(pdesk[1]);
			sleep(3);
		}	
		close(fd);
		unlink(myfifo);
	
	return 0;
}

