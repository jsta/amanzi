/*

  Operators

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Author: Konstantin Lipnikov (lipnikov@lanl.gov)
*/

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

// TPLs
#include "Epetra_MultiVector.h"
#include "Teuchos_RCP.hpp"
#include "Teuchos_ParameterList.hpp"
#include "Teuchos_ParameterXMLFileReader.hpp"
#include "UnitTest++.h"

// Amanzi
#include "DG_Modal.hh"
#include "Explicit_TI_RK.hh"
#include "GMVMesh.hh"
#include "LinearOperatorPCG.hh"
#include "Mesh.hh"
#include "MeshFactory.hh"
#include "MeshMaps_FEM.hh"
#include "MeshMaps_VEM.hh"
#include "NumericalIntegration.hh"
#include "Tensor.hh"
#include "WhetStone_typedefs.hh"

// Amanzi::Operators
#include "OperatorDefs.hh"
#include "PDE_Abstract.hh"
#include "PDE_AdvectionRiemann.hh"
#include "PDE_Reaction.hh"

#include "AnalyticDG00.hh"
#include "AnalyticDG04.hh"
#include "AnalyticDG05.hh"

namespace Amanzi {

class RemapDG : public Explicit_TI::fnBase<CompositeVector> {
 public:
  RemapDG(const Teuchos::RCP<const AmanziMesh::Mesh> mesh,
          const std::shared_ptr<WhetStone::MeshMaps> maps) : mesh_(mesh), maps_(maps) {
    // mesh data
    ncells_owned_ = mesh_->num_entities(AmanziMesh::CELL, AmanziMesh::OWNED);
    nfaces_wghost_ = mesh_->num_entities(AmanziMesh::FACE, AmanziMesh::USED);
    dim_ = mesh_->space_dimension();

    // face velocities
    velf_vec_.resize(nfaces_wghost_);
    for (int f = 0; f < nfaces_wghost_; ++f) {
      maps_->VelocityFace(f, velf_vec_[f]);
    }

    // cell-baced velocities and Jacobian matrices
    WhetStone::Entity_ID_List faces;
    uc_.resize(ncells_owned_);
    J_.resize(ncells_owned_);

    for (int c = 0; c < ncells_owned_; ++c) {
      mesh_->cell_get_faces(c, &faces);

      std::vector<WhetStone::VectorPolynomial> vvf;
      for (int n = 0; n < faces.size(); ++n) {
        vvf.push_back(velf_vec_[faces[n]]);
      }

      maps->VelocityCell(c, vvf, uc_[c]);
      maps->Jacobian(uc_[c], J_[c]);
    }

    velf_ = Teuchos::rcp(new std::vector<WhetStone::Polynomial>(nfaces_wghost_));
    velc_ = Teuchos::rcp(new std::vector<WhetStone::VectorPolynomial>(ncells_owned_));
    jac_ = Teuchos::rcp(new std::vector<WhetStone::Polynomial>(ncells_owned_));

    plist_.set<std::string>("preconditioner type", "diagonal");
    std::vector<std::string> criteria;
    criteria.push_back("absolute residual");
    criteria.push_back("relative rhs");
    criteria.push_back("make one iteration");
    plist_.set<double>("error tolerance", 1e-12)
          .set<Teuchos::Array<std::string> >("convergence criteria", criteria)
          .sublist("verbose object")
          .set<std::string>("verbosity level", "none");

    tprint_ = 0.0;
    tl2_ = 0.0;
    npcg_ = 0;
    l2norm_ = -1.0;
  };
  ~RemapDG() {};

  void Setup(const Teuchos::Ptr<Operators::PDE_Abstract> op_adv,
             const Teuchos::Ptr<Operators::PDE_AdvectionRiemann> op_flux,
             const Teuchos::Ptr<Operators::PDE_Reaction> op_reac) {
    op_adv_ = op_adv;
    op_flux_ = op_flux;
    op_reac_ = op_reac;
  }

