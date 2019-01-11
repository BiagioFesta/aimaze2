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
#include "AIMaze.hpp"
#include "Config.hpp"

namespace aimaze2 {

void AIMaze::launch() {
  createAndOpenRender();
  _gameScene.init(&_rndEngine);

  sf::Event event;

  bool keepRunning = true;
  while (keepRunning) {
    if (_renderWindow.pollEvent(event)) {
      if (event.type == sf::Event::EventType::Closed) {
        keepRunning = false;
      }
    }

    update();
    drawRender();

    // TODO(biagio): sleep to avoid High CPU
    sf::sleep(sf::milliseconds(1));
  }
}

void AIMaze::createAndOpenRender() {
  _renderWindow.create(
      sf::VideoMode{Config::kWindowWidth, Config::kWindowHeight},
      Config::kWindowTitle);
}

int AIMaze::update() {
  static sf::Clock clockUpdate;
  static float accumulator = 0.f;

  int numFrame = 0;
  accumulator += clockUpdate.restart().asSeconds();
  while (accumulator >= Config::kDeltaTimeLogicUpdate) {
    _gameScene.update(&_rndEngine);
    ++numFrame;
    accumulator -= Config::kDeltaTimeLogicUpdate;
  }

  return numFrame;
}

bool AIMaze::drawRender() {
  static constexpr float kPeriodDraw = 1.f / Config::kFPSRenderDraw;

  static sf::Clock clockRender;

  if (clockRender.getElapsedTime().asSeconds() >= kPeriodDraw) {
    _renderWindow.clear(Config::kRenderBackgroundColor);
    _gameScene.draw(&_renderWindow);
    _renderWindow.display();
    clockRender.restart();
    return true;
  }

  return false;
}

}  // namespace aimaze2
