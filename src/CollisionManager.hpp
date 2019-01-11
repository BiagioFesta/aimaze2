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
#ifndef AIMAZE2__COLLISION_MANAGER__HPP
#define AIMAZE2__COLLISION_MANAGER__HPP
#include "Obstacle.hpp"
#include "Player.hpp"

namespace aimaze2 {

class CollisionManager {
 public:
  template <typename ObstacleContainer>
  static bool playerCollided(const Player& iPlayer,
                             const ObstacleContainer& iObstacleContainer);
};

template <typename ObstacleContainer>
bool CollisionManager::playerCollided(
    const Player& iPlayer,
    const ObstacleContainer& iObstacleContainer) {
  const auto playerBox = iPlayer.getCollisionBox();

  for (const auto& obstacle : iObstacleContainer) {
    const auto obstacleBox = obstacle.getCollisionBox();
    if (playerBox.intersects(obstacleBox)) {
      return true;
    }
  }

  return false;
}

}  // namespace aimaze2

#endif  // AIMAZE2__COLLISION_MANAGER__HPP
