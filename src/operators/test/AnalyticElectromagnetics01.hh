/*
  Operators

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Author: Konstantin Lipnikov (lipnikov@lanl.gov)

  The problem is c E + curl K curl E = Q, where
  E = linear
  K = full tensor
*/

#ifndef AMANZI_OPERATOR_ANALYTIC_ELECTROMAGNETICS_01_HH_
#define AMANZI_OPERATOR_ANALYTIC_ELECTROMAGNETICS_01_HH_

#include "AnalyticElectromagneticsBase.hh"

class AnalyticElectromagnetics01 : public AnalyticElectromagneticsBase {
 public:
  AnalyticElectromagnetics01(double c,
                             Teuchos::RCP<const Amanzi::AmanziMesh::Mesh> mesh) :
      c_(c),
      AnalyticElectromagneticsBase(mesh) {};

  Amanzi::WhetStone::Tensor Tensor(const Amanzi::AmanziGeometry::Point& p, double t) {
    Amanzi::WhetStone::Tensor K(3, 2);
    K(0, 0) = 1.0;
    K(1, 1) = 2.0;
    K(2, 2) = 3.0;
    K(0, 1) = K(1, 0) = -0.5;
    K(0, 2) = K(2, 0) = -0.1;
    K(1, 2) = K(2, 1) = -0.2;
    return K;
  }

  Amanzi::AmanziGeometry::Point electric_exact(const Amanzi::AmanziGeometry::Point& p, double t) const { 
    double x = p[0];
    double y = p[1];
    double z = p[2];
    return Amanzi::AmanziGeometry::Point(z - y, x - z, y - x);
  }

  Amanzi::AmanziGeometry::Point magnetic_exact(const Amanzi::AmanziGeometry::Point& p, double t) { 
    return Amanzi::AmanziGeometry::Point(0.0, 0.0, 0.0);
  }

  Amanzi::AmanziGeometry::Point source_exact(const Amanzi::AmanziGeometry::Point& p, double t) { 
    return c_ * electric_exact(p, t);
  }

 private:
  double c_;

};

#endif
