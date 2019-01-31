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
#include "InnovationHistory.hpp"
#include <cassert>

namespace aimaze2 {

InnovationHistory::InnovationHistory(const InnovationNum iInnovationNum)
    : _nextInnovationNum(iInnovationNum) {}

bool InnovationHistory::matches(const NodeID iNodeFrom,
                                const NodeID iNodeTo,
                                InnovationNum* oInnovationNum) const {
  for (const auto& connection : _connectionsHistory) {
    if (connection._nodeFrom == iNodeFrom && connection._nodeTo == iNodeTo) {
      if (oInnovationNum != nullptr) {
        *oInnovationNum = connection._innovationNum;
      }
      return true;
    }
  }

  return false;
}

void InnovationHistory::addInnovation(const NodeID iNodeFrom,
                                      const NodeID iNodeTo,
                                      const InnovationNum iInnovationNum) {
  assert(!matches(iNodeFrom, iNodeTo, nullptr));
  _connectionsHistory.emplace_back(iNodeFrom, iNodeTo, iInnovationNum);
}

InnovationHistory::InnovationNum
InnovationHistory::getNextInnovationNumAndIncrement() noexcept {
  return _nextInnovationNum++;
}

void InnovationHistory::flush() noexcept { return _connectionsHistory.clear(); }

InnovationHistory::ConnectionHistory::ConnectionHistory(
    const NodeID iNodeFrom,
    const NodeID iNodeTo,
    const InnovationNum iInnovationNum)
    : _nodeFrom(iNodeFrom), _nodeTo(iNodeTo), _innovationNum(iInnovationNum) {}

}  // namespace aimaze2
