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

  	if (nargs <= 1){
		std::cout << "Erreur : valeur à diffuser non précisée" << std::endl;
		return EXIT_FAILURE;
	}
    if (nbp != 8){
        std::cout << "Erreur : en dimension 3, on utilise 8 threads exactement" << std::endl;
        return EXIT_FAILURE;
    }

    int jeton=0;
    MPI_Status status;
    int value=atoi(argv[1]);

    if(rank==0){
        jeton=value;
        MPI_Send(&jeton,1,MPI_INT,1,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à 1" << std::endl;
        MPI_Send(&jeton,1,MPI_INT,2,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à 2" << std::endl;
        MPI_Send(&jeton,1,MPI_INT,4,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à 4" << std::endl;
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    if(rank==1){
        MPI_Recv(&jeton,1,MPI_INT,0,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        MPI_Send(&jeton,1,MPI_INT,3,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à 3" << std::endl;
        MPI_Send(&jeton,1,MPI_INT,5,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à 5" << std::endl;
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    if(rank==2){
        MPI_Recv(&jeton,1,MPI_INT,0,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        MPI_Send(&jeton,1,MPI_INT,6,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à 6" << std::endl;
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    if(rank==3){
        MPI_Recv(&jeton,1,MPI_INT,1,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        MPI_Send(&jeton,1,MPI_INT,7,0,globComm);
        std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à 7" << std::endl;
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    if(rank==4){
        MPI_Recv(&jeton,1,MPI_INT,0,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    if(rank==5){
        MPI_Recv(&jeton,1,MPI_INT,1,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    if(rank==6){
        MPI_Recv(&jeton,1,MPI_INT,2,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    if(rank==7){
        MPI_Recv(&jeton,1,MPI_INT,3,0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    MPI_Finalize();
    return 0;
}