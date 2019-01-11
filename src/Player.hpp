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
#ifndef AIMAZE2__PLAYER__HPP
#define AIMAZE2__PLAYER__HPP
#include <SFML/Graphics.hpp>
#include <array>

namespace aimaze2 {

class Player {
 public:
  static inline const sf::Vector2f kPlayerPosition{80.f, 360.f};

  void init();
  void update(const float iGameVelocity);
  void draw(sf::RenderWindow* oRender) const;

  void jump();
  void die();

  sf::FloatRect getCollisionBox() const;

 private:
  static constexpr std::size_t kNumTextures = 4;
  enum TextureID : std::size_t { RUN_0, RUN_1, JUMP, DEAD };

  std::array<sf::Texture, kNumTextures> _textures;
  sf::Sprite _playerSprite;
  TextureID _idTexture;
  bool _jumping;
  bool _dead;
  float _velocityY;
  float _gravity;

  void applyGravity();
  void updateAnimation(const float iGameVelocity);
  void drawCollisionBox(sf::RenderWindow* oRender) const;

  static TextureID NextFrameAnimation(const TextureID iTextureId) noexcept;
};

}  // namespace aimaze2

#endif  // AIMAZE2__PLAYER__HPP
