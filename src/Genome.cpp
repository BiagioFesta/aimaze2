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
#include <utility>
#include <vector>

namespace aimaze2 {

Genome Genome::CreateSimpleGenome(const int numInputs, const int numOutputs) {
  Genome genome(numInputs, numOutputs);

  genome._geneNodes.reserve(numInputs + numOutputs + 1);

  // inputs
  for (int i = 0; i < numInputs; ++i) {
    genome._geneNodes.emplace_back(
        NodeType::INPUT, genome._nextNodeId++, kIDLayerInputs, false);
  }

  // output
  for (int i = 0; i < numOutputs; ++i) {
    genome._geneNodes.emplace_back(
        NodeType::OUTPUT, genome._nextNodeId++, kIDLayerOutpus, false);
  }

  // bias inputs
  genome._geneNodes.emplace_back(
      NodeType::INPUT, genome._nextNodeId++, kIDLayerInputs, true);

  genome._numLayers = 2;

  return genome;
}

std::vector<GeneNode>* Genome::getMutableNodes() noexcept {
  return &_geneNodes;
}

std::pair<GeneNode*, int> Genome::getMutableInputNodes() noexcept {
  return std::make_pair(_geneNodes.data(), _numInputs);
}

std::pair<GeneNode*, int> Genome::getMutableOutputNodes() noexcept {
  return std::make_pair(_geneNodes.data() + _numInputs, _numOutputs);
}

int Genome::getNumNodes() const noexcept {
  return static_cast<int>(_geneNodes.size());
}

Genome::Genome(const int numInputs, const int numOutputs)
    : _numInputs(numInputs), _numOutputs(numOutputs) {}

}  // namespace aimaze2
