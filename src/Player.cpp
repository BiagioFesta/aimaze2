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
#include "Player.hpp"
#include <cassert>
#include "Config.hpp"

namespace aimaze2 {

void Player::init() {
  std::get<TextureID::RUN_0>(_textures).loadFromFile("data/dinorun0000.png");
  std::get<TextureID::RUN_1>(_textures).loadFromFile("data/dinorun0001.png");
  std::get<TextureID::JUMP>(_textures).loadFromFile("data/dinoJump0000.png");
  std::get<TextureID::DEAD>(_textures).loadFromFile("data/dinoDead0000.png");
  std::get<TextureID::DUCK_0>(_textures).loadFromFile("data/dinoduck0000.png");
  std::get<TextureID::DUCK_1>(_textures).loadFromFile("data/dinoduck0001.png");

  _playerSprite.setPosition(kPlayerPosition);

  _idTexture = TextureID::RUN_0;
  _jumping = false;
  _dead = false;
  _ducking = false;
  _velocityY = 0.f;
  _gravity = 0.f;

  _accumulatorAnimation = 0.f;
}

void Player::update(const float iGameVelocity) {
  updateAnimation(iGameVelocity);

  if (_dead == false) {
    _playerSprite.move({0.f, _velocityY * Config::kDeltaTimeLogicUpdate});
    applyGravity();
  }
}

void Player::draw(sf::RenderWindow* oRender) const {
  oRender->draw(_playerSprite);

  if constexpr (Config::kDrawCollisionBox) {
    drawCollisionBox(oRender);
  }
}

void Player::jump() {
  static constexpr float kVelocityJump = 900.f;
  static constexpr float kGravity = 2000.f;

  if (!_jumping && !_dead) {
    _jumping = true;
    duckOff();
    _velocityY = -kVelocityJump;
    _gravity = kGravity;
  }
}

void Player::die(const sf::Vector2f& iNewPosition) {
  _dead = true;
  _jumping = false;
  duckOff();
  _playerSprite.setPosition(iNewPosition);
}

void Player::duckOn() {
  if (!_jumping && !_dead) {
    _ducking = true;
  }
}

void Player::duckOff() {
  if (_ducking) {
    _ducking = false;
    resetGroundPosition();
  }
}

sf::FloatRect Player::getCollisionBox() const {
  auto spriteBox = _playerSprite.getGlobalBounds();
  spriteBox.width -= 50.f;
  spriteBox.left += 25.f;
  spriteBox.height -= 10.f;
  spriteBox.top += 10.f;

  return spriteBox;
}

void Player::applyGravity() {
  if (_playerSprite.getPosition().y < kPlayerPosition.y) {
    _velocityY += _gravity * Config::kDeltaTimeLogicUpdate;
  } else {
    _jumping = false;
    _velocityY = 0;
    resetGroundPosition();
  }
}

void Player::updateAnimation(const float iGameVelocity) {
  static constexpr float kScaleVelocityAnimation = 80.f;

  if (_dead == true) {
    _idTexture = TextureID::DEAD;
  } else if (_jumping == true) {
    _idTexture = TextureID::JUMP;
  } else if (_ducking == true) {
    if (_idTexture != TextureID::DUCK_0 && _idTexture != TextureID::DUCK_1) {
      _idTexture = TextureID::DUCK_0;
    }
  } else if (_idTexture != TextureID::RUN_0 && _idTexture != TextureID::RUN_1) {
    _idTexture = TextureID::RUN_0;
  }

  assert(_idTexture < _textures.size());
  _playerSprite.setTexture(_textures[_idTexture], true);

  const float timePerFrame = kScaleVelocityAnimation / iGameVelocity;

  if (timePerFrame <= _accumulatorAnimation) {
    _idTexture = NextFrameAnimation(_idTexture);
    _accumulatorAnimation -= timePerFrame;
  }

  _accumulatorAnimation += Config::kDeltaTimeLogicUpdate;
}

void Player::drawCollisionBox(sf::RenderWindow* oRender) const {
  sf::RectangleShape box;

  const auto collisionBox = getCollisionBox();
  box.setSize(sf::Vector2f{collisionBox.width, collisionBox.height});
  box.setOutlineColor(Config::kDebugColor);
  box.setOutlineThickness(2.f);
  box.setFillColor(sf::Color{0, 0, 0, 0});
  box.setPosition(sf::Vector2f{collisionBox.left, collisionBox.top});

  oRender->draw(box);
}

void Player::resetGroundPosition() {
  if (_ducking) {
    _playerSprite.setPosition(kPlayerPosition + sf::Vector2f{0.f, 50.f});
  } else if (!_jumping) {
    _playerSprite.setPosition(kPlayerPosition);
  }
}

Player::TextureID Player::NextFrameAnimation(
    const TextureID iTextureId) noexcept {
  switch (iTextureId) {
    case TextureID::RUN_0:
      return TextureID::RUN_1;
    case TextureID::RUN_1:
      return TextureID::RUN_0;
    case TextureID::JUMP:
      return TextureID::JUMP;
    case TextureID::DEAD:
      return TextureID::DEAD;
    case TextureID::DUCK_0:
      return TextureID::DUCK_1;
    case TextureID::DUCK_1:
      return TextureID::DUCK_0;
  }
}

}  // namespace aimaze2
