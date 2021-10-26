/*
 Shallow water PK
 
 Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL.
 Amanzi is released under the three-clause BSD License.
 The terms of use and "as is" disclaimer for this license are
 provided in the top-level COPYRIGHT file.
 */

// TPLs
#include "Teuchos_RCP.hpp"
#include "Teuchos_ParameterList.hpp"
#include "Teuchos_XMLParameterListHelpers.hpp"
#include "UnitTest++.h"

// Amanzi
#include "LeastSquare.hh"
#include "Mesh.hh"
#include "MeshFactory.hh"

#include "ShallowWater_PK.hh"

#include "OutputXDMF.hh"

//--------------------------------------------------------------
// Analytic solution
//--------------------------------------------------------------

void analytical_exact(double t, double x, double y, double &h, double &u, double &v)
{
// Thacker's time dependent solution [Beljadid et. al. 2016]
  double eta = 2.0, g = 9.81, T = 1.436739427831727;
//  double R0 = T * std::sqrt(2.0*g*eta);
  double R0 = 9.0;
  
  h = eta * ( (T*T)/(T*T + t*t) ) * ( 1.0 - ( (x*x + y*y)/(R0*R0) )*( (T*T)/(t*t + T*T) ) );
  u = (x*t)/(t*t + T*T);
  v = (y*t)/(t*t + T*T);
}


void analytical_exact_field(Teuchos::RCP<const Amanzi::AmanziMesh::Mesh> mesh,
                           Epetra_MultiVector& hh_ex,
                           Epetra_MultiVector& vel_ex, double t)
{
  double x, y, h, u, v;

  int ncells_owned = mesh->num_entities(Amanzi::AmanziMesh::CELL, Amanzi::AmanziMesh::Parallel_type::OWNED);

  for (int c = 0; c < ncells_owned; c++) {
    const Amanzi::AmanziGeometry::Point& xc = mesh->cell_centroid(c);

    x = xc[0];
    y = xc[1];

    analytical_exact(t, x, y, h, u, v);
    hh_ex[0][c] = h;
    vel_ex[0][c] = u;
    vel_ex[1][c] = v;
  }
}


void analytical_setIC(Teuchos::RCP<const Amanzi::AmanziMesh::Mesh> mesh, Teuchos::RCP<Amanzi::State>& S)
{
  int ncells_owned = mesh->num_entities(Amanzi::AmanziMesh::CELL, Amanzi::AmanziMesh::Parallel_type::OWNED);

  std::string passwd = "state";

  Epetra_MultiVector& B_vec_c = *S->GetFieldData("surface-bathymetry",passwd)->ViewComponent("cell");
  Epetra_MultiVector& h_vec_c = *S->GetFieldData("surface-ponded_depth",passwd)->ViewComponent("cell");
  Epetra_MultiVector& ht_vec_c = *S->GetFieldData("surface-total_depth",passwd)->ViewComponent("cell");
  Epetra_MultiVector& vel_vec_c = *S->GetFieldData("surface-velocity",passwd)->ViewComponent("cell");
  Epetra_MultiVector& q_vec_c = *S->GetFieldData("surface-discharge", "surface-discharge")->ViewComponent("cell");

  for (int c = 0; c < ncells_owned; c++) {
    const Amanzi::AmanziGeometry::Point& xc = mesh->cell_centroid(c);
    double h, u, v;
    analytical_exact(0., xc[0], xc[1], h, u, v);
    h_vec_c[0][c] = h;
    B_vec_c[0][c] = 0.0;
    ht_vec_c[0][c] = h_vec_c[0][c] + B_vec_c[0][c];
    vel_vec_c[0][c] = u;
    vel_vec_c[1][c] = v;
    q_vec_c[0][c] = u*h;
    q_vec_c[1][c] = v*h;
  }
}

