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
#include <gtest/gtest.h>
#include <Population.hpp>

namespace {

constexpr std::size_t kSizePopulation = 3;
constexpr int kNumInputs = 3;
constexpr int kNumOutputs = 2;
const std::vector<float> kFitness = {1.0, 2.0, 3.0};

}  // anonymous namespace

namespace aimaze2::testing {

TEST(TestPopulation, Init) {
  Population population;
  population.init(::kSizePopulation, ::kNumInputs, ::kNumOutputs);
  ASSERT_EQ(population.getPopulationSize(), kSizePopulation);
}

TEST(TestPopulation, NaturalSelection) {
  Population population;
  population.init(::kSizePopulation, ::kNumInputs, ::kNumOutputs);
  population.setAllFitness(kFitness);
  population.naturalSelection();

  ASSERT_EQ(population.getPopulationSize(), kSizePopulation);
}

}  // namespace aimaze2::testing
