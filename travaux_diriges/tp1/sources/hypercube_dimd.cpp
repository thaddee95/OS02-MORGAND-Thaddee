# include <iostream>
# include <mpi.h>
# include <cmath>

unsigned int pow2_inf(unsigned int x) {
    if (x == 0) return 0;  // Cas spécial pour x = 0
    //if (x % 2 ==0) return 0;
    return std::pow(2,std::floor(std::log2(x)));
}

unsigned int pow2_sup(unsigned int x) {
    if (x == 0) return 1;  // Cas spécial pour x = 0
    if (x == 1) return 2;
    if (x % 2 == 0) return 2*x;
    return std::pow(2, std::ceil(std::log2(x)));
}

int main(int nargs, char* argv[]){
    MPI_Init( &nargs, &argv );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);

  	if (nargs <= 2){
		std::cout << "Erreur : dimension et/ou valeur à diffuser non précisée(s)" << std::endl;
		return EXIT_FAILURE;
	}
    int dim=atoi(argv[1]);
    if (nbp != pow(2,dim)){
        std::cout << "Erreur : en dimension " << dim << ", on utilise " << pow(2,dim) << " threads exactement" << std::endl;
        return EXIT_FAILURE;
    }

    int jeton=0;
    MPI_Status status;
    int value=atoi(argv[2]);

    if(rank==0){
        jeton=value;
        for(unsigned int i=1;i<nbp;i=i*2){
            MPI_Send(&jeton,1,MPI_INT,i,0,globComm);
            std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à " << i << std::endl;
        }
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    else{
        MPI_Recv(&jeton,1,MPI_INT,rank-pow2_inf(rank),0,globComm,&status);
        std::cout << "Je suis " << rank << " j'ai reçu " << jeton << std::endl;
        for(unsigned int i=pow2_sup(rank);i<nbp;i=i*2){
            MPI_Send(&jeton,1,MPI_INT,i+rank,0,globComm);
            std::cout << "Je suis " << rank << " j'ai envoyé " << jeton << " à " << i+rank << std::endl;
        }
        MPI_Barrier(globComm);
        std::cout << "Je suis " << rank << " mon jeton vaut " << jeton << std::endl;
    }
    
    MPI_Finalize();
    return 0;
}