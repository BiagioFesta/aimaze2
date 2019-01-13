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
#include "Genome.hpp"
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace aimaze2 {

Genome Genome::CreateSimpleGenome(const int numInputs, const int numOutputs) {
  Genome genome(numInputs, numOutputs);

  genome._geneNodesIO.reserve(numInputs + numOutputs + 1);

  // inputs
  for (int i = 0; i < numInputs; ++i) {
    genome._geneNodesIO.emplace_back(
        NodeType::INPUT, genome._nextNodeId++, kIDLayerInputs, false);
  }

  // output
  for (int i = 0; i < numOutputs; ++i) {
    genome._geneNodesIO.emplace_back(
        NodeType::OUTPUT, genome._nextNodeId++, kIDLayerOutpus, false);
  }

  // bias inputs. Note bias is the last of IO vector
  genome._geneNodesIO.emplace_back(
      NodeType::INPUT, genome._nextNodeId++, kIDLayerInputs, true);

  genome._numLayers = 2;

  return genome;
}

std::vector<GeneNode>* Genome::getMutableNodes() noexcept {
  return &_geneNodesIO;
}

std::pair<GeneNode*, int> Genome::getMutableInputNodes() noexcept {
  return std::make_pair(_geneNodesIO.data(), _numInputs);
}

std::pair<GeneNode*, int> Genome::getMutableOutputNodes() noexcept {
  return std::make_pair(_geneNodesIO.data() + _numInputs, _numOutputs);
}

void Genome::feedForward() {
  std::sort(_geneNodesHidden.begin(),
            _geneNodesHidden.end(),
            [](const GeneNode& iNodeA, const GeneNode& iNodeB) {
              return iNodeA.getLayerID() < iNodeB.getLayerID();
            });

  std::vector<const GeneConnection*> incomeConnections;

  for (auto& hiddenNode : _geneNodesHidden) {
    engageNode(&hiddenNode, &incomeConnections);
  }

  auto outputNodes = getMutableOutputNodes();
  for (int i = 0; i < outputNodes.second; ++i) {
    engageNode(outputNodes.first + i, &incomeConnections);
  }
}

int Genome::getTotalNumNodes() const noexcept {
  return static_cast<int>(_geneNodesIO.size() + _geneNodesHidden.size());
}

int Genome::getNumHiddenNodes() const noexcept {
  return static_cast<int>(_geneNodesHidden.size());
}

int Genome::getNumConnections() const noexcept {
  return static_cast<int>(_geneConnections.size());
}

int Genome::getNumActiveConnections() const noexcept {
  return static_cast<int>(std::count_if(_geneConnections.cbegin(),
                                        _geneConnections.cend(),
                                        [](const GeneConnection& iConnection) {
                                          return iConnection.isEnabled();
                                        }));
}

void Genome::addConnection(const GeneNode& iNodeFrom,
                           const GeneNode& iNodeTo,
                           const float iWeight,
                           InnovationHistory* ioInnovationHistory) {
  assert(canBeLinked(iNodeFrom, iNodeTo));

  const NodeID nodeFromID = iNodeFrom.getNodeID();
  const NodeID nodeToID = iNodeTo.getNodeID();
  InnovationNum innovationNum;

  if (!ioInnovationHistory->matches(nodeFromID, nodeToID, &innovationNum)) {
    innovationNum = ioInnovationHistory->getNextInnovationNumAndIncrement();
    ioInnovationHistory->addInnovation(nodeFromID, nodeToID, innovationNum);
  }

  _geneConnections.emplace_back(nodeFromID, nodeToID, iWeight, innovationNum);
}

void Genome::addNode(GeneConnection* iConnection,
                     InnovationHistory* ioInnovationHistory) {
  assert(!isConnectionReferBias(*iConnection));

  const NodeID iPrevNodeID = iConnection->getNodeFromID();
  const NodeID iNextNodeID = iConnection->getNodeToID();
  const GeneNode& iPrevNode = *getGeneNodeByID(iPrevNodeID);
  const GeneNode& iNextNode = *getGeneNodeByID(iNextNodeID);
  const LayerID iLayerNewNode = iPrevNode.getLayerID() + 1;
  const float iOldWeight = iConnection->getWeight();

  iConnection->setEnabled(false);

  _geneNodesHidden.emplace_back(
      NodeType::HIDDEN, _nextNodeId++, iLayerNewNode, false);
  GeneNode* iNewNode = &_geneNodesHidden.back();

  addConnection(iPrevNode, *iNewNode, 1.f, ioInnovationHistory);
  addConnection(*iNewNode, iNextNode, iOldWeight, ioInnovationHistory);
  addConnection(getBiasNode(), *iNewNode, 0.f, ioInnovationHistory);
}

bool Genome::canBeLinked(const GeneNode& iNodeFrom,
                         const GeneNode& iNodeTo) const {
  if (iNodeFrom.getLayerID() < iNodeTo.getLayerID() &&
      !areAlreadyLinked(iNodeFrom, iNodeTo)) {
    return true;
  }

  return false;
}

bool Genome::areAlreadyLinked(const GeneNode& iNodeFrom,
                              const GeneNode& iNodeTo) const {
  const NodeID nodeFromID = iNodeFrom.getNodeID();
  const NodeID nodeToID = iNodeTo.getNodeID();

  for (const auto& connection : _geneConnections) {
    if (connection.getNodeFromID() == nodeFromID &&
        connection.getNodeToID() == nodeToID) {
      return true;
    }
  }

  return false;
}

bool Genome::isConnectionReferBias(const GeneConnection& iConnection) {
  return iConnection.getNodeFromID() == getBiasNode().getNodeID();
}

void Genome::computeIncomeConnections(
    const GeneNode& iNode,
    std::vector<const GeneConnection*>* oIncomeConnections) const {
  oIncomeConnections->clear();

  const NodeID iNodeID = iNode.getNodeID();
  for (const auto& connection : _geneConnections) {
    if (connection.getNodeToID() == iNodeID) {
      oIncomeConnections->push_back(&connection);
    }
  }
}

const GeneNode* Genome::getGeneNodeByID(const NodeID iNodeID) const {
  for (const auto& node : _geneNodesIO) {
    if (node.getNodeID() == iNodeID) {
      return &node;
    }
  }

  for (const auto& node : _geneNodesHidden) {
    if (node.getNodeID() == iNodeID) {
      return &node;
    }
  }

  return nullptr;
}

const GeneNode& Genome::getBiasNode() const noexcept {
  assert(!_geneNodesIO.empty());
  return _geneNodesIO.back();
}

std::vector<GeneConnection>* Genome::getMutableConnections() noexcept {
  return &_geneConnections;
}

Genome::Genome(const int numInputs, const int numOutputs)
    : _numInputs(numInputs), _numOutputs(numOutputs) {}

void Genome::engageNode(
    GeneNode* ioNode,
    std::vector<const GeneConnection*>* oIncomeConnections) {
  const NodeID nodeID = ioNode->getNodeID();
  float value = 0.f;

  computeIncomeConnections(*ioNode, oIncomeConnections);

  for (const auto connection : *oIncomeConnections) {
    if (connection->isEnabled()) {
      assert(connection->getNodeToID() == nodeID);
      const NodeID nodeBackID = connection->getNodeFromID();
      const GeneNode* nodeBack = getGeneNodeByID(nodeBackID);
      assert(nodeBack != nullptr);
      assert(nodeBack->getLayerID() < ioNode->getLayerID());

      value += connection->getWeight() * nodeBack->getValueWithActivation();
    }
  }

  ioNode->setValue(value);
}

}  // namespace aimaze2
