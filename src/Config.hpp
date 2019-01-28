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
#ifndef AIMAZE2__CONFIG__HPP
#define AIMAZE2__CONFIG__HPP
#include <SFML/Graphics.hpp>
#include <random>

namespace aimaze2 {

class Config {
 public:
  using RndEngine = std::mt19937_64;

  static constexpr unsigned int kWindowWidth = 1024;
  static constexpr unsigned int kWindowHeight = 576;
  static constexpr const char* kWindowTitle = "AIMaze2";
  static inline const sf::Color kRenderBackgroundColor{255, 255, 255};
  static constexpr unsigned int kFPSRenderDraw = 120;
  static constexpr unsigned kFPSLogicUpdate = 1000;
  static constexpr float kDeltaTimeLogicUpdate = 1.f / kFPSLogicUpdate;
  static inline const sf::Color kFillColor{0, 0, 0};
#ifdef NDEBUG
  static constexpr bool kDrawCollisionBox = false;
#else
  static constexpr bool kDrawCollisionBox = true;
#endif
  static inline const sf::Color kDebugColor{255, 0, 0};
};

}  // namespace aimaze2

#endif  // AIMAZE2__CONFIG__HPP