  virtual void Functional(double t, const CompositeVector& u, CompositeVector& f) override {
    UpdateGeometricQuantities_(t);

    // -- populate operators
    op_adv_->SetupPolyVector(velc_);
    op_adv_->UpdateMatrices();
    op_flux_->UpdateMatrices(velf_.ptr());

    op_reac_->Setup(jac_);
    op_reac_->UpdateMatrices(Teuchos::null);

    // -- solve the problem with mass matrix
    auto global_reac = op_reac_->global_operator();
    global_reac->SymbolicAssembleMatrix();
    global_reac->AssembleMatrix();
    global_reac->InitPreconditioner(plist_);

    AmanziSolvers::LinearOperatorPCG<Operators::Operator, CompositeVector, CompositeVectorSpace>
        pcg(global_reac, global_reac);
    pcg.Init(plist_);

    CompositeVector& x = *global_reac->rhs();  // reusing internal memory
    pcg.ApplyInverse(u, x);

    npcg_++;
    if (fabs(tprint_ - t) < 1e-6 && mesh_->get_comm()->MyPID() == 0) {
      printf("t=%8.5f  L2=%9.5g  pcg=(%d, %9.4g)  nsolvers=%d\n",
          t, l2norm_, pcg.num_itrs(), pcg.residual(), npcg_);
      tprint_ += 0.1;
    }

    // -- calculate right-hand_side
    op_flux_->global_operator()->Apply(x, f);
  }

  double L2Norm(double t, const CompositeVector& p1) {
    if (fabs(tl2_ - t) < 1e-6 && mesh_->get_comm()->MyPID() == 0) {
      CompositeVector p2(p1);

      ChangeVariables(t, p1, p2, false);
      p1.Dot(p2, &l2norm_);
      tl2_ += 0.1;
    }
    return l2norm_;
  }

  // access 
  const std::vector<WhetStone::VectorPolynomial> velf_vec() const { return velf_vec_; }
  const std::vector<WhetStone::VectorPolynomial> uc() const { return uc_; }
  const std::vector<WhetStone::MatrixPolynomial> J() const { return J_; }
  const Teuchos::RCP<std::vector<WhetStone::Polynomial> > jac() const { return jac_; }

  void ChangeVariables(double t, const CompositeVector& p1, CompositeVector& p2, bool flag) {
    UpdateGeometricQuantities_(t);
    op_reac_->Setup(jac_);
    op_reac_->UpdateMatrices(Teuchos::null);

    auto global_reac = op_reac_->global_operator();
    if (flag) {
      global_reac->Apply(p1, p2);
    } else {
      global_reac->SymbolicAssembleMatrix();
      global_reac->AssembleMatrix();
      global_reac->InitPreconditioner(plist_);

      AmanziSolvers::LinearOperatorPCG<Operators::Operator, CompositeVector, CompositeVectorSpace>
          pcg(global_reac, global_reac);
      pcg.Init(plist_);

      pcg.ApplyInverse(p1, p2);
    }
  }

 private:
  void UpdateGeometricQuantities_(double t) {
    for (int f = 0; f < nfaces_wghost_; ++f) {
      // cn = j J^{-t} N dA
      WhetStone::VectorPolynomial cn;

      maps_->NansonFormula(f, t, velf_vec_[f], cn);
      (*velf_)[f] = velf_vec_[f] * cn;
    }

    WhetStone::MatrixPolynomial C;
    for (int c = 0; c < ncells_owned_; ++c) {
      // pseudo cell velocity -C^t u 
      maps_->Cofactors(t, J_[c], C);
      (*velc_)[c].Multiply(C, uc_[c], true);

      for (int i = 0; i < dim_; ++i) {
        (*velc_)[c][i] *= -1.0;
      }

      // determinant of Jacobian
      maps_->Determinant(t, J_[c], (*jac_)[c]);
    }
  }

 private:
  Teuchos::RCP<const AmanziMesh::Mesh> mesh_;
  int ncells_owned_, nfaces_wghost_;
  int dim_;

  std::shared_ptr<WhetStone::MeshMaps> maps_;
  std::vector<WhetStone::VectorPolynomial> uc_;
  std::vector<WhetStone::MatrixPolynomial> J_;
  Teuchos::RCP<std::vector<WhetStone::Polynomial> > jac_;

  Teuchos::Ptr<Operators::PDE_Abstract> op_adv_;
  Teuchos::Ptr<Operators::PDE_AdvectionRiemann> op_flux_;
  Teuchos::Ptr<Operators::PDE_Reaction> op_reac_;

  Teuchos::RCP<std::vector<WhetStone::VectorPolynomial> > velc_;
  Teuchos::RCP<std::vector<WhetStone::Polynomial> > velf_;

  std::vector<WhetStone::VectorPolynomial> velf_vec_;

  Teuchos::ParameterList plist_;
  double tprint_, tl2_;
  int npcg_;
  double l2norm_;
};

} // namespace amanzi


