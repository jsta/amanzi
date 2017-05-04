/*
  The discretiation component of the Amanzi code.
  License: BSD
  Author: Konstantin Lipnikov (lipnikov@lanl.gov)
*/

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#include "UnitTest++.h"

#include "Teuchos_ParameterList.hpp"
#include "Teuchos_RCP.hpp"
#include "Teuchos_XMLParameterListHelpers.hpp"
#include "Teuchos_LAPACK.hpp"

#include "MeshFactory.hh"
#include "MeshAudit.hh"

#include "Mesh.hh"
#include "Point.hh"

#include "mfd3d_elasticity.hh"
#include "Tensor.hh"


/* **************************************************************** */
TEST(ELASTICITY_STIFFNESS_2D) {
  using namespace Teuchos;
  using namespace Amanzi;
  using namespace Amanzi::AmanziGeometry;
  using namespace Amanzi::AmanziMesh;
  using namespace Amanzi::WhetStone;

  std::cout << "\nTest: Stiffness matrix for elasticity in 2D" << std::endl;
#ifdef HAVE_MPI
  Epetra_MpiComm *comm = new Epetra_MpiComm(MPI_COMM_WORLD);
#else
  Epetra_SerialComm *comm = new Epetra_SerialComm();
#endif

  MeshFactory meshfactory(comm);
  meshfactory.preference(FrameworkPreference({MSTK}));
  // RCP<Mesh> mesh = meshfactory(0.0, 0.0, 1.0, 1.0, 1, 1); 
  RCP<Mesh> mesh = meshfactory("test/one_cell2.exo"); 
 
  MFD3D_Elasticity mfd(mesh);

  int nnodes = 5, nrows = nnodes * 2, cell = 0;
  DenseMatrix A(nrows, nrows);

  for (int method = 0; method < 2; method++) {
    Tensor T;
    double lambda(1.0), mu(0.0);
    if (method == 0) {
      T.Init(2, 1);
      T(0, 0) = 1.0;
    } else if (method == 1) {
      mu = 3.0;
      lambda = 0.0;
      T.Init(2, 4);
      T(0, 0) = T(1, 1) = lambda + 2 * mu;
      T(0, 1) = T(1, 0) = lambda;
      T(2, 2) = mu;
    }

    // mfd.StiffnessMatrix(cell, T, A);
    mfd.StiffnessMatrixOptimized(cell, T, A);
    // mfd.StiffnessMatrixMMatrix(cell, T, A);
    // std::cout << "Number of simplex itrs=" << mfd.simplex_num_itrs() << std::endl;
    // std::cout << "Functional value=" << mfd.simplex_functional() << std::endl;

    printf("Stiffness matrix for cell %3d\n", cell);
    for (int i = 0; i < nrows; i++) {
      for (int j = 0; j < nrows; j++ ) printf("%8.4f ", A(i, j)); 
      printf("\n");
    }

    // verify SPD propery
    for (int i = 0; i < nrows; i++) CHECK(A(i, i) > 0.0);

    // verify exact integration property
    AmanziMesh::Entity_ID_List nodes;
    mesh->cell_get_nodes(cell, &nodes);
    
    int d = mesh->space_dimension();
    Point p(d);

    double xx[nrows], yy[nrows];
    for (int i = 0; i < nnodes; i++) {
      int v = nodes[i];
      mesh->node_get_coordinates(v, &p);
      xx[i] = p[0];    
      xx[i + nnodes] = 0.0;    

      yy[i] = 0.0;    
      yy[i + nnodes] = p[1];
    }

    double xi, yi, xj;
    double vxx = 0.0, vxy = 0.0, volume = mesh->cell_volume(cell); 
    for (int i = 0; i < nnodes; i++) {
      for (int j = 0; j < nnodes; j++) {
        vxx += A(i, j) * xx[i] * xx[j];
        vxy += A(i, j) * yy[i] * xx[j];
      }
    }
    CHECK_CLOSE((2 * mu + lambda) * volume, vxx, 1e-10);
    CHECK_CLOSE(0.0, vxy, 1e-10);
  }

  delete comm;
}


/* **************************************************************** */
TEST(ELASTICITY_STIFFNESS_3D) {
  using namespace Teuchos;
  using namespace Amanzi;
  using namespace Amanzi::AmanziGeometry;
  using namespace Amanzi::AmanziMesh;
  using namespace Amanzi::WhetStone;

  std::cout << "\nTest: Stiffness matrix for Elasticity in 3D" << std::endl;
#ifdef HAVE_MPI
  Epetra_MpiComm *comm = new Epetra_MpiComm(MPI_COMM_WORLD);
#else
  Epetra_SerialComm *comm = new Epetra_SerialComm();
#endif

  MeshFactory meshfactory(comm);
  meshfactory.preference(FrameworkPreference({MSTK}));
  // RCP<Mesh> mesh = meshfactory(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1, 1, 1); 
  RCP<Mesh> mesh = meshfactory("test/one_cell.exo"); 
 
  MFD3D_Elasticity mfd(mesh);

  int nrows = 24, nnodes = 8, cell = 0;
  Tensor T(3, 1);
  T(0, 0) = 1;

  DenseMatrix A(nrows, nrows);
  mfd.StiffnessMatrix(cell, T, A);

  printf("Stiffness matrix for cell %3d\n", cell);
  for (int i=0; i<nrows; i++) {
    for (int j=0; j<nrows; j++ ) printf("%8.4f ", A(i, j)); 
    printf("\n");
  }

  // verify SPD propery
  for (int i=0; i<nrows; i++) CHECK(A(i, i) > 0.0);

  // verify exact integration property
  AmanziMesh::Entity_ID_List nodes;
  mesh->cell_get_nodes(cell, &nodes);
    
  int d = mesh->space_dimension();
  Point p(d);

  double xi, yi, xj;
  double vxx = 0.0, vxy = 0.0, volume = mesh->cell_volume(cell); 
  for (int i = 0; i < nnodes; i++) {
    int v = nodes[i];
    mesh->node_get_coordinates(v, &p);
    xi = p[0];
    yi = p[1];
    for (int j = 0; j < nnodes; j++) {
      v = nodes[j];
      mesh->node_get_coordinates(v, &p);
      xj = p[0];
      vxx += A(i, j) * xi * xj;
      vxy += A(i, j) * yi * xj;
    }
  }
  CHECK_CLOSE(vxx, volume, 1e-10);
  CHECK_CLOSE(vxy, 0.0, 1e-10);

  delete comm;
}


