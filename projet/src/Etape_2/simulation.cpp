#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <mpi.h>

#include "model.hpp"
#include "display.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

struct ParamsType
{
    double length{1.};
    unsigned discretization{20u};
    std::array<double,2> wind{0.,0.};
    Model::LexicoIndices start{10u,10u};
};

void analyze_arg( int nargs, char* args[], ParamsType& params )
{
    if (nargs ==0) return;
    std::string key(args[0]);
    if (key == "-l"s)
    {
        if (nargs < 2)
        {
            std::cerr << "Manque une valeur pour la longueur du terrain !" << std::endl;
            exit(EXIT_FAILURE);
        }
        params.length = std::stoul(args[1]);
        analyze_arg(nargs-2, &args[2], params);
        return;
    }
    auto pos = key.find("--longueur=");
    if (pos < key.size())
    {
        auto subkey = std::string(key,pos+11);
        params.length = std::stoul(subkey);
        analyze_arg(nargs-1, &args[1], params);
        return;
    }

    if (key == "-n"s)
    {
        if (nargs < 2)
        {
            std::cerr << "Manque une valeur pour le nombre de cases par direction pour la discrétisation du terrain !" << std::endl;
            exit(EXIT_FAILURE);
        }
        params.discretization = std::stoul(args[1]);
        analyze_arg(nargs-2, &args[2], params);
        return;
    }
    pos = key.find("--number_of_cases=");
    if (pos < key.size())
    {
        auto subkey = std::string(key, pos+18);
        params.discretization = std::stoul(subkey);
        analyze_arg(nargs-1, &args[1], params);
        return;
    }

    if (key == "-w"s)
    {
        if (nargs < 2)
        {
            std::cerr << "Manque une paire de valeurs pour la direction du vent !" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string values =std::string(args[1]);
        params.wind[0] = std::stod(values);
        auto pos = values.find(",");
        if (pos == values.size())
        {
            std::cerr << "Doit fournir deux valeurs séparées par une virgule pour définir la vitesse" << std::endl;
            exit(EXIT_FAILURE);
        }
        auto second_value = std::string(values, pos+1);
        params.wind[1] = std::stod(second_value);
        analyze_arg(nargs-2, &args[2], params);
        return;
    }
    pos = key.find("--wind=");
    if (pos < key.size())
    {
        auto subkey = std::string(key, pos+7);
        params.wind[0] = std::stoul(subkey);
        auto pos = subkey.find(",");
        if (pos == subkey.size())
        {
            std::cerr << "Doit fournir deux valeurs séparées par une virgule pour définir la vitesse" << std::endl;
            exit(EXIT_FAILURE);
        }
        auto second_value = std::string(subkey, pos+1);
        params.wind[1] = std::stod(second_value);
        analyze_arg(nargs-1, &args[1], params);
        return;
    }

    if (key == "-s"s)
    {
        if (nargs < 2)
        {
            std::cerr << "Manque une paire de valeurs pour la position du foyer initial !" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string values =std::string(args[1]);
        params.start.column = std::stod(values);
        auto pos = values.find(",");
        if (pos == values.size())
        {
            std::cerr << "Doit fournir deux valeurs séparées par une virgule pour définir la position du foyer initial" << std::endl;
            exit(EXIT_FAILURE);
        }
        auto second_value = std::string(values, pos+1);
        params.start.row = std::stod(second_value);
        analyze_arg(nargs-2, &args[2], params);
        return;
    }
    pos = key.find("--start=");
    if (pos < key.size())
    {
        auto subkey = std::string(key, pos+8);
        params.start.column = std::stoul(subkey);
        auto pos = subkey.find(",");
        if (pos == subkey.size())
        {
            std::cerr << "Doit fournir deux valeurs séparées par une virgule pour définir la vitesse" << std::endl;
            exit(EXIT_FAILURE);
        }
        auto second_value = std::string(subkey, pos+1);
        params.start.row = std::stod(second_value);
        analyze_arg(nargs-1, &args[1], params);
        return;
    }
}

ParamsType parse_arguments( int nargs, char* args[] )
{
    if (nargs == 0) return {};
    if ( (std::string(args[0]) == "--help"s) || (std::string(args[0]) == "-h") )
    {
        std::cout << 
R"RAW(Usage : simulation [option(s)]
  Lance la simulation d'incendie en prenant en compte les [option(s)].
  Les options sont :
    -l, --longueur=LONGUEUR     Définit la taille LONGUEUR (réel en km) du carré représentant la carte de la végétation.
    -n, --number_of_cases=N     Nombre n de cases par direction pour la discrétisation
    -w, --wind=VX,VY            Définit le vecteur vitesse du vent (pas de vent par défaut).
    -s, --start=COL,ROW         Définit les indices I,J de la case où commence l'incendie (milieu de la carte par défaut)
)RAW";
        exit(EXIT_SUCCESS);
    }
    ParamsType params;
    analyze_arg(nargs, args, params);
    return params;
}

bool check_params(ParamsType& params)
{
    bool flag = true;
    if (params.length <= 0)
    {
        std::cerr << "[ERREUR FATALE] La longueur du terrain doit être positive et non nulle !" << std::endl;
        flag = false;
    }

    if (params.discretization <= 0)
    {
        std::cerr << "[ERREUR FATALE] Le nombre de cellules par direction doit être positive et non nulle !" << std::endl;
        flag = false;
    }

    if ( (params.start.row >= params.discretization) || (params.start.column >= params.discretization) )
    {
        std::cerr << "[ERREUR FATALE] Mauvais indices pour la position initiale du foyer" << std::endl;
        flag = false;
    }
    
    return flag;
}

void display_params(ParamsType const& params)
{
    std::cout << "Parametres définis pour la simulation : \n"
              << "\tTaille du terrain : " << params.length << std::endl 
              << "\tNombre de cellules par direction : " << params.discretization << std::endl 
              << "\tVecteur vitesse : [" << params.wind[0] << ", " << params.wind[1] << "]" << std::endl
              << "\tPosition initiale du foyer (col, ligne) : " << params.start.column << ", " << params.start.row << std::endl;
}

int main( int nargs, char* args[] )
{
    MPI_Init( &nargs, &args );
	MPI_Comm globComm;
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	int nbp;
	MPI_Comm_size(globComm, &nbp);
	int rank;
	MPI_Comm_rank(globComm, &rank);
    MPI_Status status;

    auto params = parse_arguments(nargs-1, &args[1]);
    if (rank==0){
        display_params(params);
    }
    if (!check_params(params)) return EXIT_FAILURE;

    std::shared_ptr<Displayer> displayer;
    if (rank==0){
        displayer = Displayer::init_instance( params.discretization, params.discretization );
    }
    auto simu = Model( params.length, params.discretization, params.wind, params.start);
    std::vector<std::uint8_t> global_vegetal_map = simu.vegetal_map();
    std::vector<std::uint8_t> global_fire_map = simu.fire_map();
    unsigned vegetal_map_size = global_vegetal_map.size();;
    unsigned fire_map_size = global_fire_map.size();
    SDL_Event event;

    std::chrono::time_point < std::chrono::system_clock > start, end;
    std::chrono::duration < double >elapsed_seconds;
    std::chrono::time_point < std::chrono::system_clock > start_global, end_global;
    std::chrono::duration < double >average_step = (std::chrono::duration<double>)0.0;
    int count_step=0;
    std::chrono::duration < double >average_display = (std::chrono::duration<double>)0.0;
    int count_display=0;
    int loop=1;
    unsigned key=0;
    if (rank==0){
        start_global = std::chrono::system_clock::now();
    }
    while (loop)
    {
        if (rank==1){
            if ((simu.time_step() & 31) == 1) 
                start = std::chrono::system_clock::now();
            loop=simu.update();
            MPI_Send(&loop,1,MPI_INT,0,0,globComm);
            if ((simu.time_step() & 31) == 0){
                end = std::chrono::system_clock::now();
            }
            if (!loop){
                break;
            }
            global_vegetal_map = simu.vegetal_map();
            global_fire_map = simu.fire_map();
            MPI_Send(global_vegetal_map.data(),vegetal_map_size, MPI_UINT8_T,0,100,globComm);
            MPI_Send(global_fire_map.data(),fire_map_size,MPI_UINT8_T,0,101,globComm);
            if ((simu.time_step() & 31) == 0){
                elapsed_seconds = end - start;
                average_step = average_step + elapsed_seconds;
                count_step++;
                std::cout << " Time step " << simu.time_step() << " Time elapsed " << elapsed_seconds.count() << "\n" << std::endl;
            }
        }
        if (rank==0){
            MPI_Recv(&loop,1,MPI_UNSIGNED,1,0,globComm,&status);
            if (!loop){
                break;
            }
            std::vector<std::uint8_t> received_vegetal_map(vegetal_map_size);
            std::vector<std::uint8_t> received_fire_map(fire_map_size);
            MPI_Recv(received_vegetal_map.data(),vegetal_map_size, MPI_UINT8_T,1,100,globComm,&status);
            MPI_Recv(received_fire_map.data(),fire_map_size,MPI_UINT8_T,1,101,globComm,&status);
            for(int i=0;i<5;i++){
                key += received_vegetal_map.data()[i];
            }
            start = std::chrono::system_clock::now();
            displayer->update( received_vegetal_map, received_fire_map );
            end = std::chrono::system_clock::now();
            elapsed_seconds = end - start;
            average_display = average_display + elapsed_seconds;
            count_display++;
            //std::cout << " Time to print the window " << elapsed_seconds.count() << "\n===============" << std::endl;
            if (SDL_PollEvent(&event) && event.type == SDL_QUIT){
                MPI_Abort(MPI_COMM_WORLD, EXIT_SUCCESS);
            }
        }
        //std::this_thread::sleep_for(0.1s);
    }
    if (rank==0){
        end_global = std::chrono::system_clock::now();
        std::chrono::duration < double >elapsed_seconds_global = end_global - start_global;
        std::cout << " Global simulation time " << elapsed_seconds_global.count() << " Key " << key << "\n===============" << std::endl;
        average_display = average_display/count_display;
        std::cout << " Average display time " << average_display.count() << std::endl;
    }
    if (rank==1){
        average_step = average_step/count_step;
        std::cout << " Average step time " << average_step.count() << std::endl;
    }
    MPI_Finalize();
    return EXIT_SUCCESS;
}
