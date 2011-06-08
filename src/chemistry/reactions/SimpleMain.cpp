#include <iostream>
#include <vector>

#include "Beaker.hpp"
#include "Glenn.hpp"
#include "activity-model-factory.hh"
#include "verbosity.hh"

using namespace std;

int main (int argc, char **argv) {
  static_cast<void>(argc);
  static_cast<void>(argv);

  std::string filename;

  Verbosity verbose = kVerbose;

  // create geochemistry object
  Beaker beaker;
  beaker.verbosity(verbose);

  Beaker::BeakerComponents components;
  components.free_ion.clear();
  components.minerals.clear();
  components.ion_exchange_sites.clear();
  components.total.clear();
  components.total_sorbed.clear();

  Beaker::BeakerParameters parameters = beaker.GetDefaultParameters();
  parameters.porosity = 0.25;
  parameters.saturation = 1.;
  parameters.water_density = 997.205133945901; // kg / m^3
  parameters.volume = 0.25; // m^3

  beaker.SetupActivityModel(ActivityModelFactory::debye_huckel);
  //beaker.SetupActivityModel(ActivityModelFactory::unit);

#if 0
  // set up simple 2-species carbonate system (H,HCO3-)
  createCarbonateSystem(&(components.total), &beaker);
#else
  filename = "reaction.dat";
  readChemistryFromFile(filename,&beaker);

  filename = "target_total.dat";
  readTargetTotalFromFile(filename,beaker.ncomp(), &components.total) ;
  // convert totals from molality [mol/kg water] -> molarity [mol/L water]
  for (unsigned int i = 0; i < components.total.size(); i++)
    components.total[i] *= parameters.water_density/1000.;
  components.total_sorbed.resize(beaker.ncomp());

  filename = "target_total.dat";
  readTargetFreeIonFromFile(filename,beaker.ncomp(), &components.free_ion) ;
#endif
  // solve for free-ion concentrations

  // to test react with unitary time step
//  beaker.ReactionStep(components, parameters, 1.0);
//  beaker.print_results();

  // to test speciation
//  beaker.Speciate(components, parameters);
//  beaker.print_results();

  // to test a time stepping loop with kinetic reactions
  Glenn g(&beaker);

  double final_time = 0.25 * 365. * 24. * 3600; // 1/4 year
  double time_step = final_time / 100;

  g.solve(&components, final_time, time_step, parameters);

  cout << "Done!\n";

}
