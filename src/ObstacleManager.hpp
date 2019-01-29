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
#ifndef AIMAZE2__OBSTACLE_MANAGER__HPP
#define AIMAZE2__OBSTACLE_MANAGER__HPP
#include <deque>
#include "Obstacle.hpp"

namespace aimaze2 {

class ObstacleManager {
 public:
  using SeedType = Config::RndEngine::result_type;

  void init(const SeedType iSeed);
  void update(const float iGameVelocity);
  void draw(sf::RenderWindow* oRender) const;

  const std::deque<Obstacle>& getObstacles() const noexcept;

 private:
  Config::RndEngine _rndEngine;
  std::deque<Obstacle> _obstacles;

  void updateSpawn(const float iGameVelocity);
};

}  // namespace aimaze2

#endif  // AIMAZE2__OBSTACLE_MANAGER__HPP
