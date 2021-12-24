/*
  Flow PK 

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Authors: Ethan Coon (ecoon@lanl.gov)

  Field evaluator for total volumetric water content which is the 
  conserved quantity in the Richards equation.

  Wrapping this conserved quantity as a field evaluator makes it
  easier to take derivatives, keep updated, and the like.
  The equation for this is simply:

    VWC = phi * (s_liquid * n_liquid + X_gas * s_gas * n_gas)

  where X_gas is the molar fraction of water in the gas phase.
*/


#ifndef AMANZI_FLOW_VWCONTENT_EVALUATOR_HH_
#define AMANZI_FLOW_VWCONTENT_EVALUATOR_HH_

#include "Teuchos_ParameterList.hpp"

#include "Factory.hh"
#include "EvaluatorSecondaryMonotype.hh"

namespace Amanzi {
namespace Flow {

class VWContentEvaluator : public EvaluatorSecondaryMonotype<CompositeVector, CompositeVectorSpace> {
 public:
  explicit VWContentEvaluator(Teuchos::ParameterList& plist);
  VWContentEvaluator(const VWContentEvaluator& other);

  // required inteface functions
  virtual Teuchos::RCP<Evaluator> Clone() const override;

  virtual void Evaluate_(const State& S, const std::vector<CompositeVector*>& results) override;

  virtual void EvaluatePartialDerivative_(const State& S, const Key& wrt_key, const Key& wrt_tag,
                                          const std::vector<CompositeVector*>& results) override;

 private:
  void Init_();

 protected:
  bool water_vapor_;
  Key saturation_key_, porosity_key_, mol_density_liquid_key_;
  
 private:
  static Utils::RegisteredFactory<Evaluator, VWContentEvaluator> reg_;
};

}  // namespace Flow
}  // namespace Amanzi

#endif
