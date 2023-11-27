/*
  Copyright 2010-202x held jointly by participating institutions.
  Amanzi is released under the three-clause BSD License.
  The terms of use and "as is" disclaimer for this license are
  provided in the top-level COPYRIGHT file.

  Authors: Ethan Coon
*/

/* -------------------------------------------------------------------------
  State

  An evaluator with no dependencies solved for by a PK.
*/

#include <memory>

#include "EvaluatorPrimaryDomainFunction.hh"
#include "StateDefs.hh"

namespace Amanzi {

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------
EvaluatorPrimaryDomainFunction_::EvaluatorPrimaryDomainFunction_(Teuchos::ParameterList& plist)
  : my_key_(Keys::cleanPListName(plist.name())),
    my_tag_(Keys::readTag(plist, "tag")),
    vo_(Keys::cleanPListName(plist.name()), plist)
{
  type_ = EvaluatorType::PRIMARY;
}


// ---------------------------------------------------------------------------
// Assignment operator.
// ---------------------------------------------------------------------------
EvaluatorPrimaryDomainFunction_&
EvaluatorPrimaryDomainFunction_::operator=(const EvaluatorPrimaryDomainFunction_& other)
{
  if (this != &other) {
    AMANZI_ASSERT(my_key_ == other.my_key_);
    requests_ = other.requests_;
    deriv_requests_ = other.deriv_requests_;
  }
  return *this;
}


// ---------------------------------------------------------------------------
// Virtual assignment operator.
// ---------------------------------------------------------------------------
Evaluator&
EvaluatorPrimaryDomainFunction_::operator=(const Evaluator& other)
{
  if (this != &other) {
    const EvaluatorPrimaryDomainFunction_* other_p = dynamic_cast<const EvaluatorPrimaryDomainFunction_*>(&other);
    AMANZI_ASSERT(other_p != NULL);
    *this = *other_p;
  }
  return *this;
}


// ---------------------------------------------------------------------------
// Lazy evaluation of the evaluator.
//
// Updates the data, if needed.  Returns true if the value of the data has
// changed since the last request for an update.
// ---------------------------------------------------------------------------
bool
EvaluatorPrimaryDomainFunction_::Update(State& S, const Key& request)
{
  Teuchos::OSTab tab = vo_.getOSTab();
  if (vo_.os_OK(Teuchos::VERB_EXTREME)) {
    *vo_.os() << "Primary Variable " << my_key_ << " requested by " << request << std::endl;
  }

  if (requests_.find(request) == requests_.end()) {
    if (vo_.os_OK(Teuchos::VERB_EXTREME)) { *vo_.os() << "  Has changed... " << std::endl; }
    requests_.insert(request);
    return true;

  } else {
    if (vo_.os_OK(Teuchos::VERB_EXTREME)) { *vo_.os() << "  Has not changed... " << std::endl; }
    return false;
  }
}


// ---------------------------------------------------------------------------
// How a PK informs this leaf of the tree that it has changed.
//
// Effectively this simply tosses the request history, so that the next
// requests will say this has changed.
// ---------------------------------------------------------------------------
void
EvaluatorPrimaryDomainFunction_::SetChanged()
{
  Teuchos::OSTab tab = vo_.getOSTab();
  if (vo_.os_OK(Teuchos::VERB_EXTREME)) {
    *vo_.os() << "Primary field \"" << vo_.color("gree") << my_key_ << vo_.reset()
              << "\" marked as changed." << std::endl;
  }

  // clear cache
  requests_.clear();

  // note the derivative is always 1, no need to re-calculate.
}


// Nothing is a dependency of a primary variable.
bool
EvaluatorPrimaryDomainFunction_::IsDependency(const State& S, const Key& key, const Tag& tag) const
{
  return false;
}


bool
EvaluatorPrimaryDomainFunction_::ProvidesKey(const Key& key, const Tag& tag) const
{
  return (key == my_key_) && (tag == my_tag_);
}


std::string
EvaluatorPrimaryDomainFunction_::WriteToString() const
{
  std::stringstream result;
  result << my_key_ << std::endl << "  Type: primary" << std::endl;
  return result.str();
}

} // namespace Amanzi
