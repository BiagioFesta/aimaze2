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
#ifndef AIMAZE2__OBSTACLE__HPP
#define AIMAZE2__OBSTACLE__HPP
#include <SFML/Graphics.hpp>
#include <array>
#include "Config.hpp"

namespace aimaze2 {

class Obstacle {
 public:
  static constexpr std::size_t kNumTypeOfObstacles = 6;
  enum class ObstacleType {
    CACTUS_SMALL,
    CACTUS_BIG,
    CACTUS_LARGE,
    BIRD_LOW,
    BIRD_MEDIUM,
    BIRD_HIGH
  };

  static void initTextures();

  void init(const ObstacleType iObstacleType);
  void update(const float iGameVelocity);
  void draw(sf::RenderWindow* oRender) const;

  bool isOutOfScreenOnLeft() const;

  sf::FloatRect getCollisionBox() const;

  const sf::Vector2f& getPosition() const noexcept;

 private:
  enum TextureID : std::size_t {
    CACTUS_SMALL,
    CACTUS_BIG,
    CACTUS_LARGE,
    BIRD_0,
    BIRD_1
  };
  static constexpr std::size_t kNumTextures = 5;
  static inline std::array<sf::Texture, kNumTextures> kTextures;

  sf::Sprite _sprite;
  TextureID _textureID;

  void drawCollisionBox(sf::RenderWindow* oRender) const;
  void updateAnimation();

  static TextureID GetFirstTexture(const ObstacleType iObstacleType);
};

}  // namespace aimaze2

#endif  //  AIMAZE2__OBSTACLE__HPP
