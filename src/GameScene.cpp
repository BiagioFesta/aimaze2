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
#include <cassert>

namespace aimaze2 {

void GameScene::init(const std::size_t iNumPlayers,
                     Config::RndEngine* iRndEngine) {
  _gameVelocity = kInitialGameVelocity;

  _ground.init(iRndEngine);

  _players.clear();
  for (std::size_t i = 0; i < iNumPlayers; ++i) {
    _players.emplace_back(PlayerStatus::RUNNING, Player{});
    _players.back().second.init();
  }
  _playerScores.resize(iNumPlayers, 0);
  _score.init();
  _obstacleManager.init();

  _sceneState = SceneState::RUNNING;
  _numPlayersDead = 0;
}

void GameScene::update(Config::RndEngine* iRndEngine) {
  if (_sceneState == SceneState::RUNNING) {
    updateGameVelocity();
    _score.update(_gameVelocity);

    for (auto& [status, player] : _players) {
      player.update(_gameVelocity);
    }

    _ground.update(_gameVelocity, iRndEngine);
    _obstacleManager.update(_gameVelocity, iRndEngine);

    for (std::size_t i = 0; i < _players.size(); ++i) {
      auto& player = _players[i].second;
      auto& status = _players[i].first;

      if (status == PlayerStatus::RUNNING &&
          _collisionManager.playerCollided(player,
                                           _obstacleManager.getObstacles())) {
        status = PlayerStatus::DEAD;

        _playerScores[i] = _score.getValue();

        auto deadPosition = kPositionPlayerDead;
        deadPosition.x += kOffsetDeadPosition * _numPlayersDead;
        player.die(kPositionPlayerDead + deadPosition);

        ++_numPlayersDead;
      }
    }

    // TODO(biagio): partition dead
  }
}

void GameScene::draw(sf::RenderWindow* oRender) const {
  _ground.draw(oRender);
  _obstacleManager.draw(oRender);
  for (const auto& [status, player] : _players) {
    player.draw(oRender);
  }
  _score.draw(oRender);
}

void GameScene::playerJump(const std::size_t iIndexPlayer) {
  assert(iIndexPlayer < _players.size());
  _players[iIndexPlayer].second.jump();
}

void GameScene::playerDuckOn(const std::size_t iIndexPlayer) {
  assert(iIndexPlayer < _players.size());
  _players[iIndexPlayer].second.duckOn();
}

void GameScene::playerDuckOff(const std::size_t iIndexPlayer) {
  assert(iIndexPlayer < _players.size());
  _players[iIndexPlayer].second.duckOff();
}

bool GameScene::arePlayersAllDead() const noexcept {
  return _numPlayersDead == _players.size();
}

void GameScene::updateGameVelocity() {
  constexpr float kTimeToIncrement = 0.2;
  constexpr float kDeltaIncrement = 1.f;
  static float accumulator = 0.f;

  if (kTimeToIncrement <= accumulator) {
    _gameVelocity += kDeltaIncrement;
    accumulator -= kTimeToIncrement;
  }

  accumulator += Config::kDeltaTimeLogicUpdate;
}

}  // namespace aimaze2
