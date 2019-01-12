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
#include "Obstacle.hpp"
#include <cassert>
#include <random>

namespace {

sf::Vector2f GetInitialPosition(
    const aimaze2::Obstacle::ObstacleType iObstacleType) {
  using aimaze2::Config;
  using aimaze2::Obstacle;

  switch (iObstacleType) {
    case Obstacle::ObstacleType::CACTUS_SMALL:
      return sf::Vector2f{Config::kWindowWidth, 390.f};
    case Obstacle::ObstacleType::CACTUS_BIG:
      return sf::Vector2f{Config::kWindowWidth, 350.f};
    case Obstacle::ObstacleType::CACTUS_LARGE:
      return sf::Vector2f{Config::kWindowWidth, 390.f};
    case Obstacle::ObstacleType::BIRD_LOW:
      return sf::Vector2f{Config::kWindowWidth, 390.f};
    case Obstacle::ObstacleType::BIRD_MEDIUM:
      return sf::Vector2f{Config::kWindowWidth, 335.f};
    case Obstacle::ObstacleType::BIRD_HIGH:
      return sf::Vector2f{Config::kWindowWidth, 250.f};
  }
}

}  // namespace

namespace aimaze2 {

void Obstacle::initTextures() {
  std::get<TextureID::CACTUS_SMALL>(kTextures).loadFromFile(
      "data/cactusSmall0000.png");
  std::get<TextureID::CACTUS_BIG>(kTextures).loadFromFile(
      "data/cactusBig0000.png");
  std::get<TextureID::CACTUS_LARGE>(kTextures).loadFromFile(
      "data/cactusSmallMany0000.png");
  std::get<TextureID::BIRD_0>(kTextures).loadFromFile("data/berd.png");
  std::get<TextureID::BIRD_1>(kTextures).loadFromFile("data/berd2.png");
}

void Obstacle::init(const ObstacleType iObstacleType) {
  const auto textureID = GetFirstTexture(iObstacleType);

  assert(textureID < kTextures.size());
  _sprite.setTexture(kTextures[textureID]);
  _sprite.setPosition(::GetInitialPosition(iObstacleType));

  _textureID = textureID;
}

void Obstacle::update(const float iGameVelocity) {
  updateAnimation();

  const float kDeltaMovement = iGameVelocity * Config::kDeltaTimeLogicUpdate;
  _sprite.move({-kDeltaMovement, 0.f});
}

void Obstacle::draw(sf::RenderWindow* oRender) const {
  oRender->draw(_sprite);

  if constexpr (Config::kDrawCollisionBox) {
    drawCollisionBox(oRender);
  }
}

bool Obstacle::isOutOfScreenOnLeft() const {
  return _sprite.getPosition().x + _sprite.getGlobalBounds().width < 0.f;
}

sf::FloatRect Obstacle::getCollisionBox() const {
  auto box = _sprite.getGlobalBounds();
  if (_textureID == TextureID::BIRD_0 || _textureID == TextureID::BIRD_1) {
    box.width -= 4.f;
    box.left += 2.f;
    box.height -= 40.f;
    box.top += 20.f;
  }
  return box;
}

void Obstacle::drawCollisionBox(sf::RenderWindow* oRender) const {
  static sf::RectangleShape box;

  const auto collisionBox = getCollisionBox();
  box.setSize(sf::Vector2f{collisionBox.width, collisionBox.height});
  box.setOutlineColor(Config::kDebugColor);
  box.setOutlineThickness(2.f);
  box.setFillColor(sf::Color{0, 0, 0, 0});
  box.setPosition(sf::Vector2f{collisionBox.left, collisionBox.top});

  oRender->draw(box);
}

void Obstacle::updateAnimation() {
  static constexpr float kTimePerFrame = 0.3f;
  static float accumulator = 0.f;

  if (_textureID == TextureID::BIRD_0 || _textureID == TextureID::BIRD_1) {
    if (kTimePerFrame <= accumulator) {
      _textureID = _textureID == TextureID::BIRD_0 ? TextureID::BIRD_1
                                                   : TextureID::BIRD_0;
      _sprite.setTexture(kTextures[_textureID], true);
      accumulator -= kTimePerFrame;
    }
    accumulator += Config::kDeltaTimeLogicUpdate;
  } else {
    accumulator = 0.f;
  }
}

Obstacle::TextureID Obstacle::GetFirstTexture(
    const ObstacleType iObstacleType) {
  switch (iObstacleType) {
    case Obstacle::ObstacleType::CACTUS_SMALL:
      return TextureID::CACTUS_SMALL;
    case Obstacle::ObstacleType::CACTUS_BIG:
      return TextureID::CACTUS_BIG;
    case Obstacle::ObstacleType::CACTUS_LARGE:
      return TextureID::CACTUS_LARGE;
    case Obstacle::ObstacleType::BIRD_LOW:
      return TextureID::BIRD_0;
    case Obstacle::ObstacleType::BIRD_MEDIUM:
      return TextureID::BIRD_0;
    case Obstacle::ObstacleType::BIRD_HIGH:
      return TextureID::BIRD_0;
  }
}

}  // namespace aimaze2
