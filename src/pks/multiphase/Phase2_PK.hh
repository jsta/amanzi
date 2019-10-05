/*
  MultiPhase

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Authors: Quan Bui (mquanbui@math.umd.edu)
*/

#ifndef AMANZI_PHASE2_PK_HH_
#define AMANZI_PHASE2_PK_HH_

// TPLs
#include "Teuchos_RCP.hpp"

// Amanzi
#include "FlowBoundaryFunction.hh"
#include "PDE_Accumulation.hh"
#include "PDE_AdvectionUpwind.hh"
#include "PDE_DiffusionFactory.hh"
#include "PDE_DiffusionFV.hh"
#include "PK_Factory.hh"
#include "PK_DomainFunction.hh"
#include "PK_PhysicalBDF.hh"
#include "primary_variable_field_evaluator.hh"
#include "State.hh"
#include "Tensor.hh"
#include "TreeVector.hh"
#include "Upwind.hh"
#include "UpwindFactory.hh"

// Amanzi::Multiphase
#include "CapillaryPressure.hh"
#include "MultiphaseTypeDefs.hh"
#include "RelativePermeability.hh"
#include "WaterRetentionModel.hh"

namespace Amanzi {
namespace Multiphase {
//class State;

class Phase2_PK: public PK_PhysicalBDF {
public:
  Phase2_PK(Teuchos::ParameterList& pk_tree,
            const Teuchos::RCP<Teuchos::ParameterList>& global_list,
            const Teuchos::RCP<State>& S,
            const Teuchos::RCP<TreeVector>& soln);

  ~Phase2_PK();

  // New interface for a PK
  virtual void Setup(){};
  virtual void Initialize() {
    InitializeFields();
    InitializePhase2();
    InitNextTI();
  }

  virtual double get_dt() { return 1.0; }
  virtual void set_dt(double) {};
  virtual bool AdvanceStep(double t_old, double t_new, bool reinit);
  virtual void CommitStep(double t_old, double t_new);
  virtual void CalculateDiagnostics(){};
  virtual std::string name() { return "phase2 pk"; }

  // Main methods of this PK
  void InitializeFields();
  void InitializePhase2();
  void InitNextTI();

  // Time integration interface new_mpc, implemented in Pressure_PK_TI.cc
  // computes the non-linear functional f = f(t,u,udot)
  virtual void Functional(double t_old, double t_new, 
                          Teuchos::RCP<TreeVector> u_old,
                          Teuchos::RCP<TreeVector> u_new,
                          Teuchos::RCP<TreeVector> f);

  // applies preconditioner to u and returns the result in Pu
  virtual int ApplyPreconditioner(Teuchos::RCP<const TreeVector> u, 
                                  Teuchos::RCP<TreeVector> Pu);

  // updates the preconditioner
  virtual void UpdatePreconditioner(double t, Teuchos::RCP<const TreeVector> up, double h);

  // computes a norm on u-du and returns the result
  virtual double ErrorNorm(Teuchos::RCP<const TreeVector> u,
                           Teuchos::RCP<const TreeVector> du) { return 0.0; }

  // check the admissibility of a solution
  // override with the actual admissibility check
  virtual bool IsAdmissible(Teuchos::RCP<const TreeVector> up) { return true; }

  // possibly modifies the predictor that is going to be used as a
  // starting value for the nonlinear solve in the time integrator,
  // the time integrator will pass the predictor that is computed
  // using extrapolation and the time step that is used to compute
  // this predictor this function returns true if the predictor was
  // modified, false if not
  virtual bool ModifyPredictor(double h, Teuchos::RCP<const TreeVector> u0,
                               Teuchos::RCP<TreeVector> u) { return false; }

  // possibly modifies the correction, after the nonlinear solver (NKA)
  // has computed it, will return true if it did change the correction,
  // so that the nonlinear iteration can store the modified correction
  // and pass it to NKA so that the NKA space can be updated
  virtual AmanziSolvers::FnBaseDefs::ModifyCorrectionResult
  ModifyCorrection(double h, Teuchos::RCP<const TreeVector> res,
                   Teuchos::RCP<const TreeVector> u,
                   Teuchos::RCP<TreeVector> du) {
    return AmanziSolvers::FnBaseDefs::CORRECTION_NOT_MODIFIED;
  }

  // experimental approach -- calling this indicates that the time
  // integration scheme is changing the value of the solution in
  // state.
  virtual void ChangedSolution() {};

  void NumericalJacobian(double t_old, double t_new, Teuchos::RCP<const TreeVector> u, double eps);

