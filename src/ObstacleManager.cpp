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
#include "ObstacleManager.hpp"
#include <cassert>
#include <random>

namespace {

aimaze2::Obstacle::ObstacleType GetRndObstacleType(
    aimaze2::Config::RndEngine* iRndEngine) {
  using aimaze2::Obstacle;

  static std::uniform_int_distribution<int> rndType(
      0, Obstacle::kNumTypeOfObstacles - 1);

  return static_cast<Obstacle::ObstacleType>(rndType(*iRndEngine));
}

}  // anonymous namespace

namespace aimaze2 {

void ObstacleManager::init() {
  Obstacle::initTextures();
  _obstacles.clear();
}

void ObstacleManager::update(const float iGameVelocity,
                             Config::RndEngine* iRndEngine) {
  updateSpawn(iGameVelocity, iRndEngine);

  for (auto& obstacle : _obstacles) {
    obstacle.update(iGameVelocity);
  }

  if (!_obstacles.empty() && _obstacles.front().isOutOfScreenOnLeft()) {
    _obstacles.pop_front();
  }
}

void ObstacleManager::draw(sf::RenderWindow* oRender) const {
  for (const auto& obstacle : _obstacles) {
    obstacle.draw(oRender);
  }
}

const std::deque<Obstacle>& ObstacleManager::getObstacles() const noexcept {
  return _obstacles;
}

void ObstacleManager::updateSpawn(const float iGameVelocity,
                                  Config::RndEngine* iRndEngine) {
  constexpr float kDeltaVariance = 1.0f;
  constexpr float kSpawnBaseTime = 3.f;
  constexpr float kScaleVelocity = 0.002f;

  static float accumulator = 0;
  static std::uniform_real_distribution<float> variance(-kDeltaVariance,
                                                        kDeltaVariance);

  const float timeToSpawn = (kSpawnBaseTime + variance(*iRndEngine)) /
                            (kScaleVelocity * iGameVelocity);

  if (timeToSpawn <= accumulator) {
    accumulator -= timeToSpawn;

    _obstacles.emplace_back();
    _obstacles.back().init(::GetRndObstacleType(iRndEngine));
  }
  accumulator += Config::kDeltaTimeLogicUpdate;
}

}  // namespace aimaze2
