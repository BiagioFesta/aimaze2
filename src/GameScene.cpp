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
  _ground.init(iRndEngine);
  _player.init();
  _obstacleManager.init();
}

void GameScene::update(Config::RndEngine* iRndEngine) {
  _ground.update(_gameVelocity, iRndEngine);
  _player.update(_gameVelocity);
  _obstacleManager.update(_gameVelocity, iRndEngine);
}

void GameScene::draw(sf::RenderWindow* oRender) const {
  _ground.draw(oRender);
  _player.draw(oRender);
  _obstacleManager.draw(oRender);
}

}  // namespace aimaze2
