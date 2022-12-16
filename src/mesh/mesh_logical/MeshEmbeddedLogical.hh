/* -*-  mode: c++; c-default-style: "google"; indent-tabs-mode: nil -*- */
#ifndef AMANZI_EMBEDDED_LOGICAL_MESH_H_
#define AMANZI_EMBEDDED_LOGICAL_MESH_H_

#include <Epetra_Map.h>
#include <AmanziComm.hh>
#include <Epetra_SerialComm.h>

#include <memory>
#include <vector>

#include "Teuchos_ParameterList.hpp"
#include "Mesh.hh"
#include "Region.hh"

#include "VerboseObject.hh"
#include "dbc.hh"
#include "errors.hh"

namespace Amanzi {

namespace AmanziMesh {

// An embedded logical mesh is the union of a standard volumetric mesh and a
// logical mesh.
//
// Connections between the two meshes must be provided.

class MeshEmbeddedLogical : public MeshFramework {
 public:
  //
  // MeshEmbeddedLogical Constructor
  //
  // Combines two meshes, each representing their own domain, into a
  // single mesh via set of specified connections.  The result is a
  // logical mesh in the sense that it provides a limited interface.
  //
  //  - face_cell_list : length nfaces array of length 2 arrays defining the
  //                     topology.  Note that face_cell_list[i][0] is a cell
  //                     LID in the logical mesh, while face_cell_list[i][1] is
  //                     a cell LID in the background mesh.
  //  - face_cell_lengths       : length of the cell-to-face connection
  //  - face_area_normals       : length nfaces array of normals of the
  //                              face, points from cell 1 to 2 in
  //                              face_cell_list topology, magnitude
  //                              is area
  MeshEmbeddedLogical(const Comm_ptr_type& comm,
                      Teuchos::RCP<MeshFramework> bg_mesh,
                      Teuchos::RCP<MeshFramework> log_mesh,
                      const std::vector<std::vector<int> >& face_cell_list,
                      const std::vector<std::vector<double> >& face_cell_lengths,
                      const std::vector<AmanziGeometry::Point>& face_area_normals,
                      const Teuchos::RCP<Teuchos::ParameterList>& plist=Teuchos::null);

  virtual bool hasNodes() const override { return false; }

  // Some meshes are logical meshes and do not have coordinate info.
  virtual bool isLogical() const override { return false; }

  // Parent entity in the source mesh if mesh was derived from another mesh
  virtual
  Entity_ID getEntityParent(const Entity_kind kind, const Entity_ID entid) const override;

  // Get cell type - UNKNOWN, TRI, QUAD, POLYGON, TET, PRISM, PYRAMID, HEX, POLYHED
  // See MeshDefs.hh
  virtual
  Cell_type getCellType(const Entity_ID cellid) const override;

  //
  // General mesh information
  // -------------------------
  //
  // Number of entities of any kind (cell, face, node) and in a
  // particular category (OWNED, GHOST, ALL)
  virtual
  std::size_t getNumEntities(const Entity_kind kind,
                             const Parallel_type ptype) const override;


  // Global ID of any entity
  virtual
  Entity_GID getEntityGID(const Entity_kind kind, const Entity_ID lid) const override;


  // All nodal methods throw -- there are no nodes in MeshLogical
  virtual AmanziGeometry::Point
  getNodeCoordinate(const Entity_ID node) const override;

  virtual void
  getFaceNodes(const Entity_ID f, Entity_ID_List& nodes) const override;

  virtual void
  getNodeFaces(const Entity_ID nodeid,
               const Parallel_type ptype,
               Entity_ID_List& faceids) const override;


  //
  // These are the important ones -- MeshLogical defines cell quantities
  //
  virtual void getCellFacesAndDirs(
    const Entity_ID c,
    Entity_ID_List& faces,
    Entity_Direction_List * const dirs) const override;

  // Get the bisectors, i.e. vectors from cell centroid to face centroids.
  virtual void getCellFacesAndBisectors(
          const Entity_ID cellid,
          Entity_ID_List& faceids,
          Point_List * const bisectors) const override;

  virtual double getCellVolume(const Entity_ID c) const override;
  virtual AmanziGeometry::Point getCellCentroid(const Entity_ID c) const override;

  //
  // MeshLogical defines face quantities
  //
  virtual void getFaceCells(const Entity_ID f,
                            const Parallel_type ptype,
                            Entity_ID_List& cells) const override;
  virtual double getFaceArea(const Entity_ID f) const override;
  virtual AmanziGeometry::Point getFaceCentroid(const Entity_ID f) const override;
  virtual AmanziGeometry::Point getFaceNormal(const Entity_ID f,
          const Entity_ID c, int * const orientation=nullptr) const override;

 protected:

  RaggedArray_DualView<Entity_ID> extra_face_cell_ids_;
  Point_List extra_face_area_normals_;
  RaggedArray_DualView<double> extra_face_cell_lengths_;
  RaggedArray_DualView<AmanziGeometry::Point> extra_face_cell_bisectors_;

  Teuchos::RCP<MeshFramework> bg_mesh_;  // background mesh, typically a Mesh_MSTK
  Teuchos::RCP<MeshFramework> log_mesh_;  // embedded mesh, typically a MeshLogical

  Teuchos::RCP<Epetra_Map> cell_map_;
  Teuchos::RCP<Epetra_Map> face_map_;
};

}  // namespace AmanziMesh
}  // namespace Amanzi

#endif
