#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signals_handler(int signal){
	psignal(signal,"\nOtrzymalem sygnal");
	printf("Numer sygnalu to: %d\n", signal);
	printf("PID: %d\n", getpid());
	if (signal==SIGINT){
		printf("Zamykam proces potomny \n");
		kill(getppid(), SIGTTIN);
	}
	return;	
}

void end_sigttin(int signal){
	printf("Jestem w procesie macierzystym");
	psignal(signal,"\nOtrzymalem sygnal");
	printf("\nNumer sygnalu to: %d\n", signal);
	printf("PID: %d\n", getpid());
	kill(getpid(), SIGKILL);
	return;	
}

int main(){
	sigset_t mask_set, old_mask;
	int pid;
	pid=fork();
	if (pid==0){
		sigemptyset(&mask_set);
		sigprocmask(SIG_SETMASK, &mask_set, &old_mask);
		printf("PID procesu potomnego: %d\n", getpid());
		for (int a=1; a<65; a++){
			signal(a, signals_handler);
		}
		for (;;)
			pause();
	}
	sigfillset(&mask_set);
	sigdelset(&mask_set, SIGTTIN);
	sigprocmask(SIG_SETMASK, &mask_set, &old_mask);
	printf("PID procesu macierzystego: %d\n", getpid());
	signal(SIGTTIN, end_sigttin);
	for (;;)
		pause();
	return 0;
}