/* *****************************************************************
* Remap of polynomilas in two dimensions. Explicit time scheme.
* Dual formulation places gradient and jumps on a test function.
***************************************************************** */
void RemapTestsDualRK(int order_p, int order_u,
                      const Amanzi::Explicit_TI::method_t& rk_method,
                      std::string maps_name, std::string file_name,
                      int nx, int ny, int nz, double dt) {
  using namespace Amanzi;
  using namespace Amanzi::AmanziMesh;
  using namespace Amanzi::AmanziGeometry;
  using namespace Amanzi::Operators;

  int dim = (nz == 0) ? 2 : 3;

  Epetra_MpiComm comm(MPI_COMM_WORLD);
  int MyPID = comm.MyPID();
  if (MyPID == 0) std::cout << "\nTest: " << dim << "D remap, dual formulation:"
                            << " mesh=" << ((nx == 0) ? "polygonal" : "square")
                            << ", orders: " << order_p << " " << order_u 
                            << ", maps=" << maps_name << std::endl;

  // polynomial space
  WhetStone::Polynomial pp(dim, order_p);
  int nk = pp.size();

  // create initial mesh
  MeshFactory meshfactory(&comm);
  meshfactory.set_partitioner(AmanziMesh::Partitioner_type::METIS);
  meshfactory.preference(FrameworkPreference({MSTK}));

  Teuchos::RCP<const Mesh> mesh0;
  if (dim == 2) {
    if (nx != 0) 
      mesh0 = meshfactory(0.0, 0.0, 1.0, 1.0, nx, ny);
    else 
      mesh0 = meshfactory(file_name, Teuchos::null);
  } else {
    mesh0 = meshfactory(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, nx, ny, nz, Teuchos::null, true, true);
  }

  int ncells_owned = mesh0->num_entities(AmanziMesh::CELL, AmanziMesh::OWNED);
  int ncells_wghost = mesh0->num_entities(AmanziMesh::CELL, AmanziMesh::USED);
  int nfaces_owned = mesh0->num_entities(AmanziMesh::FACE, AmanziMesh::OWNED);
  int nfaces_wghost = mesh0->num_entities(AmanziMesh::FACE, AmanziMesh::USED);
  int nnodes_owned = mesh0->num_entities(AmanziMesh::NODE, AmanziMesh::OWNED);
  int nnodes_wghost = mesh0->num_entities(AmanziMesh::NODE, AmanziMesh::USED);

  // create second and auxiliary mesh
  Teuchos::RCP<Mesh> mesh1;
  if (dim == 2) {
    if (nx != 0) 
      mesh1 = meshfactory(0.0, 0.0, 1.0, 1.0, nx, ny);
    else 
      mesh1 = meshfactory(file_name, Teuchos::null);
  } else {
    mesh1 = meshfactory(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, nx, ny, nz, Teuchos::null, true, true);
  }

  // deform the second mesh
  AmanziGeometry::Point xv(dim), yv(dim), xref(dim), uv(dim);
  Entity_ID_List nodeids, faces;
  AmanziGeometry::Point_List new_positions, final_positions;

  for (int v = 0; v < nnodes_wghost; ++v) {
    mesh1->node_get_coordinates(v, &xv);
    yv = xv;

    double ds(0.0001);
    for (int i = 0; i < 10000; ++i) {
      if (dim == 2) {
        uv[0] = 0.2 * std::sin(M_PI * xv[0]) * std::cos(M_PI * xv[1]);
        uv[1] =-0.2 * std::cos(M_PI * xv[0]) * std::sin(M_PI * xv[1]);
      } else {
        uv[0] = 0.2 * std::sin(M_PI * xv[0]) * std::cos(M_PI * xv[1]) * std::cos(M_PI * xv[2]);
        uv[1] =-0.1 * std::cos(M_PI * xv[0]) * std::sin(M_PI * xv[1]) * std::cos(M_PI * xv[2]);
        uv[2] =-0.1 * std::cos(M_PI * xv[0]) * std::cos(M_PI * xv[1]) * std::sin(M_PI * xv[2]);
        // uv[0] = 0.2 * std::sin(M_PI * xv[0]) * std::cos(M_PI * xv[1]);
        // uv[1] =-0.2 * std::cos(M_PI * xv[0]) * std::sin(M_PI * xv[1]);
      }
      xv += uv * ds;
    }

    nodeids.push_back(v);
    new_positions.push_back(xv);
  }
  mesh1->deform(nodeids, new_positions, false, &final_positions);

  // little factory of mesh maps
  std::shared_ptr<WhetStone::MeshMaps> maps;
  if (maps_name == "FEM") {
    maps = std::make_shared<WhetStone::MeshMaps_FEM>(mesh0, mesh1);
  } else if (maps_name == "VEM") {
    std::shared_ptr<WhetStone::MeshMaps_VEM> maps_vem;
    maps_vem = std::make_shared<WhetStone::MeshMaps_VEM>(mesh0, mesh1);
    maps_vem->set_order(order_u);  // higher-order velocity reconstruction
    maps = maps_vem;
  }

  // numerical integration
  WhetStone::NumericalIntegration numi(mesh0);

  // basic remap algorithm
  RemapDG remap(mesh0, maps);

  // create and initialize cell-based field 
  CompositeVectorSpace cvs1;
  cvs1.SetMesh(mesh0)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, nk);
  Teuchos::RCP<CompositeVector> p1 = Teuchos::rcp(new CompositeVector(cvs1));
  Epetra_MultiVector& p1c = *p1->ViewComponent("cell", true);

  // we need dg to compute scaling of basis functions
  WhetStone::DG_Modal dg(order_p, mesh0);

  AnalyticDG04 ana(mesh0, order_p);

  for (int c = 0; c < ncells_wghost; c++) {
    const AmanziGeometry::Point& xc = mesh0->cell_centroid(c);
    WhetStone::Polynomial coefs;
    ana.TaylorCoefficients(xc, 0.0, coefs);
    numi.ChangeBasisRegularToNatural(c, coefs);

    WhetStone::DenseVector data;
    coefs.GetPolynomialCoefficients(data);

    double a, b;
    for (auto it = coefs.begin(); it.end() <= coefs.end(); ++it) {
      int n = it.PolynomialPosition();

      dg.TaylorBasis(c, it, &a, &b);
      p1c[n][c] = data(n) / a;
      p1c[0][c] += data(n) * b;
    } 
  }

  // initial mass
  double mass0(0.0);
  for (int c = 0; c < ncells_owned; c++) {
    std::vector<double> data;
    for (int i = 0; i < nk; ++i) {
      data.push_back(p1c[i][c]);
    }
    WhetStone::Polynomial poly(dg.CalculatePolynomial(c, data));
    numi.ChangeBasisNaturalToRegular(c, poly);
    mass0 += numi.IntegratePolynomialCell(c, poly);
  }
  double mass_tmp(mass0);
  mesh0->get_comm()->SumAll(&mass_tmp, &mass0, 1);

  // allocate memory for global variables
  CompositeVectorSpace cvs2;
  cvs2.SetMesh(mesh1)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, nk);
  CompositeVector p2(cvs2);
  Epetra_MultiVector& p2c = *p2.ViewComponent("cell");

  // create flux operator
  Teuchos::ParameterList plist;
  plist.set<std::string>("method", "dg modal")
       .set<int>("method order", order_p)
       .set<bool>("jump operator on test function", true);

  plist.sublist("schema domain")
      .set<std::string>("base", "face")
      .set<Teuchos::Array<std::string> >("location", std::vector<std::string>({"cell"}))
      .set<Teuchos::Array<std::string> >("type", std::vector<std::string>({"scalar"}))
      .set<Teuchos::Array<int> >("number", std::vector<int>({nk}));

  plist.sublist("schema range") = plist.sublist("schema domain");

  Teuchos::RCP<PDE_AdvectionRiemann> op_flux = Teuchos::rcp(new PDE_AdvectionRiemann(plist, mesh0));
  auto global_op = op_flux->global_operator();

  // Attach volumetric advection operator to the flux operator.
  // We modify the existing parameter list.
  plist.set<std::string>("matrix type", "advection")
       .set<bool>("gradient operator on test function", true);
  plist.sublist("schema domain").set<std::string>("base", "cell");
  plist.sublist("schema range") = plist.sublist("schema domain");

  Teuchos::RCP<PDE_Abstract> op_adv = Teuchos::rcp(new PDE_Abstract(plist, global_op));

  // approximate accumulation term using the reaction operator
  plist.sublist("schema")
      .set<std::string>("base", "cell")
      .set<Teuchos::Array<std::string> >("location", std::vector<std::string>({"cell"}))
      .set<Teuchos::Array<std::string> >("type", std::vector<std::string>({"scalar"}))
      .set<Teuchos::Array<int> >("number", std::vector<int>({nk}));

  Teuchos::RCP<PDE_Reaction> op_reac = Teuchos::rcp(new PDE_Reaction(plist, mesh0));

  // explicit time integration
  CompositeVector p1aux(*p1);
  remap.Setup(op_adv.ptr(), op_flux.ptr(), op_reac.ptr());
  Explicit_TI::RK<CompositeVector> rk(remap, rk_method, p1aux);

  remap.ChangeVariables(0.0, *p1, p1aux, true);

  int nstep(0), nstep_dbg(0);
  double gcl, gcl_err(0.0);
  double t(0.0), tend(1.0);
  while(t < tend - dt/2) {
    remap.L2Norm(t, p1aux);
    rk.TimeStep(t, dt, p1aux, *p1);

    *p1aux.ViewComponent("cell") = *p1->ViewComponent("cell");

    t += dt;
    nstep++;
  }

  remap.ChangeVariables(1.0, *p1, p2, false);

  // calculate error in the new basis
  double pl2_err(0.0), pinf_err(0.0), area(0.0);
  double mass1(0.0), ql2_err(0.0), qinf_err(0.0);

  Entity_ID_List nodes;
  std::vector<int> dirs;
  AmanziGeometry::Point v0(dim), v1(dim), tau(dim);

  CompositeVectorSpace cvs3;
  cvs3.SetMesh(mesh1)->SetGhosted(true)->AddComponent("cell", AmanziMesh::CELL, 1);

  CompositeVector p2err(cvs3);
  Epetra_MultiVector& p2c_err = *p2err.ViewComponent("cell");
  p2err.PutScalar(0.0);

  for (int c = 0; c < ncells_owned; ++c) {
    double area_c(mesh1->cell_volume(c));

    std::vector<double> data;
    for (int i = 0; i < nk; ++i) {
      data.push_back(p2c[i][c]);
    }
    WhetStone::Polynomial poly(dg.CalculatePolynomial(c, data));
    numi.ChangeBasisNaturalToRegular(c, poly);

    if (nk == 1) {
      // const AmanziGeometry::Point& xg = maps->cell_geometric_center(1, c);
      const AmanziGeometry::Point& xg = mesh1->cell_centroid(c);
      double tmp = p2c[0][c] - ana.function_exact(xg, 0.0);

      pinf_err = std::max(pinf_err, fabs(tmp));
      pl2_err += tmp * tmp * area_c;

      p2c_err[0][c] = fabs(tmp);
    }
    else {
      mesh0->cell_get_nodes(c, &nodes);
      int nnodes = nodes.size();  
      for (int i = 0; i < nnodes; ++i) {
        mesh0->node_get_coordinates(nodes[i], &v0);
        mesh1->node_get_coordinates(nodes[i], &v1);

        double tmp = poly.Value(v0);
        tmp -= ana.function_exact(v1, 0.0);
        pinf_err = std::max(pinf_err, fabs(tmp));
        pl2_err += tmp * tmp * area_c / nnodes;

        p2c_err[0][c] = std::max(p2c_err[0][c], fabs(tmp));
      }
    }

    area += area_c;

    auto& jac = remap.jac();
    WhetStone::Polynomial tmp((*jac)[c]);
    tmp.ChangeOrigin(mesh0->cell_centroid(c));
    poly *= tmp;

    double mass1_c = numi.IntegratePolynomialCell(c, poly);
    mass1 += mass1_c;

    // optional projection on the space of polynomials 
    if (order_p > 0 && dim == 2) {
      poly = dg.CalculatePolynomial(c, data);
      numi.ChangeBasisNaturalToRegular(c, poly);

      maps->ProjectPolynomial(c, poly);
      poly.ChangeOrigin(mesh1->cell_centroid(c));

      if (order_p == 1) {
        poly(0, 0) = mass1_c / mesh1->cell_volume(c);
      }

      // error in the projected solution
      mesh0->cell_get_nodes(c, &nodes);
      int nnodes = nodes.size();  
      for (int i = 0; i < nnodes; ++i) {
        mesh1->node_get_coordinates(nodes[i], &v1);

        double tmp = poly.Value(v1);
        tmp -= ana.function_exact(v1, 0.0);
        qinf_err = std::max(qinf_err, fabs(tmp));
        ql2_err += tmp * tmp * area_c / nnodes;
      }
    }
  }

  // parallel collective operations
  double err_in[4] = {pl2_err, area, mass1, ql2_err};
  double err_out[4];
  mesh1->get_comm()->SumAll(err_in, err_out, 4);

  double err_tmp = pinf_err;
  mesh1->get_comm()->MaxAll(&err_tmp, &pinf_err, 1);

  err_tmp = qinf_err;
  mesh1->get_comm()->MaxAll(&err_tmp, &qinf_err, 1);

  err_tmp = gcl_err;
  mesh1->get_comm()->MaxAll(&err_tmp, &gcl_err, 1);

  // error tests
  pl2_err = std::pow(err_out[0], 0.5);
  ql2_err = std::pow(err_out[3], 0.5);
  CHECK(pl2_err < 0.12 / (order_p + 1));

  if (MyPID == 0) {
    printf("nx=%3d  L2=%12.8g %12.8g  Inf=%12.8g %12.8g dMass=%10.4g  dArea=%10.6g\n", 
        nx, pl2_err, ql2_err, pinf_err, qinf_err, err_out[2] - mass0, 1.0 - err_out[1]);
  }

  // visualization
  if (MyPID == 0) {
    GMV::open_data_file(*mesh1, (std::string)"operators.gmv");
    GMV::start_data();
    GMV::write_cell_data(p2c, 0, "remaped");
    if (order_p > 0) {
      GMV::write_cell_data(p2c, 1, "gradx");
      GMV::write_cell_data(p2c, 2, "grady");
    }

    GMV::write_cell_data(p2c_err, 0, "error");
    GMV::close_data_file();
  }
}


