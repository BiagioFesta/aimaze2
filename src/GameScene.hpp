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
#include <utility>
#include <vector>
#include "CollisionManager.hpp"
#include "GenomeDrawner.hpp"
#include "Ground.hpp"
#include "ObstacleManager.hpp"
#include "Player.hpp"
#include "Score.hpp"

namespace aimaze2 {

class GameScene {
 public:
  enum class SceneState { RUNNING, STOP };
  enum class PlayerStatus { RUNNING, DEAD };
  static inline const sf::Vector2f kPositionPlayerDead{0.f, 0.f};

  struct ObstacleProperty {
    float _distance;
    float _height;
    float _width;
    float _altitude;

    ObstacleProperty() = default;
    ObstacleProperty(const float iDistance,
                     const float iHeight,
                     const float iAltitude) noexcept;
  };

  void init(const std::size_t iNumPlayers, Config::RndEngine* iRndEngine);
  void update(Config::RndEngine* iRndEngine);
  void draw(sf::RenderWindow* oRender) const;

  void playerJump(const std::size_t iIndexPlayer);
  void playerDuckOn(const std::size_t iIndexPlayer);
  void playerDuckOff(const std::size_t iIndexPlayer);

  bool arePlayersAllDead() const noexcept;

  void updateGenomeToDraw(const Genome& iGenome);

  const ObstacleProperty& getNextObstacleProperty() const noexcept;
  float getGameVelocity() const noexcept;

 private:
  static constexpr float kInitialGameVelocity = 400.f;
  static constexpr float kOffsetDeadPosition = 10.f;

  void updateGameVelocity() noexcept;

  void computePropertyNextObstacle() noexcept;

  float _gameVelocity;
  Ground _ground;
  std::vector<std::pair<PlayerStatus, Player>> _players;
  std::vector<float> _playerScores;
  Score _score;
  ObstacleManager _obstacleManager;
  CollisionManager _collisionManager;
  SceneState _sceneState;
  std::size_t _numPlayersDead;
  ObstacleProperty _obstacleProperty;
  GenomeDrawner _genomeDrawner;
};

}  // namespace aimaze2

#endif  // AIMAZE2__GAME_SCENE__HPP