  // methods to compute boundary and source terms
  //void SetAbsolutePermeabilityTensor(Teuchos::RCP<CompositeVector> Sw);
  void SetAbsolutePermeabilityTensor();
  void AddSourceTerms(CompositeVector& rhs);
  void ComputeBCs();

  // io members, implemented in Multiphase_IO.cc
  void ProcessParameterList(Teuchos::ParameterList& list);
  void ProcessStringSourceDistribution(const std::string name, int* method);

  // misc function
  void ComputeBC_Pc();
  void IncludeCapillary(bool include_capillary) { include_capillary_ = include_capillary; }

  // access member methods
  Teuchos::RCP<Operators::PDE_Diffusion> op_prec1() { return op1_preconditioner_; }
  Teuchos::RCP<Operators::PDE_AdvectionUpwind> op_prec2() { return op2_preconditioner_; }
  std::vector<Teuchos::RCP<Operators::PDE_Diffusion> >& Ops() { return ops_; }

public:
  int ncells_owned_, ncells_wghost_;
  int nfaces_owned_, nfaces_wghost_;
  int missed_bc_faces_, dirichlet_bc_faces_;

  Teuchos::ParameterList linear_operator_list_;
  Teuchos::ParameterList preconditioner_list_;
  Teuchos::RCP<Teuchos::ParameterList> glist_;
  Teuchos::ParameterList mp_list_;

  double dT_, T_physics_, dTnext_;

private:
  // mesh structure and geometry
  Teuchos::RCP<const AmanziMesh::Mesh> mesh_;
  int dim_;
  bool include_capillary_;

  // Stationary physical quantatities
  std::vector<WhetStone::Tensor> K_;
  Teuchos::RCP<Epetra_Vector> Kxy_;
  AmanziGeometry::Point gravity_;
  double g_, atm_pressure_;
  double rho_, mu_, phi_;

  // source and sink terms
  PK_DomainFunction* src_sink_;
  int src_sink_distribution_;
  std::string passwd_;

  // Verbose control
  VerboseObject* vo_;

  // boundary conditons
  Flow::FlowBoundaryFunction* bc_pressure_;
  Flow::FlowBoundaryFunction* bc_saturation_;
  Flow::FlowBoundaryFunction* bc_flux_phase2_;

  Teuchos::RCP<State> S_;
  Teuchos::RCP<RelativePermeability> rel_perm_n_;
  Teuchos::RCP<CapillaryPressure> capillary_pressure_;
  Teuchos::RCP<Operators::PDE_Accumulation> op_acc_;
  Teuchos::RCP<Operators::PDE_Diffusion> op_matrix_;
  Teuchos::RCP<Operators::PDE_Diffusion> op_matrix_copy_;
  Teuchos::RCP<Operators::PDE_Diffusion> op_matrix_copy1_;
  Teuchos::RCP<Operators::PDE_Diffusion> op1_preconditioner_;
  Teuchos::RCP<Operators::PDE_AdvectionUpwind> op2_preconditioner_;
  Teuchos::RCP<Operators::PDE_DiffusionFV> op_sum_;
  Teuchos::RCP<Operators::PDE_AdvectionUpwind> op_sum1_;
  Teuchos::RCP<Operators::BCs> op_bc_;
  Teuchos::RCP<Operators::BCs> op_bc_s_;
  Teuchos::RCP<Operators::BCs> op_bc_pc_;
  Teuchos::RCP<Operators::BCs> op_bc_pc_prime_;

  std::vector<Teuchos::RCP<Operators::PDE_Diffusion> > ops_;
  typedef std::vector<Teuchos::RCP<Operators::PDE_Diffusion> >::iterator op_iter;
  typedef std::vector<Teuchos::RCP<Operators::Op> >::iterator local_op_iter;

  // upwind operator
  Teuchos::RCP<Operators::Upwind<RelativePermeability> > upwind_;
  Teuchos::RCP<Operators::Upwind<CapillaryPressure> > upwind_pc_;

  // The solution obtained from solving for pressure
  Teuchos::RCP<CompositeVector> pressure_phase2_;

  // solution tree vector
  Teuchos::RCP<TreeVector> soln_;

  // upwind flux
  Teuchos::RCP<CompositeVector> upwind_velocity_;
  Teuchos::RCP<CompositeVector> tmp_flux_; 

  static RegisteredPKFactory<Phase2_PK> reg_;

};

}  // namespase Flow
}  // namespace Amanzi

#endif

