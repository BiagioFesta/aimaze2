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
#include "InfoDrawner.hpp"
#include <cassert>
#include "Config.hpp"

namespace aimaze2 {

void InfoDrawner::init() { _font.loadFromFile("data/Font.ttf"); }

void InfoDrawner::update(const Population& iPopulation,
                         const int iNumAlive,
                         const Genome& iGenome,
                         const int iGenerationNum) {
  _textInfos.clear();
  updateTextStrPopulationSize(iPopulation);
  updateTextStrNumAlive(iNumAlive);
  updateTextStrGenerationNum(iGenerationNum);
  updateTextGenome(iGenome);
  updateTextPositions();
}

void InfoDrawner::draw(sf::RenderWindow* oRender) const {
  for (const auto& textInfo : _textInfos) {
    oRender->draw(textInfo);
  }
}

void InfoDrawner::updateTextStrPopulationSize(const Population& iPopulation) {
  _textInfos.emplace_back(
      "Population Size: " + std::to_string(iPopulation.getPopulationSize()),
      _font,
      kSizeText);
  _textInfos.back().setFillColor(Config::kFillColor);
}

void InfoDrawner::updateTextStrNumAlive(const int iNumAlive) {
  _textInfos.emplace_back(
      "No. Alive: " + std::to_string(iNumAlive), _font, kSizeText);
  _textInfos.back().setFillColor(Config::kFillColor);
}

void InfoDrawner::updateTextStrGenerationNum(const int iGenerationNum) {
  _textInfos.emplace_back(
      "No. Generation: " + std::to_string(iGenerationNum), _font, kSizeText);
  _textInfos.back().setFillColor(Config::kFillColor);
}

void InfoDrawner::updateTextGenome(const Genome& iGenome) {
  const std::size_t kNumInputs = iGenome.getNumInputs();
  const auto& ioNodes = iGenome.getIONodes();

  for (std::size_t i = 0; i < kNumInputs; ++i) {
    assert(i < ioNodes.size());
    const float value = ioNodes[i].getValueWithActivation();
    _textInfos.emplace_back(
        "Input " + std::to_string(i) + ": " + std::to_string(value),
        _font,
        kSizeText);
    _textInfos.back().setFillColor(Config::kFillColor);
  }
}

void InfoDrawner::updateTextPositions() {
  static constexpr float kVerticalOffset = 10.f;
  static constexpr float kHorizOffset = 10.f;

  for (std::size_t i = 0; i < _textInfos.size(); ++i) {
    _textInfos[i].setPosition(
        sf::Vector2f{kHorizOffset, kVerticalOffset + kSpacingLine * i});
  }
}

}  // namespace aimaze2
