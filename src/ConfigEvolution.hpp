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
#ifndef AIMAZE2__CONFIG_EVOLUTION__HPP
#define AIMAZE2__CONFIG_EVOLUTION__HPP
#include <random>

namespace aimaze2 {

class ConfigEvolution {
 public:
  using RndEngine = std::mt19937_64;

  static constexpr float kProbDisableCross = 0.75;
  static constexpr float kExcessCoefficient = 1.f;
  static constexpr float kDisjointCoefficient = 1.0f;
  static constexpr float kWeightsCoefficient = 3.f;
  static constexpr int kNormalizeSizeGene = 20;
  static constexpr float kThresholdSpeciate = 3.0f;
};

}  // namespace aimaze2

#endif  // AIMAZE2__CONFIG_EVOLUTION__HPP
