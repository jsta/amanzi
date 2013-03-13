/* -*-  mode: c++; c-default-style: "google"; indent-tabs-mode: nil -*- */

/* -------------------------------------------------------------------------
ATS

License: see $ATS_DIR/COPYRIGHT
Author: Ethan Coon
------------------------------------------------------------------------- */

#include "Epetra_Vector.h"
#include "advection_diffusion.hh"

namespace Amanzi {
namespace Energy {

// AdvectionDiffusion is a BDFFnBase
// computes the non-linear functional g = g(t,u,udot)
void AdvectionDiffusion::fun(double t_old, double t_new, Teuchos::RCP<TreeVector> u_old,
                 Teuchos::RCP<TreeVector> u_new, Teuchos::RCP<TreeVector> g) {
  S_inter_->set_time(t_old);
  S_next_->set_time(t_new);

  // pointer-copy temperature into states and update any auxilary data
  solution_to_state(u_old, S_inter_);
  solution_to_state(u_new, S_next_);

  bc_temperature_->Compute(t_new);
  bc_flux_->Compute(t_new);
  UpdateBoundaryConditions_();

  Teuchos::RCP<CompositeVector> u = u_new->data();
  std::cout << "Residual calculation:" << std::endl;
  std::cout << "  u: " << (*u)("cell",0) << " " << (*u)("face",0) << std::endl;

  // get access to the solution
  Teuchos::RCP<CompositeVector> res = g->data();
  res->PutScalar(0.0);

  // diffusion term, implicit
  ApplyDiffusion_(S_next_, res);
  std::cout << "  res (after diffusion): " << (*res)("cell",0) << " " << (*res)("face",0) << std::endl;

  // accumulation term
  AddAccumulation_(res);
  std::cout << "  res (after accumulation): " << (*res)("cell",0) << " " << (*res)("face",0) << std::endl;

  // advection term, explicit
  AddAdvection_(S_inter_, res, true);
  std::cout << "  res (after advection): " << (*res)("cell",0) << " " << (*res)("face",0) << std::endl;
};

// applies preconditioner to u and returns the result in Pu
void AdvectionDiffusion::precon(Teuchos::RCP<const TreeVector> u, Teuchos::RCP<TreeVector> Pu) {
  std::cout << "Precon application:" << std::endl;
  std::cout << "  u: " << (*u->data())("cell",0) << " " << (*u->data())("face",0) << std::endl;
  // preconditioner for accumulation only:
  //  *Pu = *u;

  // MFD ML preconditioner
  preconditioner_->ApplyInverse(*u, Pu.ptr());

  std::cout << "  Pu: " << (*Pu->data())("cell",0) << " " << (*Pu->data())("face",0) << std::endl;
};


// updates the preconditioner
void AdvectionDiffusion::update_precon(double t, Teuchos::RCP<const TreeVector> up, double h) {
  S_next_->set_time(t);
  PKDefaultBase::solution_to_state(up, S_next_);

  // div K_e grad u
  Teuchos::RCP<const CompositeVector> thermal_conductivity =
      S_next_->GetFieldData("thermal_conductivity");

  // update boundary conditions
  bc_temperature_->Compute(S_next_->time());
  bc_flux_->Compute(S_next_->time());
  UpdateBoundaryConditions_();

  mfd_preconditioner_->CreateMFDstiffnessMatrices(thermal_conductivity.ptr());
  mfd_preconditioner_->CreateMFDrhsVectors();

  // update with accumulation terms
  Teuchos::RCP<const CompositeVector> temp1 =
    S_next_->GetFieldData("temperature");
  Teuchos::RCP<const CompositeVector> temp0 =
    S_inter_->GetFieldData("temperature");
  Teuchos::RCP<const CompositeVector> cell_volume =
    S_next_->GetFieldData("cell_volume");

  std::vector<double>& Acc_cells = mfd_preconditioner_->Acc_cells();
  std::vector<double>& Fc_cells = mfd_preconditioner_->Fc_cells();
  int ncells = cell_volume->size("cell",false);
  for (int c=0; c!=ncells; ++c) {
    double factor = (*cell_volume)("cell",c);
    Acc_cells[c] += factor/h;
    Fc_cells[c] += factor/h * (*temp0)("cell",c);
  }

  mfd_preconditioner_->ApplyBoundaryConditions(bc_markers_, bc_values_);
  mfd_preconditioner_->AssembleGlobalMatrices();

  mfd_preconditioner_->ComputeSchurComplement(bc_markers_, bc_values_);
  mfd_preconditioner_->UpdatePreconditioner();
};


void AdvectionDiffusion::set_preconditioner(const Teuchos::RCP<Operators::Matrix> precon) {
  preconditioner_ = precon;
  mfd_preconditioner_ = Teuchos::rcp_dynamic_cast<Operators::MatrixMFD>(precon);
  ASSERT(mfd_preconditioner_ != Teuchos::null);
  mfd_preconditioner_->SetSymmetryProperty(true);
  mfd_preconditioner_->SymbolicAssembleGlobalMatrices();
  mfd_preconditioner_->CreateMFDmassMatrices(Teuchos::null);
  mfd_preconditioner_->InitPreconditioner();
}

} // namespace Energy
} // namespace Amanzi
