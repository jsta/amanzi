/*
  Transport PK

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Author: Jeffrey Johnson (jnjohnson@lbl.gov)
*/

#include "TransportBoundaryFunction_Alquimia.hh"

#ifdef ALQUIMIA_ENABLED

namespace Amanzi {
namespace Transport {

/* ******************************************************************
* Constructor of BCs for Alquimia.
****************************************************************** */
TransportBoundaryFunction_Alquimia::TransportBoundaryFunction_Alquimia(
    const Teuchos::ParameterList& plist,
    const Teuchos::RCP<const AmanziMesh::Mesh>& mesh,
    Teuchos::RCP<AmanziChemistry::Alquimia_PK> chem_pk,
    Teuchos::RCP<AmanziChemistry::ChemistryEngine> chem_engine)
    : mesh_(mesh),
      chem_pk_(chem_pk),
      chem_engine_(chem_engine)
{
  // Check arguments.
  if (chem_engine_ != Teuchos::null) {
    chem_engine_->InitState(alq_mat_props_, alq_state_, alq_aux_data_, alq_aux_output_);
    chem_engine_->GetPrimarySpeciesNames(tcc_names_);
  } else {
    Errors::Message msg;
    msg << "Geochemistry is off, but a geochemical condition was requested.";
    Exceptions::amanzi_throw(msg); 
  }

  // Get the regions assigned to this geochemical condition. We do not
  // check for region overlaps here, since a better way is to derive from 
  // the generic mesh function.
  
  std::vector<std::string> regions = plist.get<Teuchos::Array<std::string> >("regions").toVector();
  //auto regions_t = plist.get<Teuchos::Array<std::string> >("regions");
  //std::vector<std::string> regions = regions_t.toVector();
  

  std::vector<double> times = plist.get<Teuchos::Array<double> >("times").toVector();
  //auto times_t = plist.get<Teuchos::Array<double> >("times");
  //std::vector<double> times = times_t.toVector();
  
  std::vector<std::string> conditions = plist.get<Teuchos::Array<std::string> >("geochemical conditions").toVector();
  //auto cond_t = plist.get<Teuchos::Array<std::string> >("geochemical conditions");
  //std::vector<std::string> conditions = cond_t.toVector();
  
  std::cout<<"TransportBoundaryTT:  "<<regions.size()<<" "<<regions[0]<<" "<<times.size()<<" "<<times[0]<<"\n";
  // Function of geochemical conditions and the associates regions.
  f_ = Teuchos::rcp(new FunctionTabularString(times, conditions));
  Init_(regions);
  std::cout<<"TransportBoundaryTTT:  "<<"\n";
  ats_units_ = false;
  if (plist.isParameter("ats units [moles/m^3]")) {
    ats_units_ = true;
  }
}


/* ******************************************************************
* Delegating destructor.
****************************************************************** */
TransportBoundaryFunction_Alquimia::~TransportBoundaryFunction_Alquimia()
{
  std::string cond_name = (*f_)(0);
  std::cout<<"CondName2: "<<cond_name<<"\n";
  chem_engine_->FreeState(alq_mat_props_, alq_state_, alq_aux_data_, alq_aux_output_);
  std::cout<<"CondName3: "<<cond_name<<"\n";
}


/* ******************************************************************
* Internal subroutine that defines a boundary function.
****************************************************************** */
void TransportBoundaryFunction_Alquimia::Init_(const std::vector<std::string>& regions)
{
  for (int i = 0; i < regions.size(); ++i) {
    // Get the faces that belong to this region (since boundary conditions
    // are applied on faces).
    assert(mesh_->valid_set_name(regions[i], AmanziMesh::FACE));

    AmanziMesh::Entity_ID_List block;
    mesh_->get_set_entities(regions[i], AmanziMesh::FACE, AmanziMesh::Parallel_type::OWNED, &block);
    int nblock = block.size();

    // Now get the cells that are attached to these faces.
    AmanziMesh::Entity_ID_List cells;
    for (int n = 0; n < nblock; ++n) {
      int f = block[n];
      value_[f].resize(chem_engine_->NumPrimarySpecies());

      mesh_->face_get_cells(f, AmanziMesh::Parallel_type::OWNED, &cells);

      cell_for_face_[f] = cells[0];
    }
  }
}


/* ******************************************************************
* Evaluate values at time.
****************************************************************** */
void TransportBoundaryFunction_Alquimia::Compute(double t_old, double t_new) 
{
  std::string cond_name = (*f_)(t_new);
  std::cout<<"CondName: "<<cond_name<<"\n";
  // Loop over sides and evaluate values.
  for (auto it = begin(); it != end(); ++it) {
    // Find the index of the cell we're in.
    int f = it->first; 
    int cell = cell_for_face_[f];

    // Dump the contents of the chemistry state into our Alquimia containers.
    chem_pk_->CopyToAlquimia(cell, alq_mat_props_, alq_state_, alq_aux_data_);

    // Enforce the condition.
    chem_engine_->EnforceCondition(cond_name, t_new, alq_mat_props_, alq_state_, alq_aux_data_, alq_aux_output_);
    
    double factor = 1.0;
    if (ats_units_) {
      factor = 1.0/((*mol_dens_data_)[0][cell] / 1000.);
    }

    // Move the concentrations into place.
    std::vector<double>& values = it->second;
    for (int i = 0; i < values.size(); i++) {
      values[i] = alq_state_.total_mobile.data[i] * factor;
    }
  }
}

}  // namespace Transport
}  // namespace Amanzi

#endif

