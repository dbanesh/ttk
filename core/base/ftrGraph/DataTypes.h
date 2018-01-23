/// \ingroup base
/// \class ttk::ftr::DataTypes
/// \author Charles Gueunet <charles.gueunet@lip6.fr>
/// \date 2018-01-19
///
///\brief TTK DataType management for the reeb graph
///
/// \sa ttkFTRGraph.h %for a usage example.

#pragma once

#include <functional>
#include <limits>
#include <set>
#include <tuple>

namespace ttk
{
namespace ftr
{
   // Types
   // --------

   /// \brief SuperArc index in vect_superArcs_
   using idSuperArc = long unsigned int;
   /// \brief Node index in vect_nodes_
   using idNode = unsigned int;
   /// \brief Vertex index in scalars_
   using idVertex = int;
   /// \brief Edge index in vect_edgeList_
   using idEdge = int;
   /// \brief Cell index in vect_cellList_
   using idCell = int;
   /// \brief for task identifiers
   using idTask = idNode;
   /// \brief for vertex up/down valence
   using valence = unsigned char;

   // For tasks:
   // Set using scalar value comparison
   using VertCompFN     = std::function<bool(idVertex, idVertex)>;

   // Special values for types
   // --------------------------

   // QUESTION impact on performance using max (0 would be faster alloacted)
   static const idSuperArc     nullSuperArc  = std::numeric_limits<idSuperArc>::max();
   static const idNode         nullNodes     = std::numeric_limits<idNode>::max();
   static const idVertex       nullVertex    = std::numeric_limits<idVertex>::max();
   static const idEdge         nullEdge      = std::numeric_limits<idEdge>::max();
   static const idCell         nullCell      = std::numeric_limits<idCell>::max();

   // Enum data
   // ----------

   enum class GraphComponent { Arc = -1, Local_minimum, Saddle1, Saddle2, Local_maximum };

   enum class ArcType:char { Min_arc = 0, Max_arc, Saddle1_arc, Saddle2_arc, Saddle1_saddle2_arc };

   enum class NodeType { Local_minimum = 0, Saddle1, Saddle2, Degenerate, Local_maximum, Regular };
}
}

