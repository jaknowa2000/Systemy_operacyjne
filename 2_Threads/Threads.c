#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void * wt1(void *arg){
	double* a= (double*)arg;
	*a=*a*0.62137;
	printf("Identyfikator watku nr1: \n%ld\n",pthread_self());
	return (void*)a;
}

void * wt2(void *arg){
	double* b= (double*)arg;
	*b=*b*0.000000055594;
	printf("Identyfikator watku nr2: \n%ld\n",pthread_self());
	return (void*)b;
}

int main(int argc, char **argv)
{
	if (argc!=2){
		printf("Zla liczba argumentow\n");
	}
	else {
		int i=0;
		int f=0;
		int e;
		while (argv[1][i]!=0){
			i++;
		}
		if(argv[1][0]<'0' || argv[1][0]>'9')
			f=1;
		for (e=1; e<i-1; e++){
			if((argv[1][e]<'0' || argv[1][e]>'9') && (argv[1][e]!='.'))
				f=1;
		}
		if(argv[1][i-1]<'0' || argv[1][i-1]>'9')
			f=1;
		if (f==1)
			printf("To nie jest liczba\n");
		else{
			double c,d;
			c=atof(argv[1]);
			d=atof(argv[1]);
			double *wyn1, *wyn2;
			pthread_t w1,w2;
			pthread_create(&w1,NULL,wt1,&c);
			pthread_create(&w2,NULL,wt2,&d);
			pthread_join(w1,(void**)&wyn1);
			pthread_join(w2,(void**)&wyn2);
			printf("Km w milach: %f\n",*wyn1);
			printf("Km w minutach swietlnych: %.20f\n",*wyn2);
		}
	}
	return 0;
}
