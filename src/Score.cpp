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
#include "Score.hpp"
#include "Config.hpp"

namespace aimaze2 {

void Score::init() {
  _font.loadFromFile("data/Font.ttf");

  _scoreText.setFont(_font);
  _scoreText.setFillColor(Config::kFillColor);
  _scoreText.setCharacterSize(18);

  _score = 0;
}

void Score::update() {
  updateScoreValue();

  _scoreText.setString(std::to_string(_score));
  _scoreText.setPosition({Config::kWindowWidth -
                              _scoreText.getGlobalBounds().width -
                              kHorizontalOffsetSprite,
                          kVerticalOffsetSprite});
}

void Score::draw(sf::RenderWindow* oRender) const { oRender->draw(_scoreText); }

void Score::updateScoreValue() {
  static constexpr float kTimePerIncrement = 0.1f;
  static float accumulator = 0.0f;

  if (kTimePerIncrement <= accumulator) {
    ++_score;
    accumulator -= kTimePerIncrement;
  }
  accumulator += Config::kDeltaTimeLogicUpdate;
}

}  // namespace aimaze2
