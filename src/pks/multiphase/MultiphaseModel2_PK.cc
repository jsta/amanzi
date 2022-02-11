/*
  MultiPhase PK

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Authors: Quan Bui (mquanbui@math.umd.edu)
           Konstantin Lipnikov (lipnikov@lanl.gov)

  Variables: pressure liquid, molar gas density, saturation liquid.
*/

// TPLs
#include "Teuchos_RCP.hpp"

// Multiphase
#include "MultiphaseModel2_PK.hh"
#include "NCP_F.hh"
#include "NCP_MolarDensities.hh"
#include "ProductEvaluator.hh"
#include "SaturationGasEvaluator.hh"
#include "TccLiquid.hh"
#include "TotalComponentStorage_MolarDensity.hh"
#include "TotalWaterStorage.hh"
#include "VaporPressureEvaluator.hh"

namespace Amanzi {
namespace Multiphase {

using CV_t = CompositeVector;
using CVS_t = CompositeVectorSpace;

/* ******************************************************************
* Standard constructor
****************************************************************** */
MultiphaseModel2_PK::MultiphaseModel2_PK(Teuchos::ParameterList& pk_tree,
                                         const Teuchos::RCP<Teuchos::ParameterList>& glist,
                                         const Teuchos::RCP<State>& S,
                                         const Teuchos::RCP<TreeVector>& soln)
  : Multiphase_PK(pk_tree, glist, S, soln) {};

  
/* ******************************************************************
* Setup
****************************************************************** */
void MultiphaseModel2_PK::Setup()
{
  Multiphase_PK::Setup();

  tcc_liquid_key_ = Keys::getKey(domain_, "total_component_concentration_liquid"); 
  tcc_gas_key_ = Keys::getKey(domain_, "total_component_concentration_gas"); 

  advection_water_key_ = Keys::getKey(domain_, "advection_water"); 
  pressure_vapor_key_ = Keys::getKey(domain_, "pressure_vapor"); 
  x_vapor_key_ = Keys::getKey(domain_, "mole_fraction_vapor"); 

  molar_density_liquid_key_ = Keys::getKey(domain_, "molar_density_liquid"); 
  molar_density_gas_key_ = Keys::getKey(domain_, "molar_density_gas"); 

  molecular_diff_liquid_key_ = Keys::getKey(domain_, "molecular_diff_liquid"); 
  molecular_diff_gas_key_ = Keys::getKey(domain_, "molecular_diff_gas"); 

  diffusion_liquid_key_ = Keys::getKey(domain_, "diffusion_liquid"); 
  diffusion_vapor_key_ = Keys::getKey(domain_, "diffusion_vapor"); 
  diffusion_gas_key_ = Keys::getKey(domain_, "diffusion_gas"); 

  // gas molar density is the primary solution
  if (!S_->HasRecord(tcc_gas_key_)) {
    component_names_ = mp_list_->sublist("molecular diffusion")
       .get<Teuchos::Array<std::string> >("aqueous names").toVector();

    S_->Require<CV_t, CVS_t>(tcc_gas_key_, Tags::DEFAULT, passwd_, component_names_)
      .SetMesh(mesh_)->SetGhosted(true)->SetComponent("cell", AmanziMesh::CELL, component_names_.size());

    Teuchos::ParameterList elist(tcc_gas_key_);
    elist.set<std::string>("name", tcc_gas_key_);
    auto tcc_gas_eval_ = Teuchos::rcp(new EvaluatorPrimary<CV_t, CVS_t>(elist));
    S_->SetEvaluator(tcc_gas_key_, Tags::DEFAULT, tcc_gas_eval_);

    S_->RequireDerivative<CV_t, CVS_t>(tcc_gas_key_, Tags::DEFAULT,
                                       tcc_gas_key_, Tags::DEFAULT, tcc_gas_key_);
  }

  // conserved quantities
  // -- total water storage
  if (!S_->HasRecord(tws_key_)) {
    S_->Require<CV_t, CVS_t>(tws_key_, Tags::DEFAULT, tws_key_)
      .SetMesh(mesh_)->SetGhosted(true)->SetComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::ParameterList elist(tws_key_);
    elist.set<std::string>("my key", tws_key_)
         .set<std::string>("tag", "")
         .set<std::string>("molar density liquid key", molar_density_liquid_key_)
         .set<std::string>("molar density gas key", molar_density_gas_key_)
         .set<std::string>("porosity key", porosity_key_)
         .set<std::string>("saturation liquid key", saturation_liquid_key_)
         .set<std::string>("mole fraction vapor key", x_vapor_key_);

    eval_tws_ = Teuchos::rcp(new TotalWaterStorage(elist));
    S_->SetEvaluator(tws_key_, Tags::DEFAULT, eval_tws_);

    S_->RequireDerivative<CV_t, CVS_t>(tws_key_, Tags::DEFAULT,
                                       pressure_liquid_key_, Tags::DEFAULT, tws_key_);
    S_->RequireDerivative<CV_t, CVS_t>(tws_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, tws_key_);
  }

  // -- total component storage (for one component)
  if (!S_->HasRecord(tcs_key_)) {
    S_->Require<CV_t, CVS_t>(tcs_key_, Tags::DEFAULT, tcs_key_)
      .SetMesh(mesh_)->SetGhosted(true)->SetComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::ParameterList elist(tcs_key_);
    elist.set<std::string>("my key", tcs_key_)
         .set<std::string>("tag", "")
         .set<std::string>("saturation liquid key", saturation_liquid_key_)
         .set<std::string>("porosity key", porosity_key_)
         .set<std::string>("molar density liquid key", tcc_liquid_key_)
         .set<std::string>("molar density gas key", tcc_gas_key_);

    eval_tcs_ = Teuchos::rcp(new TotalComponentStorage_MolarDensity(elist));
    S_->SetEvaluator(tcs_key_, Tags::DEFAULT, eval_tcs_);

    S_->RequireDerivative<CV_t, CVS_t>(tcs_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, tcs_key_);
    S_->RequireDerivative<CV_t, CVS_t>(tcs_key_, Tags::DEFAULT,
                                       tcc_gas_key_, Tags::DEFAULT, tcs_key_);
  }

  // liquid molar density
  if (!S_->HasRecord(molar_density_liquid_key_)) {
    S_->Require<CV_t, CVS_t>(molar_density_liquid_key_, Tags::DEFAULT, molar_density_liquid_key_)
      .SetMesh(mesh_)->SetGhosted(true)
      ->SetComponent("cell", AmanziMesh::CELL, component_names_.size());
    S_->RequireEvaluator(molar_density_liquid_key_, Tags::DEFAULT);
  }

  // saturation
  if (!S_->HasRecord(saturation_gas_key_)) {
    S_->Require<CV_t, CVS_t>(saturation_gas_key_, Tags::DEFAULT, saturation_gas_key_)
      .SetMesh(mesh_)->SetGhosted(true)->SetComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::ParameterList elist(saturation_gas_key_);
    elist.set<std::string>("my key", saturation_gas_key_)
         .set<std::string>("tag", "")
         .set<std::string>("saturation liquid key", saturation_liquid_key_);
    auto eval = Teuchos::rcp(new SaturationGasEvaluator(elist));
    S_->SetEvaluator(saturation_gas_key_, Tags::DEFAULT, eval);
  }

  // water vapor
  // -- vapor pressure
  if (!S_->HasRecord(pressure_vapor_key_)) {
    S_->Require<CV_t, CVS_t>(pressure_vapor_key_, Tags::DEFAULT, pressure_vapor_key_)
      .SetMesh(mesh_)->SetGhosted(true)->SetComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::ParameterList elist(pressure_vapor_key_);
    elist.set<std::string>("my key", pressure_vapor_key_)
         .set<std::string>("tag", "")
         .set<std::string>("temperature key", temperature_key_)
         .set<std::string>("molar density liquid key", molar_density_liquid_key_)
         .set<std::string>("saturation liquid key", saturation_liquid_key_)
         .set<std::string>("eos type", "water vapor over water/ice");
    auto eval = Teuchos::rcp(new VaporPressureEvaluator(elist, wrm_));
    S_->SetEvaluator(pressure_vapor_key_, Tags::DEFAULT, eval);
  }

  // -- coefficient for water vapor diffusion operator in liquid phase
  if (!S_->HasRecord(diffusion_vapor_key_)) {
    S_->Require<CV_t, CVS_t>(diffusion_vapor_key_, Tags::DEFAULT, diffusion_vapor_key_)
      .SetMesh(mesh_)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::Array<int> dep_powers(4, 1);
    Teuchos::Array<std::string> dep_names;

    dep_names.push_back(molar_density_gas_key_);
    dep_names.push_back(molecular_diff_gas_key_);
    dep_names.push_back(porosity_key_);
    dep_names.push_back(saturation_gas_key_);

    Teuchos::ParameterList elist(diffusion_vapor_key_);
    elist.set<std::string>("my key", diffusion_vapor_key_)
         .set<std::string>("tag", "")
         .set<Teuchos::Array<std::string> >("dependencies", dep_names) 
         .set<bool>("dependency tags are my tag", true)
         .set<Teuchos::Array<int> >("powers", dep_powers);

    auto eval = Teuchos::rcp(new ProductEvaluator(elist));
    S_->SetEvaluator(diffusion_vapor_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(diffusion_vapor_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, diffusion_vapor_key_);
  }

  // liquid molar density (for current component)
  if (!S_->HasRecord(tcc_liquid_key_)) {
    S_->Require<CV_t, CVS_t>(tcc_liquid_key_, Tags::DEFAULT, tcc_liquid_key_)
      .SetMesh(mesh_)->SetGhosted(true)->SetComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::ParameterList elist(tcc_liquid_key_);
    elist.set<std::string>("my key", tcc_liquid_key_)
         .set<std::string>("tag", "")
         .set<std::string>("tcc gas key", tcc_gas_key_);
    auto eval = Teuchos::rcp(new TccLiquid(elist));
    S_->SetEvaluator(tcc_liquid_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(tcc_liquid_key_, Tags::DEFAULT,
                                       tcc_gas_key_, Tags::DEFAULT, tcc_liquid_key_);
  }
 
  // product evaluators
  // -- coefficient for advection operator (div eta_l q_l) in liquid phase 
  if (!S_->HasRecord(advection_liquid_key_)) {
    S_->Require<CV_t, CVS_t>(advection_liquid_key_, Tags::DEFAULT, advection_liquid_key_)
      .SetMesh(mesh_)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::Array<int> dep_powers(3, 1);
    Teuchos::Array<std::string> dep_names;

    dep_names.push_back(tcc_liquid_key_);
    dep_names.push_back(relperm_liquid_key_);
    dep_names.push_back(viscosity_liquid_key_);
    dep_powers[2] = -1;

    Teuchos::ParameterList elist(advection_liquid_key_);
    elist.set<std::string>("my key", advection_liquid_key_)
         .set<std::string>("tag", "")
         .set<Teuchos::Array<std::string> >("dependencies", dep_names) 
         .set<bool>("dependency tags are my tag", true)
         .set<Teuchos::Array<int> >("powers", dep_powers);
    // elist.sublist("verbose object").set<std::string>("verbosity level", "extreme");

    auto eval = Teuchos::rcp(new ProductEvaluator(elist));
    S_->SetEvaluator(advection_liquid_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(advection_liquid_key_, Tags::DEFAULT,
                                       tcc_gas_key_, Tags::DEFAULT, advection_liquid_key_);
    S_->RequireDerivative<CV_t, CVS_t>(advection_liquid_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, advection_liquid_key_);
  }

  if (!S_->HasRecord(advection_water_key_)) {
    S_->Require<CV_t, CVS_t>(advection_water_key_, Tags::DEFAULT, advection_water_key_)
      .SetMesh(mesh_)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::Array<int> dep_powers(3, 1);
    Teuchos::Array<std::string> dep_names;

    dep_names.push_back(molar_density_liquid_key_);
    dep_names.push_back(relperm_liquid_key_);
    dep_names.push_back(viscosity_liquid_key_);
    dep_powers[2] = -1;

    Teuchos::ParameterList elist(advection_water_key_);
    elist.set<std::string>("my key", advection_water_key_)
         .set<std::string>("tag", "")
         .set<Teuchos::Array<std::string> >("dependencies", dep_names) 
         .set<bool>("dependency tags are my tag", true)
         .set<Teuchos::Array<int> >("powers", dep_powers);

    auto eval = Teuchos::rcp(new ProductEvaluator(elist));
    S_->SetEvaluator(advection_water_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(advection_water_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, advection_water_key_);
  }

  // -- coefficient for advection operator in gas phase (4 fields)
  if (!S_->HasRecord(advection_gas_key_)) {
    S_->Require<CV_t, CVS_t>(advection_gas_key_, Tags::DEFAULT, advection_gas_key_)
      .SetMesh(mesh_)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::Array<int> dep_powers(3, 1);
    Teuchos::Array<std::string> dep_names;

    dep_names.push_back(tcc_gas_key_);
    dep_names.push_back(relperm_gas_key_);
    dep_names.push_back(viscosity_gas_key_);
    dep_powers[2] = -1;

    Teuchos::ParameterList elist(advection_gas_key_);
    elist.set<std::string>("my key", advection_gas_key_)
         .set<std::string>("tag", "")
         .set<Teuchos::Array<std::string> >("dependencies", dep_names) 
         .set<bool>("dependency tags are my tag", true)
         .set<Teuchos::Array<int> >("powers", dep_powers);

    auto eval = Teuchos::rcp(new ProductEvaluator(elist));
    S_->SetEvaluator(advection_gas_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(advection_gas_key_, Tags::DEFAULT,
                                       tcc_gas_key_, Tags::DEFAULT, advection_gas_key_);
    S_->RequireDerivative<CV_t, CVS_t>(advection_gas_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, advection_gas_key_);
  }

  // -- coefficient for diffusion operator in liquid phase
  if (!S_->HasRecord(diffusion_liquid_key_)) {
    S_->Require<CV_t, CVS_t>(diffusion_liquid_key_, Tags::DEFAULT, diffusion_liquid_key_)
      .SetMesh(mesh_)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::Array<int> dep_powers(2, 1);
    Teuchos::Array<std::string> dep_names;

    dep_names.push_back(molecular_diff_liquid_key_);
    dep_names.push_back(saturation_liquid_key_);

    Teuchos::ParameterList elist(diffusion_liquid_key_);
    elist.set<std::string>("my key", diffusion_liquid_key_)
         .set<std::string>("tag", "")
         .set<Teuchos::Array<std::string> >("dependencies", dep_names) 
         .set<bool>("dependency tags are my tag", true)
         .set<Teuchos::Array<int> >("powers", dep_powers);

    auto eval = Teuchos::rcp(new ProductEvaluator(elist));
    S_->SetEvaluator(diffusion_liquid_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(diffusion_liquid_key_, Tags::DEFAULT,
                                       pressure_liquid_key_, Tags::DEFAULT, diffusion_liquid_key_);
    S_->RequireDerivative<CV_t, CVS_t>(diffusion_liquid_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, diffusion_liquid_key_);
  }

  // -- coefficient for diffusion operator in gas phase
  if (!S_->HasRecord(diffusion_gas_key_)) {
    S_->Require<CV_t, CVS_t>(diffusion_gas_key_, Tags::DEFAULT, diffusion_gas_key_)
      .SetMesh(mesh_)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::Array<int> dep_powers(2, 1);
    Teuchos::Array<std::string> dep_names;

    dep_names.push_back(molecular_diff_gas_key_);
    dep_names.push_back(saturation_gas_key_);

    Teuchos::ParameterList elist(diffusion_gas_key_);
    elist.set<std::string>("my key", diffusion_gas_key_)
         .set<std::string>("tag", "")
         .set<Teuchos::Array<std::string> >("dependencies", dep_names) 
         .set<bool>("dependency tags are my tag", true)
         .set<Teuchos::Array<int> >("powers", dep_powers);

    auto eval = Teuchos::rcp(new ProductEvaluator(elist));
    S_->SetEvaluator(diffusion_gas_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(diffusion_gas_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, diffusion_gas_key_);
  }

  // mole fraction of water vapor
  if (!S_->HasRecord(x_vapor_key_)) {
    S_->Require<CV_t, CVS_t>(x_vapor_key_, Tags::DEFAULT, x_vapor_key_)
      .SetMesh(mesh_)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::Array<int> dep_powers(2, 1);
    Teuchos::Array<std::string> dep_names;

    dep_names.push_back(pressure_gas_key_);
    dep_names.push_back(pressure_vapor_key_);
    dep_powers[0] = -1;

    Teuchos::ParameterList elist(x_vapor_key_);
    elist.set<std::string>("my key", x_vapor_key_)
         .set<std::string>("tag", "")
         .set<Teuchos::Array<std::string> >("dependencies", dep_names) 
         .set<bool>("dependency tags are my tag", true)
         .set<Teuchos::Array<int> >("powers", dep_powers);

    auto eval = Teuchos::rcp(new ProductEvaluator(elist));
    S_->SetEvaluator(x_vapor_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(x_vapor_key_, Tags::DEFAULT,
                                       pressure_liquid_key_, Tags::DEFAULT, x_vapor_key_);
  }

  // nonlinear complimentary problem
  if (!S_->HasRecord(ncp_f_key_)) {
    S_->Require<CV_t, CVS_t>(ncp_f_key_, Tags::DEFAULT, ncp_f_key_)
      .SetMesh(mesh_)->SetGhosted(true)->SetComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::ParameterList elist(ncp_f_key_);
    elist.set<std::string>("my key", ncp_f_key_)
         .set<std::string>("tag", "")
         .set<std::string>("saturation liquid key", saturation_liquid_key_);
    auto eval = Teuchos::rcp(new NCP_F(elist));
    S_->SetEvaluator(ncp_f_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(ncp_f_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, ncp_f_key_);
  }

  if (!S_->HasRecord(ncp_g_key_)) {
    S_->Require<CV_t, CVS_t>(ncp_g_key_, Tags::DEFAULT, ncp_g_key_)
      .SetMesh(mesh_)->SetGhosted(true)->SetComponent("cell", AmanziMesh::CELL, 1);

    Teuchos::ParameterList elist(ncp_g_key_);
    elist.set<std::string>("my key", ncp_g_key_)
         .set<std::string>("tag", "")
         .set<std::string>("mole fraction vapor key", x_vapor_key_)
         .set<std::string>("molar density gas key", molar_density_gas_key_)
         .set<std::string>("tcc gas key", tcc_gas_key_);
    auto eval = Teuchos::rcp(new NCP_MolarDensities(elist));
    S_->SetEvaluator(ncp_g_key_, Tags::DEFAULT, eval);

    S_->RequireDerivative<CV_t, CVS_t>(ncp_g_key_, Tags::DEFAULT,
                                      pressure_liquid_key_, Tags::DEFAULT, ncp_g_key_);
    S_->RequireDerivative<CV_t, CVS_t>(ncp_g_key_, Tags::DEFAULT,
                                      tcc_gas_key_, Tags::DEFAULT, ncp_g_key_);
    S_->RequireDerivative<CV_t, CVS_t>(ncp_g_key_, Tags::DEFAULT,
                                       saturation_liquid_key_, Tags::DEFAULT, ncp_g_key_);
  }
}


/* ******************************************************************
* Push data to the state
****************************************************************** */
void MultiphaseModel2_PK::CommitStep(
    double t_old, double t_new, const Tag& tag)
{
  Multiphase_PK::CommitStep(t_old, t_new, tag);

  // miscalleneous fields
  S_->GetEvaluator(ncp_fg_key_).Update(*S_, passwd_);
}


/********************************************************************
* Modifies nonlinear update du using .. TBW
****************************************************************** */
AmanziSolvers::FnBaseDefs::ModifyCorrectionResult
MultiphaseModel2_PK::ModifyCorrection(double h, Teuchos::RCP<const TreeVector> res,
                                      Teuchos::RCP<const TreeVector> u,
                                      Teuchos::RCP<TreeVector> du)
{
  // clip liquid pressure to stay positive
  const auto& u0c = *u->SubVector(0)->Data()->ViewComponent("cell");
  auto& du0c = *du->SubVector(0)->Data()->ViewComponent("cell");

  for (int c = 0; c < ncells_owned_; ++c) {
    du0c[0][c] = std::min(du0c[0][c], u0c[0][c]);
  }

  // clip gas total component concentration to stay positive
  /*
  const auto& u1c = *u->SubVector(1)->Data()->ViewComponent("cell");
  auto& du1c = *du->SubVector(1)->Data()->ViewComponent("cell");

  for (int i = 0; i < u1c.NumVectors(); ++i) {
    for (int c = 0; c < ncells_owned_; ++c) {
      du1c[i][c] = std::min(du1c[i][c], u1c[i][c]);
      du1c[i][c] = std::max(du1c[i][c], u1c[i][c] - 1.0);
    }    
  }
  */

  // clip saturation (residual saturation is missing, FIXME)
  /*
  const auto& u2c = *u->SubVector(2)->Data()->ViewComponent("cell");
  auto& du2c = *du->SubVector(2)->Data()->ViewComponent("cell");

  for (int c = 0; c < ncells_owned_; ++c) {
    du2c[0][c] = std::min(du2c[0][c], u2c[0][c]);
    du2c[0][c] = std::max(du2c[0][c], u2c[0][c] - 1.0);
  }
  */

  return AmanziSolvers::FnBaseDefs::CORRECTION_MODIFIED;
}


/* ******************************************************************* 
* Tweak evaluators.
******************************************************************* */
void MultiphaseModel2_PK::ModifyEvaluators(int neqn)
{
  if (neqn > 0) {
    int ifield(0), n(neqn - 1);
    // ifield is dummy here
    Teuchos::rcp_dynamic_cast<TotalComponentStorage_MolarDensity>(eval_tcs_)->set_subvector(ifield, n, kH_[n]);
    Teuchos::rcp_dynamic_cast<TotalComponentStorage_MolarDensity>(eval_tcs_)->Update(*S_, passwd_, true);

    auto eval = S_->GetEvaluatorPtr(tcc_liquid_key_, Tags::DEFAULT);
    Teuchos::rcp_dynamic_cast<TccLiquid>(eval)->set_subvector(ifield, n, kH_[n]);
    Teuchos::rcp_dynamic_cast<TccLiquid>(eval)->Update(*S_, passwd_, true);
  }
}

}  // namespace Multiphase
}  // namespace Amanzi
