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
#ifndef AIMAZE2__GENOME_DRAWNER__HPP
#define AIMAZE2__GENOME_DRAWNER__HPP
#include <SFML/Graphics.hpp>
#include <array>
#include <map>
#include <vector>
#include "Genome.hpp"

namespace aimaze2 {

class GenomeDrawner {
 public:
  void updateWithGenome(const Genome& iGenome);
  void draw(sf::RenderWindow* oRender) const;

 private:
  static constexpr float kRadiusNode = 5.f;
  using NodeID = Genome::NodeID;
  using LayerID = Genome::LayerID;
  using LineShape = std::array<sf::Vertex, 2>;

  std::map<NodeID, sf::CircleShape> _nodeSprites;
  std::vector<LineShape> _connectionSprites;

  void updateNodeSprites(const Genome& iGenome);
  void updateConnectionSprites(const Genome& iGenome);
  void drawConnections(sf::RenderWindow* oRender) const;
  void drawNodes(sf::RenderWindow* oRender) const;
};

}  // namespace aimaze2

#endif  // AIMAZE2__GENOME_DRAWNER__HPP
