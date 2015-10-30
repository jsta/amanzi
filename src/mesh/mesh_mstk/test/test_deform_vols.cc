#include <UnitTest++.h>

#include <iostream>

#include "../Mesh_MSTK.hh"

#include "MeshAudit.hh"

#include "Epetra_Map.h"
#include "Epetra_MpiComm.h"
#include "Teuchos_ParameterList.hpp"
#include "Teuchos_Array.hpp"
#include "Teuchos_XMLParameterListHelpers.hpp"


TEST(MSTK_DEFORM_VOLS_2D)
{

  Teuchos::RCP<Epetra_MpiComm> comm(new Epetra_MpiComm(MPI_COMM_WORLD));

  // Define a box region to capture bottom boundary
  Teuchos::ParameterList param_list;
  Teuchos::ParameterList& regions_list = param_list.sublist("Regions");
  Teuchos::ParameterList& botreg_list = regions_list.sublist("Bottom Region");
  Teuchos::ParameterList& botreg_def = botreg_list.sublist("Region: Box");
  Teuchos::Array<double> lo_coord = Teuchos::tuple(-5.1,-0.01);
  Teuchos::Array<double> hi_coord = Teuchos::tuple(5.1,0.01);
  botreg_def.set< Teuchos::Array<double> >("Low Coordinate",lo_coord);
  botreg_def.set< Teuchos::Array<double> >("High Coordinate",hi_coord);

  //  Teuchos::writeParameterListToXmlOStream(param_list,std::cout);

  if (comm->NumProc() > 1) return;

  // Create a geometric model from region spec

  Amanzi::AmanziGeometry::GeometricModelPtr gm = 
    new Amanzi::AmanziGeometry::GeometricModel(3, regions_list, comm.get());

  // Generate a mesh consisting of 3x3 elements 

  Teuchos::RCP<Amanzi::AmanziMesh::Mesh> 
      mesh(new Amanzi::AmanziMesh::Mesh_MSTK(-5.0,0.0,5.0,10.0,10,10,comm.get(),gm));

  int nc = 
    mesh->num_entities(Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::USED);

  // Request target volume of 50% for some cells at the bottom of the center column
  // The others are unconstrained except for a barrier of minimum volume

  std::vector<double> orig_volumes, target_volumes, target_weights, min_volumes;
  orig_volumes.reserve(nc);
  target_volumes.reserve(nc);
  target_weights.reserve(nc);
  min_volumes.reserve(nc);

  for (int i = 0; i < nc; i++) {    
    orig_volumes[i] = mesh->cell_volume(i);
    // target_volumes[i] = orig_volumes[i];
    target_volumes[i] = 0.0;
    min_volumes[i] = 0.25*orig_volumes[i];

    Amanzi::AmanziGeometry::Point ccen = mesh->cell_centroid(i);
    if (fabs(ccen[0]) < 3.0)
      if (ccen[1] > 3.1 && ccen[1] < 4.1)
        target_volumes[i] = 0.90*orig_volumes[i];
  }

  Amanzi::AmanziMesh::Entity_ID_List fixed_nodes;
  mesh->get_set_entities("Bottom Region", Amanzi::AmanziMesh::NODE,
                         Amanzi::AmanziMesh::USED, &fixed_nodes);

  bool move_vertical = true;
  int status = mesh->deform(target_volumes,min_volumes,fixed_nodes,
                            move_vertical);
  CHECK(status);

  for (int i = 0; i < nc; i++) {
    if (target_volumes[i] > 0.0 && target_volumes[i] < orig_volumes[i]) {
      double voldiff = 
        (mesh->cell_volume(i)-target_volumes[i])/target_volumes[i];
    
      // Check if volume difference is with 5% of target volume
      CHECK_CLOSE(0,voldiff,5.0e-02);
    }

    // Check that we didn't fall below the minimum prescribed volume
    if (mesh->cell_volume(i) < min_volumes[i])
      std::cerr << "Cell volume = " << mesh->cell_volume(i) << 
          " is less than min volume = " << min_volumes[i] << std::endl;
    //    CHECK(mesh->cell_volume(i) >= min_volumes[i]);
  }

  mesh->write_to_exodus_file("deformed2.exo");
}


