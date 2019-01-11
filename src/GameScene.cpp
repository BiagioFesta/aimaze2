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
#include "GameScene.hpp"

namespace aimaze2 {

void GameScene::init(Config::RndEngine* iRndEngine) {
  _gameVelocity = kInitialGameVelocity;

  _ground.init(iRndEngine);
  _player.init();
  _obstacleManager.init();

  _sceneState = SceneState::RUNNING;
}

void GameScene::update(Config::RndEngine* iRndEngine) {
  if (_sceneState == SceneState::RUNNING) {
    _player.update(_gameVelocity);
    _ground.update(_gameVelocity, iRndEngine);
    _obstacleManager.update(_gameVelocity, iRndEngine);

    if (_collisionManager.playerCollided(_player,
                                         _obstacleManager.getObstacles())) {
      _sceneState = SceneState::DEAD;
    }
  } else if (_sceneState == SceneState::DEAD) {
    _player.die();
    _player.update(_gameVelocity);
  }
}

void GameScene::draw(sf::RenderWindow* oRender) const {
  _ground.draw(oRender);
  _player.draw(oRender);
  _obstacleManager.draw(oRender);
}

}  // namespace aimaze2
