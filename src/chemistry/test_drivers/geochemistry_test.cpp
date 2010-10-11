/* -*-  mode: c++; c-default-style: "google"; indent-tabs-mode: nil -*- */
#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include "ThermoDatabase.hpp"
#include "SimpleCarbonate.hpp"
#include "LargeCarbonate.hpp"
#include "Beaker.hpp"
#include "ActivityModelFactory.hpp"
#include "Verbosity.hpp"

int CommandLineOptions(int argc, char **argv, Verbosity& verbosity, int& test);
void PrintDoubleVector(const std::vector<double> &total);

int main(int argc, char **argv) {
  Verbosity verbosity = kTerse;
  int test = 0;
  int error = EXIT_SUCCESS;

  Beaker *chem = NULL;
  std::string activity_model_name;
  std::vector<KineticRate*> mineral_rates;
  mineral_rates.clear();
  std::string mineral_kinetics_file;
  mineral_kinetics_file.clear();

  error = CommandLineOptions(argc, argv, verbosity, test);

  std::vector<double> tot;
  Beaker* database = new ThermoDatabase();
  database->verbosity(kVerbose);
  database->setup(tot, mineral_kinetics_file);
  database->display();
  return EXIT_FAILURE;

  if (error == EXIT_SUCCESS) {
    switch (test) {
      case 1: {
        // set up simple 2-species carbonate system (H,HCO3-) unit activity coefficients
        if (verbosity >= kTerse) {
          std::cout << "Running simple carbonate example, unit activity coefficients." << std::endl;
        }
        chem = new SimpleCarbonate();
        activity_model_name = ActivityModelFactory::unit;
        break;
      }
      case 2: {
        // set up simple 2-species carbonate system (H,HCO3-), debye-huckel activity coefficients
        if (verbosity >= kTerse) {
          std::cout << "Running simple carbonate example, debye-huckel." << std::endl;
        }
        chem = new SimpleCarbonate();
        activity_model_name = ActivityModelFactory::debye_huckel;
        break;
      }
      case 3: {
        // larger carbonate system, 3 components, 9 secondary, unit activity coefficients
        if (verbosity >= kTerse) {
          std::cout << "Running large carbonate speciation example, unit activity coefficients." << std::endl;
        }
        chem = new LargeCarbonate();
        activity_model_name = ActivityModelFactory::unit;
        break;
      }
      case 4: {
        // larger carbonate system, 3 components, 9 secondary, debye-huckel activity coefficients
        if (verbosity >= kTerse) {
          std::cout << "Running large carbonate speciation example, debye-huckel activity coefficients." << std::endl;
        }
        chem = new LargeCarbonate();
        activity_model_name = ActivityModelFactory::debye_huckel;
        break;
      }
      case 5: {
        // calcite TST kinetics
        if (verbosity >= kTerse) {
          std::cout << "Running calcite kinetics tst problem." << std::endl;
        }
        chem = new LargeCarbonate();
        activity_model_name = ActivityModelFactory::debye_huckel;
        mineral_kinetics_file = "input/calcite-kinetics-tst.ain";
        break;
      }
      default: {
        std::cout << "Invalid test number specified on command line. try using the \'-h\' option." << std::endl;
        break;
      }
    }
  }

  if (chem != NULL) {
    std::vector<double> total;
    chem->verbosity(verbosity);
    Beaker::BeakerParameters parameters = chem->GetDefaultParameters();
    parameters.water_density = 997.205133945901; // kg / m^3
    chem->SetupActivityModel(activity_model_name);
    chem->setup(total, mineral_kinetics_file);
    if (verbosity >= kVerbose) {
      chem->display();
    }

    // solve for free-ion concentrations
    chem->speciate(total, parameters.water_density);
    if (verbosity >= kTerse) {
      chem->print_results();
    }
    if (mineral_kinetics_file.size() != 0) {
      std::cout << "----- Test Beaker Reaction Step -----" << std::endl;
      std::cout << "initial total: " << std::endl;
      PrintDoubleVector(total);
      chem->ReactionStep(total, parameters, 3600.0);
      std::cout << "final total: " << std::endl;
      PrintDoubleVector(total);
    }
  }

  // cleanup memory
  if (chem != NULL) {
    delete chem;
  }

  std::cout << "Done!\n";
}  // end main()


void PrintDoubleVector(const std::vector<double> &total)
{
  std::cout << "[ ";
  std::vector<double>::const_iterator i;
  for (i = total.begin(); i != total.end(); i++) {
    std::cout << std::scientific << std::setprecision(10) << *i << ", ";
  }
  std::cout << " ]" << std::endl;
}  // end PrintDoubleVector()

int CommandLineOptions(int argc, char **argv, Verbosity& verbosity, int& test)
{
  int error = -2;
  int option;
  extern char *optarg;

  while ((option = getopt(argc, argv, "ht:v:?")) != EOF) {
    switch (option) {
      case 't': {
        /* specify the test that should be run */
        test = std::atoi(optarg);
        error = EXIT_SUCCESS;
        break;
      }
      case 'v': {
        verbosity = static_cast<Verbosity>(std::atoi(optarg));
        break;
      }
      case '?': case 'h': {  /* help mode */
        /* print some help stuff and exit without doing anything */
        std::cout << argv[0] << " command line options:" << std::endl;
        std::cout << "    -t integer " << std::endl
                  << "         run a test case. valid test numbers are: " << std::endl
                  << "             1: simple carbonate speciation, unit activity coeff" << std::endl
                  << "             2: simple carbonate speciation, debye-huckel" << std::endl
                  << "             3: larger carbonate speciation, unit activity coeff" << std::endl
                  << "             4: larger carbonate speciation, debye-huckel" << std::endl
                  << "             5: calcite kinetics, TST rate law" << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "    -v integer" << std::endl;
        std::cout << "         verbose output:" << std::endl;
        std::cout << "             0: silent" << std::endl;
        std::cout << "             1: terse" << std::endl;
        std::cout << "             2: verbose" << std::endl;
        std::cout << "             3: debug" << std::endl;
        std::cout << "             4: debug beaker" << std::endl;
        std::cout << "             5: debug mineral kinetics" << std::endl;
        error = -1;
        break;
      }
      default: {
        /* no options */
        break;
      }
    }
  }

  if (error != -1 && test == 0) {
    std::cout << "No test number specified on command line. Try \""
              <<  argv[0] << " -h \" for help." << std::endl;
  }

  if (verbosity >= kVerbose) {
    std::cout << "Command Line Options: " << std::endl;
    std::cout << "\tTest number: " << test << std::endl;
    std::cout << "\tVerbosity: " << verbosity << std::endl;
  }
  std::cout << std::endl << std::endl;

  return error;
}  // end commandLineOptions()

