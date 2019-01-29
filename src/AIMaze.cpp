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
#include <cassert>
#include <chrono>
#include <iostream>  // TODO(biagio): delete this line as well
#include "Config.hpp"

namespace aimaze2 {

void AIMaze::launch() {
  createAndOpenRender();
  initSeedRndEngine();
  _gameScene.init(kSizePopulation, &_rndEngine);
  _population.init(kSizePopulation, kNumInputs, kNumOuputs);
  updateGenomeToDraw();

  printInfoProgram();
  _epoch = 0;

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

    sf::sleep(sf::microseconds(1));
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
    if (_gameScene.arePlayersAllDead()) {
      _population.setAllFitness(_gameScene.getPlayerScores());
      _population.naturalSelection(&_rndEngine);
      updateGenomeToDraw();
      printEpochInfo();
      ++_epoch;
      _gameScene.init(kSizePopulation, &_rndEngine);
    } else {
      setInputsAndFeedPopulation();
      applyActionPopulation();
    }
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

void AIMaze::setInputsAndFeedPopulation() {
  const float gameVelocity = _gameScene.getGameVelocity();
  const auto& nextObstacleProperty = _gameScene.getNextObstacleProperty();

  for (std::size_t i = 0; i < kSizePopulation; ++i) {
    auto refGenome = _population.getMutableGenome(i);
    auto inputNodes = refGenome->getMutableInputNodes();
    assert(inputNodes.second == kNumInputs);
    inputNodes.first[0].setValue(gameVelocity);
    inputNodes.first[1].setValue(nextObstacleProperty._distance);
    inputNodes.first[2].setValue(nextObstacleProperty._width);
    inputNodes.first[3].setValue(nextObstacleProperty._height);
    inputNodes.first[4].setValue(nextObstacleProperty._altitude);

    refGenome->feedForward();
  }
}

void AIMaze::applyActionPopulation() {
  for (std::size_t i = 0; i < kSizePopulation; ++i) {
    auto refGenome = _population.getMutableGenome(i);
    auto outputNodes = refGenome->getMutableOutputNodes();
    assert(outputNodes.second == kNumOuputs);

    const float jump = outputNodes.first[0].getValueWithActivation();
    const float duck = outputNodes.first[1].getValueWithActivation();

    if (duck > 0.5) {
      _gameScene.playerDuckOn(i);
    } else {
      _gameScene.playerDuckOff(i);
      if (jump > 0.5) {
        _gameScene.playerJump(i);
      }
    }
  }
}

void AIMaze::updateGenomeToDraw() {
  const Genome& selectedGenome = _population.getGenome(0);
  _gameScene.updateGenomeToDraw(selectedGenome);
}

void AIMaze::initSeedRndEngine() {
#ifdef NDEBUG
  _seed = static_cast<Config::RndEngine::result_type>(
      std::chrono::system_clock::now().time_since_epoch().count());
#else
  _seed = 0;
#endif
  _rndEngine.seed(_seed);
}

void AIMaze::printInfoProgram() const {
  std::cout << "AIMaze2\n"
            << "Seed RndEngine: " << _seed << "\n"
            << "Population Size: " << kSizePopulation << "\n";
}

void AIMaze::printEpochInfo() const {
  std::cout << "  Epoch " << _epoch << "\n";
}

}  // namespace aimaze2
