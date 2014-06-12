/* -*-  mode: c++; c-default-style: "google"; indent-tabs-mode: nil -*- */
/* -------------------------------------------------------------------------
Amanzi Chemistry

License: see COPYRIGHT
Author: Jeffrey Johnson

This implements the Alquimia chemistry engine.

 ------------------------------------------------------------------------- */

#include <iostream>
#include <cstring>
#include <assert.h>
#include "ChemistryEngine.hh"
#include "errors.hh"
#include "exceptions.hh"

// Support for manipulating floating point exception handling.
#ifdef _GNU_SOURCE
#define AMANZI_USE_FENV
#include <fenv.h>
#endif

namespace Amanzi {
namespace AmanziChemistry {


ChemistryEngine::ChemistryEngine(const std::string& engineName, 
                                 const std::string& inputFile):
  chem_engine_name_(engineName),
  chem_engine_inputfile_(inputFile)
{
  Errors::Message msg;

  if (chem_engine_name_ != "PFloTran")
  {
    msg << "ChemistryEngine: Unsupported chemistry engine: '" << chem_engine_name_ << "'\n";
    msg << "  Options are 'PFlotran'.\n";
    Exceptions::amanzi_throw(msg);
  }

  // All alquimia function calls require a status object.
  AllocateAlquimiaEngineStatus(&chem_status_);
  CreateAlquimiaInterface(chem_engine_name_.c_str(), &chem_, &chem_status_);
  if (chem_status_.error != 0) 
  {
    std::cout << chem_status_.message << std::endl;
    msg << "ChemistryEngine: Could not create an interface to Alquimia.";
    Exceptions::amanzi_throw(msg); 
  }

  // Set up Alquimia, get sizes for data.
  chem_.Setup(chem_engine_inputfile_.c_str(),
              &engine_state_,
              &sizes_,
              &functionality_,
              &chem_status_);
  if (chem_status_.error != 0) 
  {
    std::cout << chem_status_.message << std::endl;
    PrintAlquimiaSizes(&sizes_);
    msg << "Error in creation of ChemistryEngine.";
    Exceptions::amanzi_throw(msg); 
  }

  // Allocate storage for additional Alquimia data.
  AllocateAlquimiaProblemMetaData(&sizes_, &chem_metadata_);

  // Get the problem metadata (species and mineral names, etc).
  chem_.GetProblemMetaData(&engine_state_,
                           &chem_metadata_,
                           &chem_status_);
  if (chem_status_.error != 0) 
  {
    std::cout << chem_status_.message << std::endl;
    PrintAlquimiaProblemMetaData(&chem_metadata_);
    msg << "Error in ChemistryEngine::Initialize";
    Exceptions::amanzi_throw(msg); 
  }
}

ChemistryEngine::~ChemistryEngine()
{
  chem_.Shutdown(&engine_state_, &chem_status_);
  FreeAlquimiaProblemMetaData(&chem_metadata_);

  // Delete the various geochemical conditions.
  for (std::map<std::string, AlquimiaGeochemicalCondition*>::iterator 
       iter = chem_conditions_.begin(); iter != chem_conditions_.end(); ++iter)
  {
    FreeAlquimiaGeochemicalCondition(iter->second);
    delete iter->second;
  }

  FreeAlquimiaEngineStatus(&chem_status_);
}

const std::string& ChemistryEngine::Name() const
{
  return chem_engine_name_;
}

bool ChemistryEngine::IsThreadSafe() const
{
  Errors::Message msg;

  if (not chem_initialized_)
  {
    msg << "ChemistryEngine: Cannot query before initialization!";
    Exceptions::amanzi_throw(msg); 
  }

  return functionality_.thread_safe;
}

int ChemistryEngine::NumPrimarySpecies() const
{
  return sizes_.num_primary;
}

int ChemistryEngine::NumAqueousComplexes() const
{
  return sizes_.num_aqueous_complexes;
}

int ChemistryEngine::NumSorbedSpecies() const
{
  return sizes_.num_sorbed;
}

void ChemistryEngine::GetPrimarySpeciesNames(std::vector<std::string>& speciesNames) const
{
  const AlquimiaProblemMetaData* metadata = &chem_metadata_;
  int N = metadata->primary_names.size;
  speciesNames.resize(N);
  for (int i = 0; i < N; ++i)
    speciesNames[i] = std::string(metadata->primary_names.data[i]);
}

int ChemistryEngine::NumMinerals() const
{
  return sizes_.num_kinetic_minerals;
}

void ChemistryEngine::GetMineralNames(std::vector<std::string>& mineralNames) const
{
  const AlquimiaProblemMetaData* metadata = &chem_metadata_;
  int N = metadata->mineral_names.size;
  mineralNames.resize(N);
  for (int i = 0; i < N; ++i)
    mineralNames[i] = std::string(metadata->mineral_names.data[i]);
}

int ChemistryEngine::NumSurfaceSites() const
{
  return sizes_.num_surface_sites;
}

void ChemistryEngine::GetSurfaceSiteNames(std::vector<std::string>& siteNames) const
{
  const AlquimiaProblemMetaData* metadata = &chem_metadata_;
  int N = metadata->surface_site_names.size;
  siteNames.resize(N);
  for (int i = 0; i < N; ++i)
    siteNames[i] = std::string(metadata->surface_site_names.data[i]);
}

int ChemistryEngine::NumIonExchangeSites() const
{
  return sizes_.num_ion_exchange_sites;
}

void ChemistryEngine::GetIonExchangeNames(std::vector<std::string>& ionExchangeNames) const
{
  const AlquimiaProblemMetaData* metadata = &chem_metadata_;
  int N = metadata->ion_exchange_names.size;
  ionExchangeNames.resize(N);
  for (int i = 0; i < N; ++i)
    ionExchangeNames[i] = std::string(metadata->ion_exchange_names.data[i]);
}

int ChemistryEngine::NumIsothermSpecies() const
{
  return sizes_.num_isotherm_species;
}

void ChemistryEngine::GetIsothermSpeciesNames(std::vector<std::string>& speciesNames) const
{
  const AlquimiaProblemMetaData* metadata = &chem_metadata_;
  int N = metadata->isotherm_species_names.size;
  speciesNames.resize(N);
  for (int i = 0; i < N; ++i)
    speciesNames[i] = std::string(metadata->isotherm_species_names.data[i]);
}

int ChemistryEngine::NumFreeIonSpecies() const
{
  return sizes_.num_primary;
}

void ChemistryEngine::CreateCondition(const std::string& condition_name)
{
  // NOTE: a condition with zero aqueous/mineral constraints is assumed to be defined in 
  // NOTE: the backend engine's input file. 
  AlquimiaGeochemicalCondition* condition = new AlquimiaGeochemicalCondition();
  int num_aq = 0, num_min = 0;
  AllocateAlquimiaGeochemicalCondition(kAlquimiaMaxStringLength, num_aq, num_min, condition);
  std::strcpy(condition->name, condition_name.c_str());

  // Add this to the conditions map.
  chem_conditions_[condition_name] = condition;
}

void ChemistryEngine::AddMineralConstraint(const std::string& condition_name,
                                           const std::string& mineral_name,
                                           double volume_fraction,
                                           double specific_surface_area)
{
  assert(condition_name.length() > 0);
  assert(volume_fraction >= 0.0);
  assert(specific_surface_area >= 0.0);

  std::map<std::string, AlquimiaGeochemicalCondition*>::iterator iter = chem_conditions_.find(condition_name);
  if (iter != chem_conditions_.end())
  {
    AlquimiaGeochemicalCondition* condition = iter->second;

    // Do we have an existing constraint?
    int index = -1;
    for (int i = 0; i < condition->mineral_constraints.size; ++i)
    {
      if (!std::strcmp(condition->mineral_constraints.data[i].mineral_name, mineral_name.c_str()))
      {
        // Overwrite the old constraint.
        index = i;
        free(condition->mineral_constraints.data[index].mineral_name);
      }
    }
    if (index == -1)
    {
      // New constraint!
      index = condition->mineral_constraints.size;
      condition->mineral_constraints.size++;
      condition->mineral_constraints.data = (AlquimiaMineralConstraint*)realloc(condition->mineral_constraints.data, sizeof(AlquimiaMineralConstraint) * (index+1));
    }

    // Add the mineral constraint.
    condition->mineral_constraints.data[index].mineral_name = strdup(mineral_name.c_str());
    condition->mineral_constraints.data[index].volume_fraction = volume_fraction;
    condition->mineral_constraints.data[index].specific_surface_area = specific_surface_area;
  }
  else
  {
    Errors::Message msg;
    msg << "ChemistryEngine::AddMineralConstraint: no condition named '" << condition_name << "'.";
    Exceptions::amanzi_throw(msg); 
  }
}

void ChemistryEngine::AddAqueousConstraint(const std::string& condition_name,
                                           const std::string& primary_species_name,
                                           const std::string& constraint_type,
                                           const std::string& associated_species)
{
  assert(condition_name.length() > 0);
  assert(primary_species_name.length() > 0);
  assert((constraint_type == "total_aqueous") || (constraint_type == "charge") || 
         (constraint_type == "free") || (constraint_type == "mineral") ||
         (constraint_type == "gas") || (constraint_type == "pH"));

  std::map<std::string, AlquimiaGeochemicalCondition*>::iterator iter = chem_conditions_.find(condition_name);
  if (iter != chem_conditions_.end())
  {
    AlquimiaGeochemicalCondition* condition = iter->second;

    // Is there a mineral constraint for the associated species?
    if (!associated_species.empty())
    {
      bool found_mineral = false;
      for (int i = 0; i < condition->mineral_constraints.size; ++i)
      {
        if (!std::strcmp(condition->mineral_constraints.data[i].mineral_name, associated_species.c_str()))
          found_mineral = true;
      }
      if (!found_mineral)
      {
        Errors::Message msg;
        msg << "ChemistryEngine::AddAqueousConstraint: the condition '" << condition_name << "' does not have a mineral constraint for '" << associated_species << "'.";
        Exceptions::amanzi_throw(msg); 
      }
    }

    // Do we have an existing constraint?
    int index = -1;
    for (int i = 0; i < condition->aqueous_constraints.size; ++i)
    {
      if (!std::strcmp(condition->aqueous_constraints.data[i].primary_species_name, primary_species_name.c_str()))
      {
        // Overwrite the old constraint.
        index = i;
        free(condition->aqueous_constraints.data[index].primary_species_name);
        free(condition->aqueous_constraints.data[index].constraint_type);
        if (condition->aqueous_constraints.data[index].associated_species != NULL)
          free(condition->aqueous_constraints.data[index].associated_species);
      }
    }
    if (index == -1)
    {
      // New constraint!
      index = condition->aqueous_constraints.size;
      condition->aqueous_constraints.size++;
      condition->aqueous_constraints.data = (AlquimiaAqueousConstraint*)realloc(condition->aqueous_constraints.data, sizeof(AlquimiaAqueousConstraint) * (index+1));
    }

    // Add the aqueous constraint.
    condition->aqueous_constraints.data[index].primary_species_name = strdup(primary_species_name.c_str());
    condition->aqueous_constraints.data[index].constraint_type = strdup(constraint_type.c_str());
    if (!associated_species.empty())
      condition->aqueous_constraints.data[index].associated_species = strdup(associated_species.c_str());
    else
      condition->aqueous_constraints.data[index].associated_species = NULL;
  }
  else
  {
    Errors::Message msg;
    msg << "ChemistryEngine::AddAqueousConstraint: no condition named '" << condition_name << "'.";
    Exceptions::amanzi_throw(msg); 
  }
}

void ChemistryEngine::EnforceCondition(const std::string& condition_name,
                                       const double time,
                                       const AlquimiaMaterialProperties& mat_props,
                                       AlquimiaState& chem_state,
                                       AlquimiaAuxiliaryData& aux_data,
                                       AlquimiaAuxiliaryOutputData& aux_output)
{
  Errors::Message msg;

  // Retrieve the chemical condition for the given name.
  std::map<std::string, AlquimiaGeochemicalCondition*>::iterator iter = chem_conditions_.find(condition_name);
  if (iter == chem_conditions_.end())
  {
    CreateCondition(condition_name);
    iter = chem_conditions_.find(condition_name);
  }

#ifdef AMANZI_USE_FENV
  // Disable divide-by-zero floating point exceptions.
  int fpe_mask = fedisableexcept(FE_DIVBYZERO);
#endif 

  // Process the condition on the given array at the given time.
  // FIXME: Time is ignored for the moment.
  int ierr = 0;
  AlquimiaGeochemicalCondition* condition = iter->second;
  chem_.ProcessCondition(&engine_state_,
                         condition,
                         &(const_cast<AlquimiaMaterialProperties&>(mat_props)),
                         &chem_state,
                         &aux_data,
                         &chem_status_);

#ifdef AMANZI_USE_FENV
  // Re-enable pre-existing floating point exceptions.
  feclearexcept(fpe_mask);
  fpe_mask = feenableexcept(fpe_mask);
#endif 

// FIXME: Figure out a neutral parallel-friendly way to report errors.
  assert(chem_status_.error == 0);

#if 0
  if (chem_status_.error != 0)
    ierr = -1;

  // figure out if any of the processes threw an error, if so all processes will re-throw
  int recv = 0;
  mesh_->get_comm()->MaxAll(&ierr, &recv, 1);
  if (recv != 0) 
  {
    msg << "Error in enforcement of chemical condition '" << condition_name << "'";
    Exceptions::amanzi_throw(msg); 
  }  
#endif
}

void ChemistryEngine::Advance(const double delta_time,
                              const AlquimiaMaterialProperties& mat_props,
                              AlquimiaState& chem_state,
                              AlquimiaAuxiliaryData& aux_data,
                              AlquimiaAuxiliaryOutputData& aux_output,
                              int& num_iterations)
{
#ifdef AMANZI_USE_FENV
  // Disable divide-by-zero floating point exceptions.
  int fpe_mask = fedisableexcept(FE_DIVBYZERO);
#endif 

  // Advance the chemical reaction all operator-split-like.
  chem_.ReactionStepOperatorSplit(&engine_state_,
                                  (const_cast<double*>(&delta_time)),
                                  &(const_cast<AlquimiaMaterialProperties&>(mat_props)),
                                  &chem_state,
                                  &aux_data,
                                  &chem_status_);

#ifdef AMANZI_USE_FENV
  // Re-enable pre-existing floating point exceptions.
  feclearexcept(fpe_mask);
  fpe_mask = feenableexcept(fpe_mask);
#endif 

// FIXME: Figure out a neutral parallel-friendly way to report errors.
  assert(chem_status_.error == 0);

#if 0
  if (chem_status_.error != 0)
    ierr = -1;

  // figure out if any of the processes threw an error, if so all processes will re-throw
  int recv = 0;
  mesh_->get_comm()->MaxAll(&ierr, &recv, 1);
  if (recv != 0) 
  {
    msg << "Error in advance of chemical reactions.";
    Exceptions::amanzi_throw(msg); 
  }  
#endif

  // Write down the number of Newton iterations.
  num_iterations = chem_status_.num_newton_iterations;
}

const AlquimiaSizes& ChemistryEngine::Sizes() const
{
  return sizes_;
}

void ChemistryEngine::InitState(AlquimiaMaterialProperties& mat_props,
                                AlquimiaState& chem_state, 
                                AlquimiaAuxiliaryData& aux_data,
                                AlquimiaAuxiliaryOutputData& aux_output)
{
  AllocateAlquimiaMaterialProperties(&sizes_, &mat_props);
  AllocateAlquimiaState(&sizes_, &chem_state);
  AllocateAlquimiaAuxiliaryData(&sizes_, &aux_data);
  AllocateAlquimiaAuxiliaryOutputData(&sizes_, &aux_output);

  // Make sure the auxiliary ints/doubles are zeroed out.
  std::fill(aux_data.aux_ints.data, aux_data.aux_ints.data + aux_data.aux_ints.size, 0);
  std::fill(aux_data.aux_doubles.data, aux_data.aux_doubles.data + aux_data.aux_doubles.size, 0.0);
}
                 
void ChemistryEngine::FreeState(AlquimiaMaterialProperties& mat_props,
                                AlquimiaState& chem_state,
                                AlquimiaAuxiliaryData& aux_data,
                                AlquimiaAuxiliaryOutputData& aux_output)
{
  FreeAlquimiaMaterialProperties(&mat_props);
  FreeAlquimiaState(&chem_state);
  FreeAlquimiaAuxiliaryData(&aux_data);
  FreeAlquimiaAuxiliaryOutputData(&aux_output);
}

} // namespace
} // namespace

