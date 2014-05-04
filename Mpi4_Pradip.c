#include <stdio.h>
#include "mpi.h"
#include <malloc.h>

#define tag_N_pe 0
#define tag 1
int main(argc, argv)
int argc;
char **argv;
{

	int N=100;
	int pid, pnum;
	int i,j,k;
	int **A, **B, **C_local;
	MPI_Status status;
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &pnum);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    
    int N_pe=N/pnum;//No. of rows per process
    int modulo=N%pnum;
    if (pid<modulo){
    	N_pe++;
    }
    /*int A[N_pe][N];
    int B[N][N];
    int C_local[N_pe][N];*/
    A = (int**)malloc(N_pe *sizeof(int *));
	for (i = 0; i < N_pe; i++){
		A[i] = (int*)malloc(N *sizeof(int*));
	}
	B = (int**)malloc(N *sizeof(int *));
	for (i = 0; i < N; i++){
		B[i] = (int*)malloc(N*sizeof(int*));
	}
	C_local = (int**)malloc(N_pe *sizeof(int *));
	for (i = 0; i < N_pe; i++){
		C_local[i] = (int*)malloc(N*sizeof(int*));
	}
	
// initializing matrices	
    srand(2006);
    for (i=0;i<N_pe;i++){
    	for (j=0;j<N;j++){
    		A[i][j]=rand();
    	}
    }
    for (i=0;i<N;i++){
    	for (j=0;j<N;j++){
    		B[i][j]=rand();
    	}
    }
    
  //  Computation
    int iter=0;
    for (i=0;i<N_pe;i++){
    	iter++;
    	if (pid==0){
    		printf("iter: %i\n", iter);
    	}
    	for(j=0;j<N;j++){
    		C_local[i][j]=0;
    		for (k=0;k<N;k++){
    			C_local[i][j]=C_local[i][j]+A[i][k]*B[k][j];
    		}
    	}
    }
  //  
    if (pid!=0){
    	MPI_Send(&N_pe, 1, MPI_INT, 0, tag_N_pe, MPI_COMM_WORLD);
    	MPI_Send(C_local[0], N_pe * N, MPI_INT, 0, tag, MPI_COMM_WORLD);	    		
    }
    if (pid==0){
    	int **C;
		C = (int**)malloc(N *sizeof(int *));
		for (i = 0; i < N; i++){
			C[i] = (int*)malloc(N*sizeof(int*));
		}
    	int rownum=0;
    	for (rownum=0;rownum<N_pe;rownum++){
    		for (j=0;j<N;j++){
    			C[rownum][j]=C_local[rownum][j];
    		}
    	}
    	for (i=1;i<pnum;i++){
    		MPI_Recv(&N_pe, 1, MPI_INT, i, tag_N_pe, MPI_COMM_WORLD,&status);
    		MPI_Recv(C[rownum], N_pe * N, MPI_INT, i, tag, MPI_COMM_WORLD,&status);	    
    		rownum=rownum+N_pe;	
    	}
  	}
  	
    MPI_Finalize();
}
