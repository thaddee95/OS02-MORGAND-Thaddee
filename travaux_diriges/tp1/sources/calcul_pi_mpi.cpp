# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <mpi.h>

// Attention , ne marche qu'en C++ 11 ou supérieur :
double approximate_pi( unsigned long nbSamples ) 
{
    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration d = beginning.time_since_epoch();
    unsigned seed = d.count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution <double> distribution ( -1.0 ,1.0);
    unsigned long nbDarts = 0;
    // Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
    for ( unsigned sample = 0 ; sample < nbSamples ; ++ sample ) {
        double x = distribution(generator);
        double y = distribution(generator);
        // Test if the dart is in the unit disk
        if ( x*x+y*y<=1 ) nbDarts ++;
    }
    // Number of nbDarts throwed in the unit disk
    double ratio = double(nbDarts)/double(nbSamples);
    return 4*ratio;
}

int main( int nargs, char* argv[] )
{
	// On initialise le contexte MPI qui va s'occuper :
	//    1. Créer un communicateur global, COMM_WORLD qui permet de gérer
	//       et assurer la cohésion de l'ensemble des processus créés par MPI;
	//    2. d'attribuer à chaque processus un identifiant ( entier ) unique pour
	//       le communicateur COMM_WORLD
	//    3. etc...
	MPI_Init( &nargs, &argv );
	// Pour des raisons de portabilité qui débordent largement du cadre
	// de ce cours, on préfère toujours cloner le communicateur global
	// MPI_COMM_WORLD qui gère l'ensemble des processus lancés par MPI.
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	// On interroge le communicateur global pour connaître le nombre de processus
	// qui ont été lancés par l'utilisateur :
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	// On interroge le communicateur global pour connaître l'identifiant qui
	// m'a été attribué ( en tant que processus ). Cet identifiant est compris
	// entre 0 et nbp-1 ( nbp étant le nombre de processus qui ont été lancés par
	// l'utilisateur )
	int rank;
	MPI_Comm_rank(globComm, &rank);
	
  	if (nargs <= 1){
		std::cout << "Erreur : nombre de points non précisé" << std::endl;
		return EXIT_FAILURE;
	}
	unsigned long nbSamples_total = atoi(argv[1]);
	unsigned long nbSamples_thread=nbSamples_total/nbp;
	unsigned long nbSamples_thread0=nbSamples_total-(nbp-1)*nbSamples_thread;
	unsigned long nbDarts_total=0;

	if (rank==0){
		std::chrono::time_point < std::chrono::system_clock > start, end;
  		start = std::chrono::system_clock::now();
		typedef std::chrono::high_resolution_clock myclock;
		myclock::time_point beginning = myclock::now();
		myclock::duration d = beginning.time_since_epoch();
		unsigned seed = d.count();
		std::default_random_engine generator(seed);
		std::uniform_real_distribution <double> distribution ( -1.0 ,1.0);
		unsigned long nbDarts = 0;
		// Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
		for ( unsigned sample = 0 ; sample < nbSamples_thread0 ; ++ sample ) {
			double x = distribution(generator);
			double y = distribution(generator);
			// Test if the dart is in the unit disk
			if ( x*x+y*y<=1 ) nbDarts ++;
		}
		MPI_Reduce(&nbDarts,&nbDarts_total,1,MPI_UNSIGNED_LONG,MPI_SUM,0,MPI_COMM_WORLD);
		// Number of nbDarts throwed in the unit disk
		double ratio = double(nbDarts_total)/double(nbSamples_total);
		double rep = 4*ratio;
		end = std::chrono::system_clock::now();
  		std::chrono::duration < double >elapsed_seconds = end - start;
		std::cout << "Valeur de pi approximée : " << rep << std::endl;
    	std::cout << "Temps CPU calcul de pi avec MPI : " << elapsed_seconds.count() << " secondes\n";
	}
	else{
		typedef std::chrono::high_resolution_clock myclock;
		myclock::time_point beginning = myclock::now();
		myclock::duration d = beginning.time_since_epoch();
		unsigned seed = d.count();
		std::default_random_engine generator(seed);
		std::uniform_real_distribution <double> distribution ( -1.0 ,1.0);
		unsigned long nbDarts = 0;
		// Throw nbSamples darts in the unit square [-1 :1] x [-1 :1]
		for ( unsigned sample = 0 ; sample < nbSamples_thread ; ++ sample ) {
			double x = distribution(generator);
			double y = distribution(generator);
			// Test if the dart is in the unit disk
			if ( x*x+y*y<=1 ) nbDarts ++;
		}
		MPI_Reduce(&nbDarts,&nbDarts_total,1,MPI_UNSIGNED_LONG,MPI_SUM,0,MPI_COMM_WORLD);
	}

	// A la fin du programme, on doit synchroniser une dernière fois tous les processus
	// afin qu'aucun processus ne se termine pendant que d'autres processus continue à
	// tourner. Si on oublie cet instruction, on aura une plantage assuré des processus
	// qui ne seront pas encore terminés.
	MPI_Finalize();
	return EXIT_SUCCESS;
}