/*
TEST(REMAP_DUAL_FEM) {
  RemapTestsDualRK(0,1, Amanzi::Explicit_TI::heun_euler, "dg modal", "FEM", "", 10,10,0, 0.1);
  RemapTestsDualRK(1,2, Amanzi::Explicit_TI::heun_euler, "dg modal", "FEM", "", 10,10,0, 0.1);
}
*/

TEST(REMAP_DUAL_VEM) {
  RemapTestsDualRK(0,1, Amanzi::Explicit_TI::heun_euler, "VEM", "test/median15x16.exo", 0,0,0, 0.05);
  RemapTestsDualRK(1,2, Amanzi::Explicit_TI::heun_euler, "VEM", "test/median15x16.exo", 0,0,0, 0.05);
  // RemapTestsDualRK(2,3, Amanzi::Explicit_TI::heun_euler, "VEM", "test/median15x16.exo", 0,0,0, 0.05);
  RemapTestsDualRK(0,1, Amanzi::Explicit_TI::heun_euler, "VEM", "", 5,5,5, 0.2);
  // RemapTestsDualRK(1,2, Amanzi::Explicit_TI::heun_euler, "VEM", "", 5,5,5, 0.1);
}

/*
TEST(REMAP2D_DG_QUADRATURE_ERROR) {
  RemapTestsDualRK(2,1, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "",  16, 16,0, 0.05);
  RemapTestsDualRK(2,1, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "",  32, 32,0, 0.05 / 2);
  RemapTestsDualRK(2,1, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "",  64, 64,0, 0.05 / 4);
  RemapTestsDualRK(2,1, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "", 128,128,0, 0.05 / 8);
  RemapTestsDualRK(2,1, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "", 256,256,0, 0.05 / 16);
}
*/

/*
TEST(REMAP2D_DG_QUADRATURE_ERROR) {
  RemapTestsDualRK(2,3, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "test/median15x16.exo", 0,0,0, 0.05);
  RemapTestsDualRK(2,3, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "test/median32x33.exo", 0,0,0, 0.05 / 2);
  RemapTestsDualRK(2,3, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "test/median63x64.exo", 0,0,0, 0.05 / 4);
  RemapTestsDualRK(2,3, Amanzi::Explicit_TI::heun_euler, "VEM", "test/median127x128.exo", 0,0,0, 0.05 / 8);
  RemapTestsDualRK(2,3, Amanzi::Explicit_TI::heun_euler, "VEM", "test/median255x256.exo", 0,0,0, 0.05 / 16);
}
*/

/*
TEST(REMAP2D_DG_QUADRATURE_ERROR) {
  RemapTestsDualRK(2,3, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "test/random10.exo", 0,0,0, 0.05);
  RemapTestsDualRK(2,3, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "test/random20.exo", 0,0,0, 0.05 / 2);
  RemapTestsDualRK(2,3, Amanzi::Explicit_TI::tvd_3rd_order, "VEM", "test/random40.exo", 0,0,0, 0.05 / 4);
}
*/

