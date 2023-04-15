/*
  Copyright 2010-202x held jointly by participating institutions.
  Amanzi is released under the three-clause BSD License.
  The terms of use and "as is" disclaimer for this license are
  provided in the top-level COPYRIGHT file.

  Authors: Konstantin Lipnikov (lipnikov@lanl.gov)
*/

/*
  Flow PK

  A power law for permeability factor depending on porosity.
*/

#ifndef AMANZI_FLOW_PERMEABILITY_POWER_LAW_HH_
#define AMANZI_FLOW_PERMEABILITY_POWER_LAW_HH_

#include "Teuchos_ParameterList.hpp"

#include "Permeability.hh"

namespace Amanzi {
namespace Flow {

class Permeability_PowerLaw : public Permeability {
 public:
  explicit Permeability_PowerLaw(Teuchos::ParameterList& plist)
  {
    phi_ref_ = plist.get<double>("undeformed soil porosity");
    exp_ = plist.get<double>("power law exponent");
  }
  ~Permeability_PowerLaw(){};

  // required methods from the base class
  double Factor(double phi)
  {
    return std::pow(phi / phi_ref_, exp_);
  }

  double dFactordPorosity(double phi)
  {
    return std::pow(phi / phi_ref_, exp_ - 1) / phi_ref_;
  }

 private:
  double phi_ref_, exp_;
};

} // namespace Flow
} // namespace Amanzi

#endif
