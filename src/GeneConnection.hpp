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
#ifndef AIMAZE2__GENE_CONNECTION__HPP
#define AIMAZE2__GENE_CONNECTION__HPP
#include "GeneNode.hpp"

namespace aimaze2 {

class GeneConnection {
 public:
  using NodeID = GeneNode::NodeID;
  using InnovationNum = int;

 private:
  NodeID _nodeFrom;
  NodeID _nodeTo;
  float _weight;
  bool _enabled;
  InnovationNum _innovationNum;
};

}  // namespace aimaze2

#endif  // AIMAZE2__GENE_CONNECTION__HPP
