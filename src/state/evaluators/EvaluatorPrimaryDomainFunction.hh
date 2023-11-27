/*
  Copyright 2010-202x held jointly by participating institutions.
  Amanzi is released under the three-clause BSD License.
  The terms of use and "as is" disclaimer for this license are
  provided in the top-level COPYRIGHT file.

  Authors: Ethan Coon
*/

/*
  State

*/

/*!

An evaluator with no dependencies that serves as the primary variable to be
solved for by a PK.  Note that users almost never are required to write an
input spec for these -- they are controlled by the PK and therefore the input
spec for this evaluator is written by that PK.

.. _evaluator-primary-spec:
.. admonition:: evaluator-primary-spec

   * `"tag`" ``[string]`` **""** Time tag at which this primary variable is used.

*/

#ifndef AMANZI_STATE_EVALUATOR_PRIMARY_
#define AMANZI_STATE_EVALUATOR_PRIMARY_

#include <memory>

#include "Evaluator.hh"
#include "EvaluatorPrimary.hh"
#include "Evaluator_Factory.hh"

namespace Amanzi {

//
// Dummy class, does everything but know the type, which is required to
// UpdateDerivative.  This is never used, instead the below templated one
// is.
//
class EvaluatorPrimaryDomainFunction_ : public EvaluatorPrimary_ {
 public:
  // ---------------------------------------------------------------------------
  // Constructors
  // ---------------------------------------------------------------------------
  explicit EvaluatorPrimaryDomainFunction_(Teuchos::ParameterList& plist);
  EvaluatorPrimaryDomainFunction_(const EvaluatorPrimaryDomainFunction_& other) = default;

  virtual EvaluatorPrimary_& operator=(const EvaluatorPrimary_& other) override;
  EvaluatorPrimaryDomainFunction_& operator=(const EvaluatorPrimaryDomainFunction_& other);

  // ---------------------------------------------------------------------------
  // Step 1 of graph checking -- Requires evaluators for the full dependency
  // graph.  Nothing to do here.
  // ---------------------------------------------------------------------------
  virtual void EnsureEvaluators(State& S) override {};

  // ---------------------------------------------------------------------------
  // Lazy evaluation of the evaluator.
  //
  // Updates the data, if needed.  Returns true if the value of the data has
  // changed since the last request for an update.
  // ---------------------------------------------------------------------------
  virtual bool Update(State& S, const Key& request) override final;

};

} // namespace Amanzi

#endif
