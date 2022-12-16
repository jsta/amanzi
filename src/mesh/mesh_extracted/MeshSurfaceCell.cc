/* -*-  mode: c++; c-default-style: "google"; indent-tabs-mode: nil -*- */
//
// This is a mesh for a single surface cell.
//
// This exists solely because we need "surface meshes" extracted from
// MeshColumn.  This is really just 1 cell.  Really.
//

#include <vector>
#include <string>
#include <algorithm>

#include "Teuchos_ParameterList.hpp"
#include "Epetra_Map.h"
#include "AmanziComm.hh"
#include "Epetra_SerialComm.h"

#include "VerboseObject.hh"
#include "dbc.hh"
#include "errors.hh"

#include "Region.hh"
#include "MeshSurfaceCell.hh"

namespace Amanzi {
namespace AmanziMesh {

MeshSurfaceCell::MeshSurfaceCell(const Teuchos::RCP<const MeshFramework>& parent_mesh,
        bool flatten)
    : MeshFramework(parent_mesh->getComm(),
                    parent_mesh->getGeometricModel(),
                    parent_mesh->getParameterList())
{
  parent_ = parent_mesh;

  // set dimensions
  if (flatten) {
    setSpaceDimension(2);
  } else {
    setSpaceDimension(3);
  }
  setManifoldDimension(2);
  parent_face_ = 0; // the parent face is always the 0th by construction of a
                    // MeshColumn

  // set my nodes
  parent_->getFaceNodes(parent_face_, node_parents_);
  nodes_.resize(node_parents_.size());
  if (flatten) {
    for (int i=0; i!=node_parents_.size(); ++i) {
      auto parent_node = parent_->getNodeCoordinate(node_parents_[i]);
      AmanziGeometry::Point child_node(2);
      child_node[0] = parent_node[0];
      child_node[1] = parent_node[1];
      nodes_[i] = child_node;
    }
  } else {
    for (int i=0; i!=node_parents_.size(); ++i) {
      nodes_[i] = parent_->getNodeCoordinate(node_parents_[i]);
    }
  }

  // set the cell type
  if (nodes_.size() == 3) {
    cell_type_ = Cell_type::TRI;
  } else if (nodes_.size() == 4) {
    cell_type_ = Cell_type::QUAD;
  } else {
    cell_type_ = Cell_type::POLYGON;
  }
}

// Number of entities of any kind (cell, face, node) and in a
// particular category (OWNED, GHOST, ALL)
std::size_t MeshSurfaceCell::getNumEntities(const Entity_kind kind,
        const Parallel_type ptype) const
{
  int count;
  switch (kind) {
    case Entity_kind::CELL:
      count = 1;
      break;

    default: // num_nodes == num_faces == num_boundary_faces
      count = nodes_.size();
      break;
  }
  return count;
}


// Downward Adjacencies
//---------------------
// Get nodes of a cell
void MeshSurfaceCell::getCellNodes(const Entity_ID cellid,
        Entity_ID_List& nodeids) const
{
  AMANZI_ASSERT(cellid == 0);
  nodeids.resize(nodes_.size());
  for (int i=0; i!=nodes_.size(); ++i) nodeids[i] = i;
}


// Get nodes of face
// On a distributed mesh, all nodes (OWNED or GHOST) of the face
// are returned
// In 3D, the nodes of the face are returned in ccw order consistent
// with the face normal
// In 2D, nfnodes is 2
void MeshSurfaceCell::getFaceNodes(const Entity_ID faceid,
        Entity_ID_List& nodeids) const
{
  AMANZI_ASSERT(faceid < nodes_.size());
  nodeids = { faceid, (faceid + 1) % (int) nodes_.size() };
}


// Cells of type 'ptype' connected to a node - The order of cells is
// not guaranteed to be the same for corresponding nodes on
// different processors
void MeshSurfaceCell::getNodeCells(const Entity_ID nodeid,
        const Parallel_type ptype,
        Entity_ID_List& cellids) const
{
  AMANZI_ASSERT(nodeid < nodes_.size());
  cellids = {0};
}


// Faces of type 'ptype' connected to a node - The order of faces is
// not guarnateed to be the same for corresponding nodes on
// different processors
void MeshSurfaceCell::getNodeFaces(const Entity_ID nodeid,
        const Parallel_type ptype,
        Entity_ID_List& faceids) const {
  faceids = { (nodeid - 1) % (int) nodes_.size(), nodeid };
}


// get faces and face dirs of a cell. This can be called by
// cell_get_faces_and_dirs method of the base class and the data
// cached or it can be called directly by the
// cell_get_faces_and_dirs method of this class
void MeshSurfaceCell::getCellFacesAndDirs(const Entity_ID cellid,
        Entity_ID_List& faceids,
        Entity_Direction_List * const face_dirs) const
{
  AMANZI_ASSERT(cellid == 0);
  faceids.resize(nodes_.size());
  for (int i=0; i!=nodes_.size(); ++i) faceids[i] = i;
  if (face_dirs) face_dirs->resize(nodes_.size(),1);
}


// Cells connected to a face - this function is implemented in each
// mesh framework. The results are cached in the base class
void MeshSurfaceCell::getFaceCells(const Entity_ID faceid,
        const Parallel_type ptype,
        Entity_ID_List& cellids) const {
  cellids.resize(1,0);
}


} // close namespace AmanziMesh
} // close namespace Amanzi




