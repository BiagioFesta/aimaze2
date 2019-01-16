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
#include "GenomeDrawner.hpp"  // TODO(biagio): delete this line

namespace {

aimaze2::Genome BuildDebugGenome() {  // TODO(biagio): delete this function
  using aimaze2::Genome;
  using aimaze2::InnovationHistory;

  InnovationHistory innovationHistory(1);

  Genome genome = Genome::CreateSimpleGenome(3, 1);
  auto inputs = genome.getMutableInputNodes().first;
  auto outputs = genome.getMutableOutputNodes().first;

  auto node1 = inputs[0].getNodeID();
  auto node2 = inputs[1].getNodeID();
  auto node3 = inputs[2].getNodeID();
  auto node4 = outputs[0].getNodeID();

  genome.addConnection(node1, node4, 1.f, &innovationHistory);
  genome.addConnection(node2, node4, 1.f, &innovationHistory);
  genome.addConnection(node3, node4, 1.f, &innovationHistory);

  auto node5 = genome.addNode(node2, node4, &innovationHistory, true);
  auto node6 = genome.addNode(node5, node4, &innovationHistory, true);

  innovationHistory.getNextInnovationNumAndIncrement();

  genome.addConnection(node3, node5, 1.f, &innovationHistory);
  genome.addConnection(node1, node6, 1.f, &innovationHistory);

  return genome;
}

}  // anonymous namespace

namespace aimaze2 {

void AIMaze::launch() {
  createAndOpenRender();
  _gameScene.init(2, &_rndEngine);

  sf::Event event;

  bool keepRunning = true;
  while (keepRunning) {
    if (_renderWindow.pollEvent(event)) {
      if (event.type == sf::Event::EventType::Closed) {
        keepRunning = false;
      } else if (event.type == sf::Event::EventType::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Up) {
          _gameScene.playerJump(0);
        } else if (event.key.code == sf::Keyboard::Key::Down) {
          _gameScene.playerDuckOn(0);
        } else if (event.key.code == sf::Keyboard::Key::Space) {
          _gameScene.playerJump(1);
        }
      } else if (event.type == sf::Event::EventType::KeyReleased) {
        if (event.key.code == sf::Keyboard::Key::Down) {
          _gameScene.playerDuckOff(0);
        }
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
    drawDebugGenome();
    _renderWindow.display();
    clockRender.restart();
    return true;
  }

  return false;
}

void AIMaze::drawDebugGenome() {
  Genome genome = ::BuildDebugGenome();

  GenomeDrawner drawner;
  drawner.updateWithGenome(genome);

  drawner.draw(&_renderWindow);
}

}  // namespace aimaze2
