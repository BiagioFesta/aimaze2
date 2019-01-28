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
#ifndef AIMAZE2__AIMAZE__HPP
#define AIMAZE2__AIMAZE__HPP
#include <SFML/Graphics.hpp>
#include "GameScene.hpp"
#include "Population.hpp"

namespace aimaze2 {

class AIMaze {
 public:
  void launch();

 private:
  static constexpr int kNumInputs = 5;
  static constexpr int kNumOuputs = 2;
  static constexpr std::size_t kSizePopulation = 100;

  sf::RenderWindow _renderWindow;
  Config::RndEngine _rndEngine;  // TODO(biagio): has to be initialized
  GameScene _gameScene;
  Population _population;

  void createAndOpenRender();
  int update();
  bool drawRender();

  void setInputsAndFeedPopulation();
  void applyActionPopulation();
  void updateGenomeToDraw();
};

}  // namespace aimaze2

#endif  // AIMAZE2__AIMAZE__HPP