void error(Teuchos::RCP<const Amanzi::AmanziMesh::Mesh> mesh,
           Epetra_MultiVector& hh_ex, Epetra_MultiVector& vel_ex,
           const Epetra_MultiVector& hh, const Epetra_MultiVector& vel,
           double& err_max, double& err_L1, double& hmax, double& err_max_u, double& err_L1_u, double& err_max_v, double& err_L1_v)
{
  int ncells_owned = mesh->num_entities(Amanzi::AmanziMesh::CELL, Amanzi::AmanziMesh::Parallel_type::OWNED);

  err_max = 0.;
  err_L1 = 0.;
  hmax = 0.;
  err_max_u = 0.;
  err_L1_u = 0.;
  err_max_v = 0.;
  err_L1_v = 0.;

  for (int c = 0; c < ncells_owned; c++) {
    double tmp = std::abs(hh_ex[0][c] - hh[0][c]);
    err_max = std::max(err_max, tmp);
    err_L1 += tmp * mesh->cell_volume(c);
    hmax = std::sqrt(mesh->cell_volume(c));
    
    tmp = std::abs(vel_ex[0][c] - vel[0][c]);
    err_max_u = std::max(err_max_u, tmp);
    err_L1_u += tmp * mesh->cell_volume(c);
    
    tmp = std::abs(vel_ex[1][c] - vel[1][c]);
    err_max_v = std::max(err_max_v, tmp);
    err_L1_v += tmp * mesh->cell_volume(c);
  }

  double err_max_tmp;
  double err_L1_tmp;
  double err_max_tmp_u;
  double err_L1_tmp_u;
  double err_max_tmp_v;
  double err_L1_tmp_v;

  mesh->get_comm()->MaxAll(&err_max, &err_max_tmp, 1);
  mesh->get_comm()->SumAll(&err_L1, &err_L1_tmp, 1);
  mesh->get_comm()->MaxAll(&err_max_u, &err_max_tmp_u, 1);
  mesh->get_comm()->SumAll(&err_L1_u, &err_L1_tmp_u, 1);
  mesh->get_comm()->MaxAll(&err_max_v, &err_max_tmp_v, 1);
  mesh->get_comm()->SumAll(&err_L1_v, &err_L1_tmp_v, 1);

  err_max = err_max_tmp;
  err_L1  = err_L1_tmp;
  
  err_max_u = err_max_tmp_u;
  err_L1_u  = err_L1_tmp_u;
  
  err_max_v = err_max_tmp_v;
  err_L1_v  = err_L1_tmp_v;

  std::cout << "err_max = " << err_max << std::endl;
  std::cout << "err_L1  = " << err_L1 << std::endl;
  
  std::cout << "err_max (u)= " << err_max_u << std::endl;
  std::cout << "err_L1 (u) = " << err_L1_u << std::endl;
  
  std::cout << "err_max (v)= " << err_max_v << std::endl;
  std::cout << "err_L1 (v) = " << err_L1_v << std::endl;
}


