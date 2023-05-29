/*
  Copyright 2010-202x held jointly by participating institutions.
  Amanzi is released under the three-clause BSD License.
  The terms of use and "as is" disclaimer for this license are
  provided in the top-level COPYRIGHT file.

  Authors: Konstantin Lipnikov (lipnikov@lanl.gov)
*/

/*
  EOS

  EOS for tabulated viscosity.
*/

#include "LookupTable_Amanzi.hh"
#include "ViscosityTabular.hh"

namespace Amanzi {
namespace AmanziEOS {

ViscosityTabular::ViscosityTabular(Teuchos::ParameterList& eos_plist)
  : EOS_Viscosity(eos_plist)
{
  table_ = Teuchos::rcp(new LookupTable_Amanzi(eos_plist_));
}

} // namespace AmanziEOS
} // namespace Amanzi
