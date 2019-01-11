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
#include "Ground.hpp"

namespace {

float getRndYPositionOnGround(aimaze2::Config::RndEngine* iRndEngine) {
  using aimaze2::Config;
  using aimaze2::Ground;

  static std::uniform_real_distribution<float> sRndYGroundPosition(
      5.f + static_cast<float>(Config::kWindowHeight) -
          static_cast<float>(Ground::kHeightGround),
      static_cast<float>(Config::kWindowHeight));
  return sRndYGroundPosition(*iRndEngine);
}

void buildRockSprite(sf::RectangleShape* oRockSprite,
                     aimaze2::Config::RndEngine* iRndEngine) {
  constexpr int kKindOfRock = 2;
  static const std::array<sf::Vector2f, kKindOfRock> kSizesRocks{
      sf::Vector2f{3.f, 2.f}, sf::Vector2f{6.f, 2.f}};
  static std::uniform_int_distribution<> kRndSize(0, kKindOfRock - 1);

  oRockSprite->setSize(kSizesRocks[kRndSize(*iRndEngine)]);
  oRockSprite->setFillColor(aimaze2::Config::kFillColor);
}

void ifOutScreenRegenerate(sf::RectangleShape* ioRockSprite,
                           aimaze2::Config::RndEngine* iRndEngine) {
  using aimaze2::Config;

  if (!ioRockSprite->getGlobalBounds().intersects(sf::FloatRect{
          0.f, 0.f, Config::kWindowWidth, Config::kWindowHeight})) {
    ::buildRockSprite(ioRockSprite, iRndEngine);
    ioRockSprite->setPosition(sf::Vector2f{
        Config::kWindowWidth, ::getRndYPositionOnGround(iRndEngine)});
  }
}

}  // anonymous namespace

namespace aimaze2 {

void Ground::init(Config::RndEngine* iRndEngine) {
  _lineSprite.setSize({Config::kWindowWidth, 2});
  _lineSprite.setFillColor(Config::kFillColor);
  _lineSprite.setPosition({0.f, Config::kWindowHeight - kHeightGround});

  _rockSprites.resize(kNumOfRocks);
  for (auto& rockSprite : _rockSprites) {
    ::buildRockSprite(&rockSprite, iRndEngine);
  }

  distributeRocksPosition(iRndEngine);
}

void Ground::update(const float iGameVelocity, Config::RndEngine* iRndEngine) {
  const float kDeltaMovement = iGameVelocity * Config::kDeltaTimeLogicUpdate;

  for (auto& rockSprite : _rockSprites) {
    rockSprite.move({-kDeltaMovement, 0.f});
    ::ifOutScreenRegenerate(&rockSprite, iRndEngine);
  }
}

void Ground::draw(sf::RenderWindow* oRender) const {
  oRender->draw(_lineSprite);
  for (const auto& rockSprite : _rockSprites) {
    oRender->draw(rockSprite);
  }
}

void Ground::distributeRocksPosition(Config::RndEngine* iRndEngine) {
  constexpr float kGapPosition = static_cast<float>(Config::kWindowWidth) /
                                 static_cast<float>(kNumOfRocks);

  float accumulatorX = 0.f;
  for (auto& rockSprite : _rockSprites) {
    rockSprite.setPosition(accumulatorX, ::getRndYPositionOnGround(iRndEngine));
    accumulatorX += kGapPosition;
  }
}

}  // namespace aimaze2
