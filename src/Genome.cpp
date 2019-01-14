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
#include <cmath>
#include <limits>
#include <random>
#include <utility>
#include <vector>

namespace {

float RndProbability(aimaze2::ConfigEvolution::RndEngine* iRndEngine) {
  static constexpr float kMaxFloat = std::numeric_limits<float>::max();

  // TODO(biagio): to check
  std::uniform_real_distribution<float> rnd(0, std::nextafter(1.0, kMaxFloat));
  return rnd(*iRndEngine);
}

bool LaunchCoin(aimaze2::ConfigEvolution::RndEngine* iRndEngine) {
  std::uniform_int_distribution<> coin(0, 1);
  return coin(*iRndEngine);
}

}  // namespace

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

Genome Genome::Crossover(Genome* iGenomeA,
                         Genome* iGenomeB,
                         ConfigEvolution::RndEngine* iRndEngine) {
  assert(iGenomeA->_numInputs == iGenomeB->_numInputs);
  assert(iGenomeA->_numOutputs == iGenomeB->_numOutputs);
  assert(iGenomeB->_geneNodesHidden.size() <=
         iGenomeA->_geneNodesHidden.size());

  // TODO(biagio): assert iGenomeA has more fit

  /* Copy the structure from GenomeA.
     Indeed, in case of disjoint or excess the GenomeA will be picked anyway.
   */
  Genome child = CopyNodeStructureFrom(*iGenomeA);

  iGenomeA->sortConnectionsByInnovationNum();
  iGenomeB->sortConnectionsByInnovationNum();

  auto itConA = iGenomeA->_geneConnections.cbegin();
  auto itConB = iGenomeB->_geneConnections.cbegin();
  const auto itConAEnd = iGenomeA->_geneConnections.cend();
  const auto itConBEnd = iGenomeB->_geneConnections.cend();
  decltype(itConA) chosenCon;
  bool toDisable;
  bool toSkip;

  while (itConA != itConAEnd) {
    toDisable = false;
    toSkip = false;

    if (itConB == itConBEnd) {
      chosenCon = itConA;
      ++itConA;
    } else {
      if (itConA->getInnovationNum() == itConB->getInnovationNum()) {
        chosenCon = ::LaunchCoin(iRndEngine) ? itConA : itConB;

        if (!itConA->isEnabled() || !itConB->isEnabled()) {
          if (::RndProbability(iRndEngine) < 0.75f) {
            toDisable = true;
          }
        }

        ++itConA;
        ++itConB;
      } else {
        chosenCon = itConA;
        if (itConA->getInnovationNum() < itConB->getInnovationNum()) {
          ++itConA;
        } else {
          toSkip = true;
          ++itConB;
        }
      }
    }

    if (!toSkip) {
      child._geneConnections.push_back(*chosenCon);
      if (toDisable) {
        child._geneConnections.back().setEnabled(false);
      }
    }
  }  // until all connection have been visited

  assert(child.isValid());
  return child;
}

std::pair<GeneNode*, int> Genome::getMutableInputNodes() noexcept {
  return std::make_pair(_geneNodesIO.data(), _numInputs);
}

std::pair<GeneNode*, int> Genome::getMutableOutputNodes() noexcept {
  return std::make_pair(_geneNodesIO.data() + _numInputs, _numOutputs);
}

