# include <iostream>
# include <mpi.h>

int main(int nargs, char* argv[]){
    MPI_Init( &nargs, &argv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);

    int jeton=1;
    MPI_Status status;

    if(rank==0){
        MPI_Send(&jeton,1,MPI_INT,rank+1,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << std::endl;
        MPI_Recv(&jeton,1,MPI_INT,nbp-1,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
    }
    else{
        MPI_Recv(&jeton,1,MPI_INT,rank-1,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        jeton++;
        MPI_Send(&jeton,1,MPI_INT,(rank+1)%nbp,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << std::endl;
    }

    MPI_Finalize();
    return 0;
}

/* attention compiler avec mpic++:
mpic++ -o jeton.exe jeton.cpp
mpirun -np 4 ./jeton.exe pour 4 processeurs
*/