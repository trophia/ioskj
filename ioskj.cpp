#define DEBUG 1

#include "imports.hpp"
#include "dimensions.hpp"
#include "model.hpp"
#include "parameters.hpp"
#include "data.hpp"
#include "tracker.hpp"
 
using namespace IOSKJ;

// Instantiate components
IOSKJ::Model model;
IOSKJ::Params params;
IOSKJ::Data data;

/**
 * Tasks that need to be done at startup
 */
void startup(void){
	// Read parameters and data
	params.read();
	data.read();
}

/**
 * Tasks that are usually done at shutdown
 */
void shutdown(void) {
	// Write parameters and data
	params.write();
	data.write();
	model.write();
}

/**
 * Run the model with a parameters set read from the "parameters/input/list.tsv" file
 */
void run(void){
	// Do tracking
	Tracker tracker("model/output/track.tsv");
	// For each time step...
	for(uint time=0;time<=time_max;time++){
		#if DEBUG
		std::cout<<time<<" "<<year(time)<<" "<<quarter(time)<<" "<<model.biomass_spawning_overall(quarter(time))<<std::endl;
		#endif
		//... set model parameters
		params.set(model,time);
		//... update the model
		model.update(time);
		//... get model variables corresponding to data
		data.get(model,time);
		//... get model variables of interest for tracking
		tracker.get(model,time);
	}
}

/**
 * Do slices of likelihoods for a parameter
 */
void profile(std::string parameter){ 
	//!auto set = params.read_set("parameters.tsv");
	//!params.profile(parameter,set,data,"profile.tsv");
}

int main(int argc, char** argv){
	startup();
	try{
		for(int arg=1;arg<argc;arg++){
            std::string task = argv[arg];
            std::cout<<"-------------Task-------------\n"
                    <<task<<"\n"
                    <<"-------------------------------\n";
            if(task=="run") run();
        }
	} catch(std::exception& error){
        std::cout<<"************Error************\n"
                <<error.what()<<"\n"
                <<"******************************\n";
	}
	shutdown();
	return 0;
}