TEST(MSTK_DEFORM_VOLS_3D)
{
  Teuchos::RCP<Epetra_MpiComm> comm(new Epetra_MpiComm(MPI_COMM_WORLD));

  // Define a box region to capture bottom boundary
  Teuchos::ParameterList param_list;
  Teuchos::ParameterList& regions_list = param_list.sublist("Regions");
  Teuchos::ParameterList& botreg_list = regions_list.sublist("Bottom Region");
  Teuchos::ParameterList& botreg_def = botreg_list.sublist("Region: Box");
  Teuchos::Array<double> lo_coord = Teuchos::tuple(-0.1,-0.1,-0.01);
  Teuchos::Array<double> hi_coord = Teuchos::tuple(10.1,10.1,0.01);
  botreg_def.set< Teuchos::Array<double> >("Low Coordinate",lo_coord);
  botreg_def.set< Teuchos::Array<double> >("High Coordinate",hi_coord);

  //  Teuchos::writeParameterListToXmlOStream(param_list,std::cout);

  // Create a geometric model from region spec

  Amanzi::AmanziGeometry::GeometricModelPtr gm = 
    new Amanzi::AmanziGeometry::GeometricModel(3, regions_list, comm.get());

  // Generate a mesh consisting of 10x10 elements 

  Teuchos::RCP<Amanzi::AmanziMesh::Mesh> 
      mesh(new Amanzi::AmanziMesh::Mesh_MSTK(0.0,0.0,0.0,10.0,1.0,10.0,10,1,10,comm.get(),gm));

  int nc = 
    mesh->num_entities(Amanzi::AmanziMesh::CELL,Amanzi::AmanziMesh::USED);

  // Request target volume of 50% for some cells at the bottom of the center column
  // The others are unconstrained except for a barrier of minimum volume

  std::vector<double> orig_volumes, target_volumes, target_weights, min_volumes;
  orig_volumes.reserve(nc);
  target_volumes.reserve(nc);
  target_weights.reserve(nc);
  min_volumes.reserve(nc);

  for (int i = 0; i < nc; i++) {    
    orig_volumes[i] = mesh->cell_volume(i);
    target_volumes[i] = orig_volumes[i];
    min_volumes[i] = orig_volumes[i]; // quite stringent - no wiggle room

    Amanzi::AmanziGeometry::Point ccen = mesh->cell_centroid(i);
    if (ccen[0] > 2.0 && ccen[0] < 8.0) { // row of cells along y axis
      if (ccen[2] > 3.1 && ccen[2] < 4.1)  {
        target_volumes[i] = 0.85*orig_volumes[i];
        min_volumes[i] = 0.770*orig_volumes[i];
      }
      else if (ccen[2] >= 4.1) {
        min_volumes[i] = 0.7*orig_volumes[i];
      }
    }
  }

  Amanzi::AmanziMesh::Entity_ID_List fixed_nodes;
  mesh->get_set_entities("Bottom Region", Amanzi::AmanziMesh::NODE,
                         Amanzi::AmanziMesh::USED, &fixed_nodes);

  bool move_vertical = true;
  int status = mesh->deform(target_volumes,min_volumes,fixed_nodes,
                            move_vertical);
  CHECK(status);

  for (int i = 0; i < nc; i++) {
    if (target_volumes[i] > 0.0 && target_volumes[i] < orig_volumes[i]) {
      double voldiff = (mesh->cell_volume(i)-target_volumes[i])/target_volumes[i];
    
      // Check if volume difference is with 5% of target volume
      CHECK_CLOSE(0,voldiff,7.5e-02);
    }

    // Check that we didn't fall below the minimum prescribed volume
    // but because we asked for many volumes to stay exactly the same
    // as the original (min_volume = target_volume = original_volume)
    // give some margin of numerical error

    double eps = 1.0e-6*orig_volumes[i];
    CHECK(mesh->cell_volume(i)+eps >= min_volumes[i]);
    if (!(mesh->cell_volume(i)+eps >= min_volumes[i])) {
      double diff = mesh->cell_volume(i)-min_volumes[i];
      std::cerr << "Cell " << i << ": Min volume = " << min_volumes[i] << "    "
                << "Cell volume = " << mesh->cell_volume(i)  << "  Diff = "
                << diff << std::endl;
    }
  }

  mesh->write_to_exodus_file("deformed3.exo");
}

