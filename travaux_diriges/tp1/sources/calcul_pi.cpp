# include <chrono>
# include <random>
# include <cstdlib>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>

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
  	if (nargs <= 1){
		std::cout << "Erreur : nombre de points non précisé" << std::endl;
		return EXIT_FAILURE;
	}
    unsigned long nbSamples = atoi(argv[1]);
  	std::chrono::time_point < std::chrono::system_clock > start, end;
  	start = std::chrono::system_clock::now();
  	double rep = approximate_pi(nbSamples);
  	end = std::chrono::system_clock::now();
  	std::chrono::duration < double >elapsed_seconds = end - start;
	std::cout << "Valeur de pi approximée : " << rep << std::endl;
    std::cout << "Temps CPU calcul de pi naif : " << elapsed_seconds.count() << " secondes\n";
	return EXIT_SUCCESS;
}