std::vector<GeneNode>* Genome::getMutableIONodes() noexcept {
  return &_geneNodesIO;
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

void Genome::addConnection(const NodeID iNodeFromID,
                           const NodeID iNodeToID,
                           const float iWeight,
                           InnovationHistory* ioInnovationHistory) {
  assert(canBeLinked(iNodeFromID, iNodeToID));

  InnovationNum innovationNum;

  if (!ioInnovationHistory->matches(iNodeFromID, iNodeToID, &innovationNum)) {
    innovationNum = ioInnovationHistory->getNextInnovationNumAndIncrement();
    ioInnovationHistory->addInnovation(iNodeFromID, iNodeToID, innovationNum);
  }

  _geneConnections.emplace_back(iNodeFromID, iNodeToID, iWeight, innovationNum);
}

Genome::NodeID Genome::addNode(GeneConnection* iConnection,
                               InnovationHistory* ioInnovationHistory,
                               const bool iAddBias) {
  assert(!isConnectionReferBias(*iConnection));

  const NodeID prevNodeID = iConnection->getNodeFromID();
  const NodeID nextNodeID = iConnection->getNodeToID();
  const LayerID layerNewNode = getGeneNodeByID(prevNodeID)->getLayerID() + 1;
  const float oldWeight = iConnection->getWeight();

  iConnection->setEnabled(false);

  _geneNodesHidden.emplace_back(
      NodeType::HIDDEN, _nextNodeId++, layerNewNode, false);
  const NodeID newNodeID = _geneNodesHidden.back().getNodeID();

  addConnection(prevNodeID, newNodeID, 1.f, ioInnovationHistory);
  addConnection(newNodeID, nextNodeID, oldWeight, ioInnovationHistory);

  if (iAddBias) {
    addConnection(
        getBiasNode().getNodeID(), newNodeID, 0.f, ioInnovationHistory);
  }

  return newNodeID;
}

Genome::NodeID Genome::addNode(const NodeID iNodeFromID,
                               const NodeID iNodeToID,
                               InnovationHistory* ioInnovationHistory,
                               const bool iAddBias) {
  return addNode(getConnectionAmongNodes(iNodeFromID, iNodeToID),
                 ioInnovationHistory,
                 iAddBias);
}

bool Genome::canBeLinked(const NodeID iNodeFromID,
                         const NodeID iNodeToID) const {
  const GeneNode* const nodeFrom = getGeneNodeByID(iNodeFromID);
  const GeneNode* const nodeTo = getGeneNodeByID(iNodeToID);

  if (!nodeFrom || !nodeTo) {
    return false;
  }

  if (nodeFrom->getLayerID() < nodeTo->getLayerID() &&
      !areAlreadyLinked(iNodeFromID, iNodeToID)) {
    return true;
  }

  return false;
}

bool Genome::areAlreadyLinked(const NodeID iNodeFromID,
                              const NodeID iNodeToID) const {
  for (const auto& connection : _geneConnections) {
    if (connection.getNodeFromID() == iNodeFromID &&
        connection.getNodeToID() == iNodeToID) {
      return true;
    }
  }

  return false;
}

bool Genome::isConnectionReferBias(const GeneConnection& iConnection) {
  return iConnection.getNodeFromID() == getBiasNode().getNodeID();
}

const GeneNode& Genome::getBiasNode() const noexcept {
  assert(!_geneNodesIO.empty());
  return _geneNodesIO.back();
}

std::vector<GeneConnection>* Genome::getMutableConnections() noexcept {
  return &_geneConnections;
}

void Genome::sortConnectionsByInnovationNum() {
  std::sort(_geneConnections.begin(),
            _geneConnections.end(),
            [](const GeneConnection& iConnectionA,
               const GeneConnection& iConnectionB) {
              return iConnectionA.getInnovationNum() <
                     iConnectionB.getInnovationNum();
            });
}

bool Genome::isValid() const {
  const std::size_t expectedIO = static_cast<std::size_t>(_numInputs) +
                                 static_cast<std::size_t>(_numOutputs) +
                                 static_cast<std::size_t>(1);

  if (_geneNodesIO.size() != expectedIO) {
    return false;
  }

  for (const auto& node : _geneNodesIO) {
    if (node.getNodeType() != GeneNode::NodeType::INPUT &&
        node.getNodeType() != GeneNode::NodeType::OUTPUT) {
      return false;
    }
    if (node.getLayerID() != kIDLayerInputs &&
        node.getLayerID() != kIDLayerOutpus) {
      return false;
    }
  }

  for (const auto& node : _geneNodesHidden) {
    if (node.getNodeType() != GeneNode::NodeType::HIDDEN) {
      return false;
    }
  }

  for (const auto& connection : _geneConnections) {
    const NodeID nodeFromID = connection.getNodeFromID();
    const NodeID nodeToID = connection.getNodeToID();
    const GeneNode* nodeFrom = getGeneNodeByID(nodeFromID);
    const GeneNode* nodeTo = getGeneNodeByID(nodeToID);
    if (!nodeFrom || !nodeTo) {
      return false;
    }
    assert(areAlreadyLinked(nodeFromID, nodeToID));

    if (nodeTo->getLayerID() <= nodeFrom->getLayerID()) {
      return false;
    }
  }

  return true;
}

Genome Genome::CopyNodeStructureFrom(const Genome& iGenome) {
  Genome copy;
  copy._nextNodeId = iGenome._nextNodeId;
  copy._numLayers = iGenome._numLayers;
  copy._numInputs = iGenome._numInputs;
  copy._numOutputs = iGenome._numOutputs;
  copy._geneNodesIO = iGenome._geneNodesIO;
  copy._geneNodesHidden = iGenome._geneNodesHidden;
  copy._geneConnections.reserve(iGenome._geneConnections.size());

  return copy;
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

GeneConnection* Genome::getConnectionAmongNodes(const NodeID iNodeFromID,
                                                const NodeID iNodeToID) {
  // TODO(biagio): warning! vector can grows and invalidate reference
  for (auto& connection : _geneConnections) {
    if (connection.getNodeFromID() == iNodeFromID &&
        connection.getNodeToID() == iNodeToID) {
      return &connection;
    }
  }

  return nullptr;
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

}  // namespace aimaze2
