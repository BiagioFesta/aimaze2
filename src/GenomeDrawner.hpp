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
#include "Genome.hpp"

namespace aimaze2 {

class GenomeDrawner {
 public:
  void init();
  void updateWithGenome(const Genome& iGenome);
  void draw(sf::RenderWindow* oRender) const;

 private:
  static constexpr float kPixelSizeNode = 10.f;
  using NodeID = Genome::NodeID;
  using LineSprite = std::array<sf::Vertex, 2>;

  struct NodeSprite {
    NodeID _id;
    sf::CircleShape _sprite;
    sf::Text _label;

    NodeSprite(const sf::Vector2f& iPosition, const NodeID iNodeID);
  };

  sf::View _view;
  std::vector<NodeSprite> _nodeSprites;
  std::vector<LineSprite> _connectionSprites;

  void buildFromGenome(const Genome& iGenome);

  const NodeSprite& getNodeSpriteFromID(const NodeID iNodeID) const;
};

}  // namespace aimaze2

#endif  // AIMAZE2__GENOME_DRAWNER__HPP
