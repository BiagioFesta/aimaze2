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
#ifndef AIMAZE2__INFO_DRAWNER__HPP
#define AIMAZE2__INFO_DRAWNER__HPP
#include <SFML/Graphics.hpp>
#include <vector>
#include "Genome.hpp"
#include "Population.hpp"

namespace aimaze2 {

class InfoDrawner {
 public:
  static constexpr unsigned int kSizeText = 9;
  static constexpr float kSpacingLine = 10.f;

  void init();
  void update(const Population& iPopulation,
              const int iNumAlive,
              const Genome& iGenome,
              const int iGenerationNum);
  void draw(sf::RenderWindow* oRender) const;

 private:
  sf::Font _font;
  std::vector<sf::Text> _textInfos;

  void updateTextStrPopulationSize(const Population& iPopulation);
  void updateTextStrNumAlive(const int iNumAlive);
  void updateTextStrGenerationNum(const int iGenerationNum);
  void updateTextGenome(const Genome& iGenome);
  void updateTextPositions();
};

}  // namespace aimaze2

#endif  // AIMAZE2__INFO_DRAWNER__HPP
