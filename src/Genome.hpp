/*
  Copyright (C) 2019  Biagio Festa

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef AIMAZE2__GENOME__HPP
#define AIMAZE2__GENOME__HPP
#include <limits>
#include <utility>
#include <vector>
#include "ConfigEvolution.hpp"
#include "GeneConnection.hpp"
#include "GeneNode.hpp"
#include "InnovationHistory.hpp"

namespace aimaze2 {

class Genome {
 public:
  using NodeID = GeneNode::NodeID;
  using LayerID = GeneNode::LayerID;
  using NodeType = GeneNode::NodeType;
  using InnovationNum = GeneConnection::InnovationNum;
  static constexpr LayerID kIDLayerInputs = 0;
  static constexpr LayerID kIDLayerOutpus = std::numeric_limits<LayerID>::max();

  static Genome CreateSimpleGenome(const int numInputs, const int numOutputs);

  static Genome Crossover(Genome* iGenomeA,
                          Genome* iGenomeB,
                          ConfigEvolution::RndEngine* iRndEngine);

  std::pair<GeneNode*, int> getMutableInputNodes() noexcept;
  std::pair<GeneNode*, int> getMutableOutputNodes() noexcept;
  std::vector<GeneNode>* getMutableIONodes() noexcept;
  std::vector<GeneNode>* getMutableHiddenNodes() noexcept;
  const std::vector<GeneNode>& getIONodes() const noexcept;
  const std::vector<GeneNode>& getHiddenNodes() const noexcept;

  void feedForward();

  int getTotalNumNodes() const noexcept;

  int getNumHiddenNodes() const noexcept;

  int getNumConnections() const noexcept;

  int getNumActiveConnections() const noexcept;

  int getNumLayers() const noexcept;

  /*! \note Preconditions: The nodes can be linked.
   *  \see canBeLinked
   */
  void addConnection(const NodeID iNodeFromID,
                     const NodeID iNodeToID,
                     const float iWeight,
                     InnovationHistory* ioInnovationHistory);

  NodeID addNode(const NodeID iNodeFromID,
                 const NodeID iNodeToID,
                 InnovationHistory* ioInnovationHistory,
                 const bool iAddBias);

  /*! \brief Checks whether two nodes can be linked or not.
   *  \note That is:
   *        - nodeFrom.layer < nodeTo.layer.
   *        - there are not already linked.
   */
  bool canBeLinked(const NodeID iNodeFromID, const NodeID iNodeToID) const;

  /*! \brief Checkes whethere two nodes are linked or not.
   *  \note This method takes into account the 'connection's direction'!
   */
  bool areAlreadyLinked(const NodeID iNodeFromID, const NodeID iNodeToID) const;

  bool isConnectionReferBias(const GeneConnection& iConnection);

  const GeneNode& getBiasNode() const noexcept;

  const std::vector<GeneConnection>& getConnections() const noexcept;

  std::vector<GeneConnection>* getMutableConnections() noexcept;

  void sortConnectionsByInnovationNum();

  bool isValid() const;

  bool isSameSpecie(const Genome& iGenome) const;

 private:
  NodeID _nextNodeId = 0;
  int _numLayers = 0;
  int _numInputs = 0;
  int _numOutputs = 0;
  std::vector<GeneNode> _geneNodesIO;
  std::vector<GeneNode> _geneNodesHidden;
  std::vector<GeneConnection> _geneConnections;

  static Genome CopyNodeStructureFrom(const Genome& iGenome);

  Genome() = default;
  Genome(const int numInputs, const int numOutputs);

  /*! \param [out] oIncomeConnections   Just a temporary vector for computing
   *                                    connections.
   */
  void engageNode(GeneNode* ioNode,
                  std::vector<const GeneConnection*>* oIncomeConnections);

  NodeID addNode(GeneConnection* iConnection,
                 InnovationHistory* ioInnovationHistory,
                 const bool iAddBias);

  GeneConnection* getConnectionAmongNodes(const NodeID iNodeFromID,
                                          const NodeID iNodeToID);

  const GeneNode* getGeneNodeByID(const NodeID iNodeID) const;

  void computeIncomeConnections(
      const GeneNode& iNode,
      std::vector<const GeneConnection*>* oIncomeConnections) const;

  bool areIONodesValidType() const;
  bool areHiddenNodesValidType() const;
  bool areConnectionsValid() const;
  bool isNumLayersValid() const;
  bool areSameInnovationNumberInConnections() const;

  static float computeSimilaritySpecie(const Genome& iGenomeA,
                                       const Genome& iGenomeB);
};

}  // namespace aimaze2

#endif  // AIMAZE2__GENOME__HPP
