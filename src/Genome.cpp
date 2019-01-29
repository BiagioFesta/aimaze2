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
#include <set>
#include <stack>
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

float RndWeight(aimaze2::ConfigEvolution::RndEngine* iRndEngine) {
  // TODO(biagio): check this
  return ::RndProbability(iRndEngine) * 4.f - 2.f;
}

float PerturbeWeight(aimaze2::ConfigEvolution::RndEngine* iRndEngine,
                     float iWeight) {
  static constexpr float kStep = 0.1f;

  // TODO(biagio): check this
  iWeight = iWeight + ::RndProbability(iRndEngine) * kStep * 2 - kStep;
  return iWeight;
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

  assert(genome.isValid());
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

std::vector<GeneNode>* Genome::getMutableHiddenNodes() noexcept {
  return &_geneNodesHidden;
}

const std::vector<GeneNode>& Genome::getIONodes() const noexcept {
  return _geneNodesIO;
}

const std::vector<GeneNode>& Genome::getHiddenNodes() const noexcept {
  return _geneNodesHidden;
}

int Genome::getNumInputs() const noexcept { return _numInputs; }

int Genome::getNumOutputs() const noexcept { return _numOutputs; }

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

int Genome::getNumLayers() const noexcept { return _numLayers; }

void Genome::mutate(ConfigEvolution::RndEngine* iRndEngine,
                    InnovationHistory* ioInnovationHistory) {
  if (getNumConnections() == 0) {
    addRndConnection(iRndEngine, ioInnovationHistory);
  }

  float probability;

  probability = ::RndProbability(iRndEngine);
  if (probability < ConfigEvolution::kProbabilityMutateWeights) {
    mutateAllWeights(iRndEngine);
  }

  probability = ::RndProbability(iRndEngine);
  if (probability < ConfigEvolution::kProbabilityNewNode) {
    addRndNode(iRndEngine, ioInnovationHistory);
  }

  probability = ::RndProbability(iRndEngine);
  if (probability < ConfigEvolution::kProbabilityNewConnection) {
    addRndConnection(iRndEngine, ioInnovationHistory);
  }
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
  assert(isValid());
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

const std::vector<GeneConnection>& Genome::getConnections() const noexcept {
  return _geneConnections;
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

  if (!areIONodesValidType()) {
    return false;
  }

  if (!areHiddenNodesValidType()) {
    return false;
  }

  if (!areConnectionsValid()) {
    return false;
  }

  if (!isNumLayersValid()) {
    return false;
  }

  if (areSameInnovationNumberInConnections()) {
    return false;
  }

  return true;
}

bool Genome::isSameSpecie(const Genome& iGenome) const {
  const float similarity = computeSimilaritySpecie(*this, iGenome);
  return similarity < ConfigEvolution::kThresholdSpeciate;
}

float Genome::computeSimilaritySpecie(const Genome& iGenomeA,
                                      const Genome& iGenomeB) {
  int numDisjoint = 0;
  int numExcess = 0;
  int numMatching = 0;
  float totalDifference = 0.f;

  auto itConA = iGenomeA._geneConnections.cbegin();
  auto itConB = iGenomeB._geneConnections.cbegin();
  const auto itConAEnd = iGenomeA._geneConnections.cend();
  const auto itConBEnd = iGenomeB._geneConnections.cend();

  while (itConA != itConAEnd || itConB != itConBEnd) {
    if (itConA == itConAEnd) {
      // A is end
      ++numExcess;
      ++itConB;
    } else if (itConB == itConBEnd) {
      // B is end
      ++numExcess;
      ++itConA;
    } else {
      if (itConA->getInnovationNum() == itConB->getInnovationNum()) {
        // same innovation number
        ++numMatching;
        ++itConA;
        ++itConB;
        totalDifference += std::abs(itConA->getWeight() - itConB->getWeight());
      } else if (itConA->getInnovationNum() < itConB->getInnovationNum()) {
        // A is smaller
        ++numDisjoint;
        ++itConA;
      } else {
        // B is smaller
        ++numDisjoint;
        ++itConB;
      }
    }
  }  // until all connection have been evaluated

  float diffWeights = totalDifference;
  if (numMatching) {
    diffWeights /= static_cast<float>(numMatching);
  }

  const auto largeGenome =
      std::max(iGenomeA.getNumConnections(), iGenomeB.getNumConnections());
  const float largeGenomeNormalized =
      largeGenome < ConfigEvolution::kNormalizeSizeGene
          ? 1.f
          : static_cast<float>(largeGenome);

  const float t1 = ConfigEvolution::kExcessCoefficient *
                   static_cast<float>(numExcess) / largeGenomeNormalized;
  const float t2 = ConfigEvolution::kDisjointCoefficient *
                   static_cast<float>(numDisjoint) / largeGenomeNormalized;
  const float t3 = ConfigEvolution::kWeightsCoefficient * diffWeights;

  return t1 + t2 + t3;
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

  shiftNodesToUpperLayer(getMutableGeneNodeByID(nextNodeID));
  updateNumLayers();

  addConnection(prevNodeID, newNodeID, 1.f, ioInnovationHistory);
  addConnection(newNodeID, nextNodeID, oldWeight, ioInnovationHistory);

  if (iAddBias) {
    addConnection(
        getBiasNode().getNodeID(), newNodeID, 0.f, ioInnovationHistory);
  }

  assert(isValid());
  return newNodeID;
}

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

GeneNode* Genome::getMutableGeneNodeByID(const NodeID iNodeID) {
  for (auto& node : _geneNodesIO) {
    if (node.getNodeID() == iNodeID) {
      return &node;
    }
  }

  for (auto& node : _geneNodesHidden) {
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

void Genome::computeOutgoingconnections(
    const GeneNode& iNode,
    std::vector<const GeneConnection*>* oOutgoingConnections) const {
  oOutgoingConnections->clear();

  const NodeID iNodeID = iNode.getNodeID();
  for (const auto& connection : _geneConnections) {
    if (connection.getNodeFromID() == iNodeID) {
      oOutgoingConnections->push_back(&connection);
    }
  }
}

bool Genome::areIONodesValidType() const {
  for (const auto& node : _geneNodesIO) {
    if (node.getNodeType() != GeneNode::NodeType::INPUT &&
        node.getNodeType() != GeneNode::NodeType::OUTPUT) {
      return false;
    }
  }
  return true;
}

bool Genome::areHiddenNodesValidType() const {
  for (const auto& node : _geneNodesHidden) {
    if (node.getNodeType() != GeneNode::NodeType::HIDDEN) {
      return false;
    }
  }
  return true;
}

bool Genome::areConnectionsValid() const {
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

bool Genome::isNumLayersValid() const {
  auto itMax =
      std::max_element(_geneNodesHidden.cbegin(),
                       _geneNodesHidden.cend(),
                       [](const GeneNode& iNodeA, const GeneNode& iNodeB) {
                         return iNodeA.getLayerID() < iNodeB.getLayerID();
                       });
  const int expectedNumLayers =
      2 + (itMax != _geneNodesHidden.cend() ? itMax->getLayerID() : 0);
  return _numLayers == expectedNumLayers;
}

bool Genome::areSameInnovationNumberInConnections() const {
  std::set<InnovationNum> visitedInnovationNums;
  for (const auto& connection : _geneConnections) {
    visitedInnovationNums.insert(connection.getInnovationNum());
  }

  return visitedInnovationNums.size() != _geneConnections.size();
}

bool Genome::addRndConnection(ConfigEvolution::RndEngine* iRndEngine,
                              InnovationHistory* ioInnovationHistory) {
  auto candidateNodes = computeAllNodeIDs();
  std::shuffle(candidateNodes.begin(), candidateNodes.end(), *iRndEngine);

  for (const NodeID nodeFrom : candidateNodes) {
    auto candidateLinks = computeForwardNodes(nodeFrom);
    std::shuffle(candidateLinks.begin(), candidateLinks.end(), *iRndEngine);

    for (const NodeID nodeTo : candidateLinks) {
      if (canBeLinked(nodeFrom, nodeTo)) {
        addConnection(
            nodeFrom, nodeTo, ::RndWeight(iRndEngine), ioInnovationHistory);
        return true;
      }
    }
  }

  return false;
}

bool Genome::addRndNode(ConfigEvolution::RndEngine* iRndEngine,
                        InnovationHistory* ioInnovationHistory) {
  GeneConnection* connection = getRndConnection(iRndEngine);
  if (connection && !isConnectionReferBias(*connection)) {
    addNode(connection, ioInnovationHistory, true);
    return true;
  }
  return false;
}

void Genome::mutateAllWeights(ConfigEvolution::RndEngine* iRndEngine) {
  for (auto& connection : _geneConnections) {
    const float probability = ::RndProbability(iRndEngine);

    if (probability < ConfigEvolution::kProbabilityResetWeight) {
      connection.setWeight(::RndWeight(iRndEngine));
    } else {
      const float newWeight =
          ::PerturbeWeight(iRndEngine, connection.getWeight());
      connection.setWeight(newWeight);
    }
  }
}

Genome::NodeID Genome::getRndNodeID(
    ConfigEvolution::RndEngine* iRndEngine) const {
  assert(getTotalNumNodes() > 0);
  std::uniform_int_distribution<std::size_t> rndIndex(0,
                                                      getTotalNumNodes() - 1);
  const std::size_t index = rndIndex(*iRndEngine);
  if (index < _geneNodesIO.size()) {
    return _geneNodesIO[index].getNodeID();
  }

  const std::size_t indexHidden = index - _geneNodesIO.size();

  assert(indexHidden < _geneNodesHidden.size());
  return _geneNodesHidden[indexHidden].getNodeID();
}

GeneConnection* Genome::getRndConnection(
    ConfigEvolution::RndEngine* iRndEngine) {
  if (_geneConnections.empty()) {
    return nullptr;
  }

  std::uniform_int_distribution<std::size_t> rndIndex(
      0, _geneConnections.size() - 1);
  return &(_geneConnections[rndIndex(*iRndEngine)]);
}

void Genome::shiftNodesToUpperLayer(GeneNode* iNode) {
  assert(iNode);

  std::stack<GeneNode*> openList;
  openList.push(iNode);

  std::vector<const GeneConnection*> outConnections;
  LayerID maxLayer = kIDLayerInputs;

  while (!openList.empty()) {
    GeneNode* const current = openList.top();
    openList.pop();
    if (current->getNodeType() == GeneNode::NodeType::HIDDEN) {
      maxLayer = std::max(maxLayer, current->getLayerID());
      current->setLayerID(current->getLayerID() + 1);
    }

    computeOutgoingconnections(*current, &outConnections);
    for (const auto& connection : outConnections) {
      openList.push(getMutableGeneNodeByID(connection->getNodeToID()));
    }
  }
}

void Genome::updateNumLayers() {
  auto itMax =
      std::max_element(_geneNodesHidden.cbegin(),
                       _geneNodesHidden.cend(),
                       [](const GeneNode& iNodeA, const GeneNode& iNodeB) {
                         return iNodeA.getLayerID() < iNodeB.getLayerID();
                       });
  _numLayers = 2 + (itMax != _geneNodesHidden.cend() ? itMax->getLayerID() : 0);
}

std::vector<Genome::NodeID> Genome::computeAllNodeIDs() const {
  std::vector<Genome::NodeID> allNodes;
  allNodes.reserve(getTotalNumNodes());

  for (const auto& nodeIO : _geneNodesIO) {
    allNodes.emplace_back(nodeIO.getNodeID());
  }

  for (const auto& nodeHidden : _geneNodesHidden) {
    allNodes.emplace_back(nodeHidden.getNodeID());
  }

  return allNodes;
}

std::vector<Genome::NodeID> Genome::computeForwardNodes(
    const NodeID iNodeFromID) const {
  std::vector<Genome::NodeID> forwardNodes;
  // TODO(biagio): pessimistic reserve?

  const GeneNode* node = getGeneNodeByID(iNodeFromID);
  const auto fromLayer = node->getLayerID();

  for (const auto& nodeIO : _geneNodesIO) {
    if (fromLayer < nodeIO.getLayerID()) {
      forwardNodes.emplace_back(nodeIO.getNodeID());
    }
  }

  for (const auto& nodeHidden : _geneNodesHidden) {
    if (fromLayer < nodeHidden.getLayerID()) {
      forwardNodes.emplace_back(nodeHidden.getNodeID());
    }
  }

  return forwardNodes;
}

}  // namespace aimaze2
