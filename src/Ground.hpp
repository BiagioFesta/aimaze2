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
#ifndef AIMAZE2__GROUND__HPP
#define AIMAZE2__GROUND__HPP
#include <SFML/Graphics.hpp>
#include <array>
#include "Config.hpp"

namespace aimaze2 {

class Ground {
 public:
  static constexpr int kNumOfRocks = 10;
  static constexpr float kHeightGround = 150.f;

  void init(Config::RndEngine* iRndEngine);

  void update(const float iGameVelocity, Config::RndEngine* iRndEngine);

  void draw(sf::RenderWindow* oRender) const;

 private:
  void distributeRocksPosition(Config::RndEngine* iRndEngine);

  sf::RectangleShape _lineSprite;
  std::vector<sf::RectangleShape> _rockSprites;
};

}  // namespace aimaze2

#endif  // AIMAZE2__GROUND__HPP
