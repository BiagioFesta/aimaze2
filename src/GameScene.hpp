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
#ifndef AIMAZE2__GAME_SCENE__HPP
#define AIMAZE2__GAME_SCENE__HPP
#include <SFML/Graphics.hpp>
#include "CollisionManager.hpp"
#include "Ground.hpp"
#include "ObstacleManager.hpp"
#include "Player.hpp"

namespace aimaze2 {

class GameScene {
 public:
  enum class SceneState { RUNNING, DEAD };

  void init(Config::RndEngine* iRndEngine);
  void update(Config::RndEngine* iRndEngine);
  void draw(sf::RenderWindow* oRender) const;

 private:
  static constexpr float kInitialGameVelocity = 400.f;

  float _gameVelocity;
  Ground _ground;
  Player _player;
  ObstacleManager _obstacleManager;
  CollisionManager _collisionManager;
  SceneState _sceneState;
};

}  // namespace aimaze2

#endif  // AIMAZE2__GAME_SCENE__HPP
