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
#ifndef AIMAZE2__INNOVATION_HISTORY__HPP
#define AIMAZE2__INNOVATION_HISTORY__HPP
#include <vector>
#include "GeneConnection.hpp"

namespace aimaze2 {

class InnovationHistory {
 public:
  using NodeID = GeneNode::NodeID;
  using InnovationNum = GeneConnection::InnovationNum;

  explicit InnovationHistory(const InnovationNum iInnovationNum);

  bool matches(const NodeID iNodeFrom,
               const NodeID iNodeTo,
               InnovationNum* oInnovationNum) const;

  void addInnovation(const NodeID iNodeFrom,
                     const NodeID iNodeTo,
                     const InnovationNum iInnovationNum);

  InnovationNum getNextInnovationNumAndIncrement() noexcept;

  void flush() noexcept;

 private:
  struct ConnectionHistory {
    NodeID _nodeFrom;
    NodeID _nodeTo;
    InnovationNum _innovationNum;

    ConnectionHistory(const NodeID iNodeFrom,
                      const NodeID iNodeTo,
                      const InnovationNum iInnovationNum);
  };

  std::vector<ConnectionHistory> _connectionsHistory;
  InnovationNum _nextInnovationNum;
};

}  // namespace aimaze2

#endif  // AIMAZE2__INNOVATION_HISTORY__HPP
