#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

struct mymsgbuf {
	long mtype;
	char i[1024];
} msg;

struct mymsgbuf buf;

int open_queue( key_t keyval ) {
	int qid;
	if((qid = msgget( keyval, IPC_CREAT | 0660 )) == -1)
		return(-1);
	return(qid);
}

int send_message( int qid, struct mymsgbuf *qbuf ){
	int result, length;
	length = sizeof(struct mymsgbuf) - sizeof(long);
	if((result = msgsnd( qid, qbuf, length, 0)) == -1)
		return(-1);
	return(result);
}

int remove_queue( int qid ){
	if( msgctl( qid, IPC_RMID, 0) == -1)
		return(-1);
	return(0);
}

int read_message( int qid, long type, struct mymsgbuf *qbuf ) {
	int result, length;
	length = sizeof(struct mymsgbuf) - sizeof(long);
	if((result = msgrcv( qid, qbuf, length, type, 0)) == -1)
		return(-1);
	return(result);
}
	

int main(int argc, char** argv){

	int qid;
	key_t msgkey;
	msgkey = ftok(".", 'm');
	if(( qid = open_queue( msgkey)) == -1) {
		perror("Blad przy otwieraniu kolejki");
		exit(1);
	}
	msg.mtype = 1;
	buf.mtype = 1;
	int pid;
	pid=fork();
		if(pid==0){
			strcpy (msg.i, argv[1]);
			if((send_message( qid, &msg )) == -1) {
				perror("Blad wysylania");
				exit(1);
			}
			sleep(2);
			read_message(qid, buf.mtype, &buf);
			printf("%s\n", buf.i);
			read_message(qid, buf.mtype, &buf);
			printf("%s\n", buf.i);
			read_message(qid, buf.mtype, &buf);	
			printf("%s\n", buf.i);
			remove_queue(qid);
		}
		else {
			sleep(1);
			read_message(qid, buf.mtype, &buf);
			struct stat sb;
			if(stat(buf.i, &sb) == 0 && argc==2){
				char bufor[300];
				char b[300]="Uprawnienia wlasciciela: ";
				int c=25;
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
				strcpy (msg.i, b);
				if((send_message( qid, &msg )) == -1) {
					perror("Blady wysylania");
					exit(1);
				}
				sprintf(bufor, "Rozmiar w bajtach: %ld", sb.st_size);
				strcpy (msg.i, bufor);
				if((send_message( qid, &msg )) == -1) {
					perror("Blady wysylania");
					exit(1);
				}
				if (sb.st_nlink==1){
					strcpy (msg.i, "Tylko ten plik ma ten numer i-wezla\n");
					if((send_message( qid, &msg )) == -1) {
						perror("Blady wysylania");
						exit(1);
					}
				}	
				else{
					strcpy (msg.i, "Nie tylko ten plik ma ten numer i-wezla\n");
					if((send_message( qid, &msg )) == -1) {
						perror("Blady wysylania");
						exit(1);
					}
				}
			}
			else{
				if(argc!=2){
					strcpy (msg.i, "Zla liczba argumentow");
					if((send_message( qid, &msg )) == -1) {
						perror("Blad wysylania");
						exit(1);
					}
				}
				else{
					strcpy (msg.i, "Nieprawidlowa sciezka");
					if((send_message( qid, &msg )) == -1) {
						perror("Blad wysylania");
						exit(1);
					}
				}
			}
			sleep(4);
			remove_queue(qid);
		}	

	return 0;
}

