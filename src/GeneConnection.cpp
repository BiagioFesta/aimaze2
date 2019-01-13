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
#include "GeneConnection.hpp"

namespace aimaze2 {

GeneConnection::GeneConnection(const NodeID iNodeFrom,
                               const NodeID iNodeTo,
                               const float iWeight,
                               const InnovationNum iInnovationNum)
    : _nodeFrom(iNodeFrom),
      _nodeTo(iNodeTo),
      _weight(iWeight),
      _innovationNum(iInnovationNum) {}

GeneConnection::NodeID GeneConnection::getNodeFromID() const noexcept {
  return _nodeFrom;
}

GeneConnection::NodeID GeneConnection::getNodeToID() const noexcept {
  return _nodeTo;
}

float GeneConnection::getWeight() const noexcept { return _weight; }

GeneConnection::InnovationNum GeneConnection::getInnovationNum() const
    noexcept {
  return _innovationNum;
}

bool GeneConnection::isEnabled() const noexcept { return _enabled; }

void GeneConnection::setEnabled(const bool iEnabled) noexcept {
  _enabled = iEnabled;
}

}  // namespace aimaze2
