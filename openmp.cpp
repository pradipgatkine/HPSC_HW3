#include <time.h>
#include <iostream>
#include<stdlib.h>
#include <omp.h>
using namespace std;

using namespace std;

int N=0;

int main()
{
cout<< "Enter the N (100/1000/10000):";
cin>>N;

clock_t tStart = clock();
int A[N][N];
int B[N][N];
int C[N][N];

// Initialization of Arrays
for (int i=0; i<N; i++)
{
  for (int j=0; j<1; j++)
 	{	
  	A[i][j] = rand() % 1000 + 1;
	B[i][j] = rand() % 1000 + 1;
	}
}


// Computing the product

#pragma omp parallel for num_threads(2)
for (int i=0; i<N; i++){
for (int j=0; j<N; j++){

C[i][j] =0;

for (int r=0; r<N; r++){
C[i][j]= C[i][j] + A[i][r]*B[r][j];
}

}
}

double t = double((clock() - tStart))/CLOCKS_PER_SEC;
cout<<"Time taken:"<<t<<"s\n";
return 0;
}

