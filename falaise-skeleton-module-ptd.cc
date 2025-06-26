#include <bayeux/dpp/chain_module.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_cluster.h>
#include <falaise/snemo/datamodels/geomid_utils.h>  
#include <bayeux/mctools/base_step_hit.h>
#include <bayeux/mctools/simulated_data.h>
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <string>
#include "TLatex.h"
#include "TVector3.h"
#include "TSystem.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <cmath>

using namespace std;


class falaise_skeleton_module_ptd : public dpp::chain_module
{

public:
  // Constructor
  falaise_skeleton_module_ptd();

  // Destructor
  virtual ~falaise_skeleton_module_ptd();

  // Initialisation function
  virtual void initialize (const datatools::properties &,
                           datatools::service_manager &,
			   dpp::module_handle_dict_type &);

  // Event processing function
  dpp::chain_module::process_status process (datatools::things & event);
  
private:
  int event_number, electron_number, alpha_number, gamma_number;
  TFile *save_file;
  TTree *tree;
  
  // Macro to register the module
  DPP_MODULE_REGISTRATION_INTERFACE(falaise_skeleton_module_ptd);

};

////////////////////////////////////////////////////////////////////

// Macro to add the module in the global register of data processing modules:
// The module defined by this class 'falaise_skeleton_module_ptd' will be registered
// with the label ID 'FalaiseSkeletonModule_PTD' (to use in pipeline configuration file)
DPP_MODULE_REGISTRATION_IMPLEMENT(falaise_skeleton_module_ptd, "FalaiseSkeletonModule_PTD")


falaise_skeleton_module_ptd::falaise_skeleton_module_ptd()
{
  save_file = new TFile("extracted_data.root", "RECREATE");
  tree = new TTree("Event", "Event information");
  tree->Branch("event_number", &event_number);
  tree->Branch("electron_number", &electron_number);
  tree->Branch("alpha_number", &alpha_number);
  tree->Branch("gamma_number", &gamma_number);
  
}


falaise_skeleton_module_ptd::~falaise_skeleton_module_ptd()
{
  save_file->cd();
  tree->Write();
  save_file->Close();
  delete save_file;
  std::cout << "falaise_skeleton_module_ptd::~falaise_skeleton_module_ptd() called" << std::endl;
}


void falaise_skeleton_module_ptd::initialize (const datatools::properties & module_properties, datatools::service_manager &, dpp::module_handle_dict_type &)
{
  std::cout << "falaise_skeleton_module_ptd::initialize() called" << std::endl;
  event_number=0;

  this->_set_initialized(true);
}



dpp::chain_module::process_status falaise_skeleton_module_ptd::process (datatools::things & event)
{
   // Retrieve the PTD bank
  const snemo::datamodel::particle_track_data & PTD = event.get<snemo::datamodel::particle_track_data>("PTD");
  //In PTD bank, things are stored in particles, you need to loop on it
  for (const datatools::handle<snemo::datamodel::particle_track> & particle : PTD.particles()){
    bool vertex_close_to_the_source = false;
    bool vertex_associated_to_a_calo = false;
    //Each particle can have severals vertices, so we loop on this parameters too
    for(const datatools::handle<snemo::datamodel::vertex> & vertex : particle->get_vertices()){
      if(vertex->is_on_reference_source_plane()){
	vertex_close_to_the_source=1;
      }
      else if(vertex->is_on_main_calorimeter()){
	vertex_associated_to_a_calo = true;			  
      }
    }//end of loop on vertices
  
    if(vertex_close_to_the_source && vertex_associated_to_a_calo && particle->get_associated_calorimeter_hits().size()==1){ //an electron is defined by 2 vertices + 1 OM hit only
      electron_number++;
      //you can get a lots of information with this collection like time,energy...
      //const auto& calorimeter_hits = particle->get_associated_calorimeter_hits();
      //energy_elec.push_back(calorimeter_hits[0]->get_energy()); //you can take index 0 because we ask size==1 so only 1 calo hit
		    
    }
    
    else{ //if only one vertices -> alpha like track
      alpha_number++;
    }
  }//end of loop on particles
  if (PTD.hasIsolatedCalorimeters()) {//gamma look                                                  
    const snemo::datamodel::CalorimeterHitHdlCollection &cc_collection = PTD.isolatedCalorimeters();
    for (const auto &it_hit : cc_collection) {
      //you can get a lots of information with this collection like time,energy...
      // num_om.push_back(snemo::datamodel::om_num(it_hit->get_geom_id()));
      gamma_number++;

    }
  }

  
    tree->Fill();
    event_number++;      
  
    return dpp::base_module::PROCESS_SUCCESS;
}

