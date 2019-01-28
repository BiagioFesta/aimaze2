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
#ifndef AIMAZE2__GENE_NODE__HPP
#define AIMAZE2__GENE_NODE__HPP

namespace aimaze2 {

class GeneNode {
 public:
  enum class NodeType { INPUT, HIDDEN, OUTPUT };
  using NodeID = int;
  using LayerID = int;

  GeneNode(const NodeType iNodeType,
           const NodeID iNodeID,
           const LayerID iLayerID,
           const bool isBias);

  NodeType getNodeType() const noexcept;
  NodeID getNodeID() const noexcept;
  LayerID getLayerID() const noexcept;
  void setLayerID(const LayerID iLayerID) noexcept;
  bool isBiasNode() const noexcept;
  float getPureValue() const noexcept;
  float getValueWithActivation() const noexcept;
  void setValue(const float iValue) noexcept;

 private:
  NodeType _nodeType;
  NodeID _nodeID;
  LayerID _layerID;
  bool _isBias;
  float _value;

  static float computeActivationValue(const float iValue);
};

}  // namespace aimaze2

#endif  // AIMAZE2__GENE_NODE__HPP
