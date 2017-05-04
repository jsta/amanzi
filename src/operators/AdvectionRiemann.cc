/*
  Operators 

  Copyright 2010-201x held jointly by LANS/LANL, LBNL, and PNNL. 
  Amanzi is released under the three-clause BSD License. 
  The terms of use and "as is" disclaimer for this license are 
  provided in the top-level COPYRIGHT file.

  Author: Konstantin Lipnikov (lipnikov@lanl.gov)
*/

#include <vector>

#include "mfd3d_elasticity.hh"

#include "AdvectionRiemann.hh"
#include "OperatorDefs.hh"
#include "Operator_Schema.hh"
#include "Op_Cell_Schema.hh"
#include "Op_Face_Schema.hh"

namespace Amanzi {
namespace Operators {

/* ******************************************************************
* Initialize operator from parameter list.
****************************************************************** */
void AdvectionRiemann::InitAdvection_(Teuchos::ParameterList& plist)
{
  Teuchos::ParameterList& range = plist.sublist("schema range");
  Teuchos::ParameterList& domain = plist.sublist("schema domain");

  if (global_op_ == Teuchos::null) {
    // constructor was given a mesh
    // -- range schema and cvs
    local_schema_row_.Init(range, mesh_);
    global_schema_row_ = local_schema_row_;

    Teuchos::RCP<CompositeVectorSpace> cvs_row = Teuchos::rcp(new CompositeVectorSpace());
    cvs_row->SetMesh(mesh_)->SetGhosted(true);

    for (auto it = global_schema_row_.items().begin(); it != global_schema_row_.items().end(); ++it) {
      std::string name(local_schema_row_.KindToString(it->kind));
      cvs_row->AddComponent(name, it->kind, it->num);
    }

    // -- domain schema and cvs
    local_schema_col_.Init(domain, mesh_);
    global_schema_col_ = local_schema_col_;

    Teuchos::RCP<CompositeVectorSpace> cvs_col = Teuchos::rcp(new CompositeVectorSpace());
    cvs_col->SetMesh(mesh_)->SetGhosted(true);

    for (auto it = global_schema_col_.items().begin(); it != global_schema_col_.items().end(); ++it) {
      std::string name(local_schema_col_.KindToString(it->kind));
      cvs_col->AddComponent(name, it->kind, it->num);
    }

    global_op_ = Teuchos::rcp(new Operator_Schema(cvs_row, cvs_col, plist, global_schema_row_, global_schema_col_));
    if (local_schema_col_.base() == AmanziMesh::CELL) {
      local_op_ = Teuchos::rcp(new Op_Cell_Schema(global_schema_row_, global_schema_col_, mesh_));
    } else if (local_schema_col_.base() == AmanziMesh::FACE) {
      local_op_ = Teuchos::rcp(new Op_Face_Schema(global_schema_row_, global_schema_col_, mesh_));
    }

  } else {
    // constructor was given an Operator
    global_schema_row_ = global_op_->schema_row();
    global_schema_col_ = global_op_->schema_col();

    mesh_ = global_op_->DomainMap().Mesh();
    local_schema_row_.Init(range, mesh_);
    local_schema_col_.Init(domain, mesh_);

    local_op_ = Teuchos::rcp(new Op_Cell_Schema(global_schema_row_, global_schema_col_, mesh_));
  }

  // register the advection Op
  global_op_->OpPushBack(local_op_);

  // parameters
  // -- discretization method
  std::string name = plist.get<std::string>("discretization", "none");
  if (name == "BernardiRaugel") {
    space_col_ = BERNARDI_RAUGEL;
    space_row_ = P0;
  } else if (name == "DG order 0: face" || name == "DG order 0: cell") {
    space_col_ = DG0;
    space_row_ = DG0;
  } else {
    Errors::Message msg;
    msg << "Discretization method is either missing or invalid.";
    Exceptions::amanzi_throw(msg);
  }

  // -- fluxes
  flux_ = plist.get<std::string>("flux formula", "velocity");
  riemann_ = plist.get<std::string>("riemann problem", "continuous");
  
  // mesh info
  ncells_owned = mesh_->num_entities(AmanziMesh::CELL, AmanziMesh::OWNED);
  nfaces_owned = mesh_->num_entities(AmanziMesh::FACE, AmanziMesh::OWNED);
  nnodes_owned = mesh_->num_entities(AmanziMesh::NODE, AmanziMesh::OWNED);

  ncells_wghost = mesh_->num_entities(AmanziMesh::CELL, AmanziMesh::USED);
  nfaces_wghost = mesh_->num_entities(AmanziMesh::FACE, AmanziMesh::USED);
  nnodes_wghost = mesh_->num_entities(AmanziMesh::NODE, AmanziMesh::USED);
}


/* ******************************************************************
* A simple first-order transport method.
* Advection operator is of the form: div (u C), where u is the given
* velocity field and C is the advected field.
****************************************************************** */
void AdvectionRiemann::UpdateMatrices(const CompositeVector& u)
{
  if (local_schema_row_.base() == AmanziMesh::CELL) {
    UpdateMatricesCell_(u);
  } else if (local_schema_row_.base() == AmanziMesh::FACE) {
    UpdateMatricesFace_(u);
  }
}


/* ******************************************************************
* A simple first-order transport method.
* Advection operator is of the form: div (u C), where u is the given
* velocity field and C is the advected field.
****************************************************************** */
void AdvectionRiemann::UpdateMatricesCell_(const CompositeVector& u)
{
  std::vector<WhetStone::DenseMatrix>& matrix = local_op_->matrices;
  std::vector<WhetStone::DenseMatrix>& matrix_shadow = local_op_->matrices_shadow;

  AmanziMesh::Entity_ID_List nodes;
  int d = mesh_->space_dimension();

  WhetStone::MFD3D_Elasticity mfd(mesh_);

  for (int c = 0; c < ncells_owned; ++c) {
    if (space_col_ == BERNARDI_RAUGEL && space_row_ == P0) { 
      mesh_->cell_get_nodes(c, &nodes);
      int nnodes = nodes.size();
      int nfaces = mesh_->cell_get_num_faces(c);
      int ndofs = d * nnodes + nfaces;

      WhetStone::DenseMatrix Acell(1, ndofs);
      mfd.DivergenceMatrixBernardiRaugel(c, Acell);

      matrix[c] = Acell;
    }
  }
}


/* ******************************************************************
* A simple first-order transport method.
* Advection operator is of the form: div (u C), where u is the given
* velocity field and C is the advected field.
****************************************************************** */
void AdvectionRiemann::UpdateMatricesFace_(const CompositeVector& u)
{
  std::vector<WhetStone::DenseMatrix>& matrix = local_op_->matrices;
  std::vector<WhetStone::DenseMatrix>& matrix_shadow = local_op_->matrices_shadow;

  int dir;
  AmanziMesh::Entity_ID_List cells;
  const Epetra_MultiVector& uf = *u.ViewComponent("face");

  for (int f = 0; f < nfaces_owned; ++f) {
    mesh_->face_get_cells(f, AmanziMesh::USED, &cells);
    int ncells = cells.size();

    int ndofs = ncells * (local_schema_col_.items())[0].num;
    WhetStone::DenseMatrix Aface(ndofs, ndofs);
    Aface.PutScalar(0.0);

    if (ncells == 2) {
      mesh_->face_normal(f, false, cells[0], &dir);
      double u = uf[0][f] * dir * mesh_->face_area(f);

      int i = (u > 0.0) ? 1 : 0;  // downwind cell
      Aface(i, i) = -u;
      Aface(i, 1 - i) = u;
    }

    matrix[f] = Aface;
  }
}


/* *******************************************************************
* Apply boundary condition to the local matrices
******************************************************************* */
void AdvectionRiemann::ApplyBCs(bool primary, bool eliminate)
{
  for (auto bc = bcs_trial_.begin(); bc != bcs_trial_.end(); ++bc) {
    if ((*bc)->kind() == AmanziMesh::FACE) {
      ApplyBCs_Face(bc->ptr(), local_op_, primary, eliminate);
    } 
    else if ((*bc)->kind() == AmanziMesh::NODE) {
      ApplyBCs_Node(bc->ptr(), local_op_, primary, eliminate);
    }
  }
}


/* *******************************************************************
* Identify the advected flux of u
******************************************************************* */
void AdvectionRiemann::UpdateFlux(
    const CompositeVector& h, const CompositeVector& u,
    const Teuchos::RCP<BCs>& bc, CompositeVector& flux)
{
  h.ScatterMasterToGhosted("cell");
  
  const Epetra_MultiVector& h_f = *h.ViewComponent("face", true);
  const Epetra_MultiVector& u_f = *u.ViewComponent("face", false);
  Epetra_MultiVector& flux_f = *flux.ViewComponent("face", false);

  flux.PutScalar(0.0);
  for (int f = 0; f < nfaces_owned; ++f) {
    flux_f[0][f] = u_f[0][f] * h_f[0][f];
  }  
}

}  // namespace Operators
}  // namespace Amanzi
