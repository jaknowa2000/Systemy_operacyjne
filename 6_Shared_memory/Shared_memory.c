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
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <err.h>
#include <string.h>

#define SHM_SIZE     100

union semun {
	int val;							
	struct semid_ds *buf;				
	unsigned short int *array;	
	struct seminfo *__buf;			
};
	
int semlock(int semid) {
	struct sembuf opr;	
	opr.sem_num =  0;
	opr.sem_op  = -1;        
	opr.sem_flg =  0;        
	if (semop(semid, &opr, 1) == -1){
    		warn("Blad blokowania semafora!");
    		return 0;
    	}
    	else{
    		return 1;
    	}
}

int semunlock(int semid) {
	struct sembuf opr;	
	opr.sem_num = 0;
	opr.sem_op  = 1;
	opr.sem_flg = 0;
	if (semop(semid, &opr, 1) == -1){
    		warn("Blad odblokowania semafora!");
    		return 0;
	}
	else{
    		return 1;
	}
}

int main(int argc, char** argv){
	key_t key, key1, key2;
	int   shmid, semid1, semid2;
	union semun ctl;
	if ((key1 = ftok(".", 'A')) == -1) 
        	errx(1, "Blad tworzenia klucza!");
	if ((semid1 = semget(key1, 1, IPC_CREAT | 0600)) == -1)
    		errx(2, "Blad tworzenia semafora!");
	ctl.val = 1;
	if (semctl(semid1, 0, SETVAL, ctl) == -1)
    		errx(3, "Blad ustawiania semafora!");
	if ((key2 = ftok(".", 'B')) == -1) 
        	errx(1, "Blad tworzenia klucza!");
	if ((semid2 = semget(key2, 1, IPC_CREAT | 0600)) == -1)
    		errx(2, "Blad tworzenia semafora!");
	ctl.val = 1;
    	if (semctl(semid2, 0, SETVAL, ctl) == -1)
    		errx(3, "Blad ustawiania semafora!");
	semlock(semid2);
	char c;
	char *shm, *s;
	if ((key = ftok(".", 'A')) == -1) 
        errx(1, "Blad tworzenia klucza!");
	if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) 
    	errx(2, "Blad tworzenia segmentu pamieci dzielonej!");
	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
    	errx(3, "Blad przylaczania pamieci dzielonej!"); 
	int i, pid;
	pid=fork();
		if(pid==0){
			s = shm;
	       		for (int b=0; b<strlen(argv[1]); b++){
	       			*s++=argv[1][b];
	       		}
			semlock(semid1);
			semunlock(semid2);
			while (semctl(semid1, 0, GETVAL) == 0)
				sleep(0.1);
			s=shm;
			for (i = 0; i < 2; i++){
				printf("%s" , s);
				s=s+strlen(s);
			}		
			shmdt(shm);
			shmctl(shmid, IPC_RMID, NULL);	
			semctl(semid1, 0, IPC_RMID, ctl);
    			semctl(semid2, 0, IPC_RMID, ctl);	
		}
		else {
			while (semctl(semid2, 0, GETVAL) == 0)
				sleep(0.1);
			s = shm;
			char buffor[30];
			for (i = 0; i < SHM_SIZE; i++){
				buffor[i]=*s;
	    			s++;
			}
			int c=25;
			struct stat sb;
			if(stat(buffor, &sb) == 0 && argc==2){
				char b[300]="Uprawnienia wlasciciela: ";
				char bufor[300];
				if (sb.st_mode & S_IREAD){
					b[c]='R';
					c++;
				}	
				if (sb.st_mode & S_IWRITE){
					b[c]='W';
					c++;
				}
				if (sb.st_mode & S_IEXEC){
					b[c]='X';
					c++;
				}
				b[c]='-';
				c++;
				b[c]='\n';
				c++;
				c=c+sprintf(b+c, "Rozmiar w bajtach: %ld\n", sb.st_size);
				if (sb.st_nlink==1){
					c=c+sprintf(b+c, "Tylko ten plik ma ten numer i-wezla\n");
				}	
				else{
					c=c+sprintf(b+c, "Nie tylko ten plik ma ten numer i-wezla\n");

				}
				s=shm;
				for (int c=0; c<300; c++){
					*s=b[c];
					s++;
				}
			}
			else{
					s=shm;
					char ba[30];
				if(argc!=2){
					sprintf(ba, "Zla liczba argumentow\n");
					for (int c=0; c<30; c++){
						*s=ba[c];
						s++;
					}
				}
				else{
					sprintf(ba, "Nieprawidlowa sciezka\n");
					for (int c=0; c<30; c++){
						*s=ba[c];
						s++;
					}
				}
			}
			semlock(semid2);
			semunlock(semid1);
			sleep(3);
			shmdt(shm);
		}
	
	return 0;
}
