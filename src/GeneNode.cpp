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
#include "GeneNode.hpp"
#include <cmath>

namespace aimaze2 {

GeneNode::GeneNode(const NodeType iNodeType,
                   const NodeID iNodeID,
                   const LayerID iLayerID,
                   const bool isBias)
    : _nodeType(iNodeType),
      _nodeID(iNodeID),
      _layerID(iLayerID),
      _isBias(isBias),
      _value(_isBias ? 1.f : 0.f) {}

GeneNode::NodeType GeneNode::getNodeType() const noexcept { return _nodeType; }

GeneNode::NodeID GeneNode::getNodeID() const noexcept { return _nodeID; }

GeneNode::LayerID GeneNode::getLayerID() const noexcept { return _layerID; }

bool GeneNode::isBiasNode() const noexcept { return _isBias; }

float GeneNode::getPureValue() const noexcept { return _value; }

float GeneNode::getValueWithActivation() const noexcept {
  if (_nodeType == NodeType::INPUT || _isBias) {
    return _value;
  }

  return computeActivationValue(_value);
}

void GeneNode::setValue(const float iValue) noexcept { _value = iValue; }

float GeneNode::computeActivationValue(const float iValue) {
  constexpr float kCoefficient = -4.9f;
  return 1.f / (1.f + expf(kCoefficient * iValue));
}

}  // namespace aimaze2
