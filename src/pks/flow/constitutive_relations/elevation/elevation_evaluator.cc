/* -*-  mode: c++; c-default-style: "google"; indent-tabs-mode: nil -*- */

/*
  The elevation evaluator gets the surface elevation, slope, and updates pres + elev.

  Authors: Ethan Coon (ecoon@lanl.gov)
*/

#include "elevation_evaluator.hh"

namespace Amanzi {
namespace Flow {
namespace FlowRelations {

ElevationEvaluator::ElevationEvaluator(Teuchos::ParameterList& plist) :
    SecondaryVariablesFieldEvaluator(plist),
    updated_once_(false) {
  my_keys_.push_back(plist_.get<std::string>("elevation key", "elevation"));
  my_keys_.push_back(plist_.get<std::string>("slope magnitude key", "slope_magnitude"));
  setLinePrefix(my_keys_[0]+std::string(" evaluator"));
}

void ElevationEvaluator::EvaluateField_(const Teuchos::Ptr<State>& S,
        const std::vector<Teuchos::Ptr<CompositeVector> >& results) {
  EvaluateElevationAndSlope_(S, results);
}

// This is hopefully never called?
void ElevationEvaluator::EvaluateFieldPartialDerivative_(const Teuchos::Ptr<State>& S,
        Key wrt_key, const std::vector<Teuchos::Ptr<CompositeVector> >& results) {
  ASSERT(0);
}

// Custom EnsureCompatibility forces this to be updated once.
bool ElevationEvaluator::HasFieldChanged(const Teuchos::Ptr<State>& S,
                                         Key request) {
  bool changed = SecondaryVariablesFieldEvaluator::HasFieldChanged(S,request);
  if (!updated_once_) {
    UpdateField_(S);
    updated_once_ = true;
    return true;
  }
  return changed;
}

} //namespace
} //namespace
} //namespace