/* **************************************************************** */
TEST(SHALLOW_WATER_ANALYTICAL) {
  using namespace Teuchos;
  using namespace Amanzi;
  using namespace Amanzi::AmanziMesh;
  using namespace Amanzi::AmanziGeometry;
  using namespace Amanzi::ShallowWater;

  Comm_ptr_type comm = Amanzi::getDefaultComm();
  int MyPID = comm->MyPID();
  if (MyPID == 0) std::cout << "Test: 2D shallow water: analytical solution" << std::endl;

  // read parameter list
  std::string xmlFileName = "test/shallow_water_analytical.xml";
  Teuchos::RCP<Teuchos::ParameterList> plist = Teuchos::getParametersFromXmlFile(xmlFileName);

  /* create a mesh framework */
  ParameterList regions_list = plist->get<Teuchos::ParameterList>("regions");
  auto gm = Teuchos::rcp(new Amanzi::AmanziGeometry::GeometricModel(2, regions_list, *comm));

  // create a mesh
  bool request_faces = true, request_edges = false;
  MeshFactory meshfactory(comm,gm);
  meshfactory.set_preference(Preference({Framework::MSTK}));

  std::vector<double> dx, Linferror, L1error, L2error, dt_val, Linferror_u, L1error_u, Linferror_v, L1error_v;

  for (int NN = 100; NN <= 300; NN *= 3) {

    RCP<Mesh> mesh = meshfactory.create(-3.0, -3.0, 3.0, 3.0, NN, NN, request_faces, request_edges);
    // mesh = meshfactory.create("test/median63x64.exo",request_faces,request_edges);
    // works only with first order, no reconstruction

    // create a state
    Teuchos::ParameterList state_list = plist->sublist("state");
    RCP<State> S = rcp(new State(state_list));
    S->RegisterMesh("surface", mesh);
    S->set_time(0.0);

    Teuchos::RCP<TreeVector> soln = Teuchos::rcp(new TreeVector());

    Teuchos::ParameterList pk_tree = plist->sublist("PKs").sublist("shallow water");

    // create a shallow water PK
    ShallowWater_PK SWPK(pk_tree,plist,S,soln);
    SWPK.Setup(S.ptr());
    S->Setup();
    S->InitializeFields();
    S->InitializeEvaluators();
    SWPK.Initialize(S.ptr());

    analytical_setIC(mesh, S);
    S->CheckAllFieldsInitialized();

    const Epetra_MultiVector& hh = *S->GetFieldData("surface-ponded_depth")->ViewComponent("cell");
    const Epetra_MultiVector& ht = *S->GetFieldData("surface-total_depth")->ViewComponent("cell");
    const Epetra_MultiVector& vel = *S->GetFieldData("surface-velocity")->ViewComponent("cell");
    const Epetra_MultiVector& q = *S->GetFieldData("surface-discharge")->ViewComponent("cell");
    const Epetra_MultiVector& B = *S->GetFieldData("surface-bathymetry")->ViewComponent("cell");

    // create pid vector
    Epetra_MultiVector pid(B);
    for (int c = 0; c < pid.MyLength(); c++) pid[0][c] = MyPID;

    // create screen io
    auto vo = Teuchos::rcp(new Amanzi::VerboseObject("ShallowWater", pk_tree));
    WriteStateStatistics(*S, *vo);

    // advance in time
    double t_old(0.0), t_new(0.0), dt;

    // initialize io
    Teuchos::ParameterList iolist;
    std::string fname;
    fname = "SW_sol";
    iolist.get<std::string>("file name base", fname);
    OutputXDMF io(iolist, mesh, true, false);

    std::string passwd("state");

    int iter = 0;
    double dt_max = 0.0;

    while (t_new < 0.01) {
      // cycle 1, time t
      double t_out = t_new;

      Epetra_MultiVector hh_ex(hh);
      Epetra_MultiVector vel_ex(vel);

      analytical_exact_field(mesh, hh_ex, vel_ex, t_out);

      if (iter % 5 == 0) {
        io.InitializeCycle(t_out, iter, "");
        io.WriteVector(*hh(0), "depth", AmanziMesh::CELL);
        io.WriteVector(*ht(0), "total_depth", AmanziMesh::CELL);
        io.WriteVector(*vel(0), "vx", AmanziMesh::CELL);
        io.WriteVector(*vel(1), "vy", AmanziMesh::CELL);
        io.WriteVector(*q(0), "qx", AmanziMesh::CELL);
        io.WriteVector(*q(1), "qy", AmanziMesh::CELL);
        io.WriteVector(*B(0), "B", AmanziMesh::CELL);
        io.WriteVector(*pid(0), "pid", AmanziMesh::CELL);

        io.WriteVector(*hh_ex(0), "hh_ex", AmanziMesh::CELL);
        io.WriteVector(*vel_ex(0), "vx_ex", AmanziMesh::CELL);
        io.WriteVector(*vel_ex(1), "vy_ex", AmanziMesh::CELL);
        io.FinalizeCycle();
      }

//      dt = SWPK.get_dt();
      dt = 0.5/NN;
      dt_max = dt;
//      std::cout<<"dt: "<<dt<<std::endl;

      t_new = t_old + dt;

      SWPK.AdvanceStep(t_old, t_new);
      SWPK.CommitStep(t_old, t_new, S);

      t_old = t_new;
      iter++;
    }

    if (MyPID == 0) std::cout << "Time-stepping finished." << std::endl;

    // cycle 1, time t
    double t_out = t_new;

    Epetra_MultiVector hh_ex(hh);
    Epetra_MultiVector vel_ex(vel);

    analytical_exact_field(mesh, hh_ex, vel_ex, t_out);

    double err_max, err_L1, hmax, err_max_u, err_L1_u, err_max_v, err_L1_v;

    error(mesh, hh_ex, vel_ex, hh, vel, err_max, err_L1, hmax, err_max_u, err_L1_u, err_max_v, err_L1_v);

    dt_val.push_back(dt_max);
    dx.push_back(hmax);
    Linferror.push_back(err_max);
    L1error.push_back(err_L1);
    Linferror_u.push_back(err_max_u);
    L1error_u.push_back(err_L1_u);
    Linferror_v.push_back(err_max_v);
    L1error_v.push_back(err_L1_v);

    io.InitializeCycle(t_out, iter, "");
    io.WriteVector(*hh(0), "depth", AmanziMesh::CELL);
    io.WriteVector(*ht(0), "total_depth", AmanziMesh::CELL);
    io.WriteVector(*vel(0), "vx", AmanziMesh::CELL);
    io.WriteVector(*vel(1), "vy", AmanziMesh::CELL);
    io.WriteVector(*q(0), "qx", AmanziMesh::CELL);
    io.WriteVector(*q(1), "qy", AmanziMesh::CELL);
    io.WriteVector(*B(0), "B", AmanziMesh::CELL);
    io.WriteVector(*pid(0), "pid", AmanziMesh::CELL);

    io.WriteVector(*hh_ex(0), "hh_ex", AmanziMesh::CELL);
    io.WriteVector(*vel_ex(0), "vx_ex", AmanziMesh::CELL);
    io.WriteVector(*vel_ex(1), "vy_ex", AmanziMesh::CELL);
    io.FinalizeCycle();
    
    std::cout<<"dt: "<<dt_max<<std::endl;
  } // NN
  
  double L1_dt_order = Amanzi::Utils::bestLSfit(dt_val, L1error);
  double Linf_dt_order = Amanzi::Utils::bestLSfit(dt_val, Linferror);
  
  double L1_dt_order_u = Amanzi::Utils::bestLSfit(dt_val, L1error_u);
  double Linf_dt_order_u = Amanzi::Utils::bestLSfit(dt_val, Linferror_u);
  
  double L1_dt_order_v = Amanzi::Utils::bestLSfit(dt_val, L1error_v);
  double Linf_dt_order_v = Amanzi::Utils::bestLSfit(dt_val, Linferror_v);

  std::cout << "computed order L1 (dt) = " << L1_dt_order << std::endl;
  std::cout << "computed order Linfty (dt) = " << Linf_dt_order << std::endl;
  
  std::cout << "computed order L1 (dt) u = " << L1_dt_order_u << std::endl;
  std::cout << "computed order Linfty (dt) u = " << Linf_dt_order_u << std::endl;
  
  std::cout << "computed order L1 (dt) v = " << L1_dt_order_v << std::endl;
  std::cout << "computed order Linfty (dt) v = " << Linf_dt_order_v << std::endl;

//  CHECK(order > 0.9);  // first order scheme
}
