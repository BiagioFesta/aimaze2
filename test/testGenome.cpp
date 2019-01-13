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
#include <Genome.hpp>
#include <memory>

namespace aimaze2::testing {

TEST(TestGenome, CreateSimpleGenome) {
  constexpr int kNumInputs = 3;
  constexpr int kNumOutputs = 2;
  constexpr int kNumBias = 1;
  constexpr int kNumTotalNodes = kNumInputs + kNumOutputs + kNumBias;

  Genome genome = Genome::CreateSimpleGenome(kNumInputs, kNumOutputs);

  ASSERT_EQ(genome.getNumNodes(), kNumTotalNodes);

  const auto inputs = genome.getMutableInputNodes();
  const auto outputs = genome.getMutableOutputNodes();

  ASSERT_EQ(inputs.second, kNumInputs);
  ASSERT_EQ(outputs.second, kNumOutputs);

  const auto nodes = genome.getMutableNodes();
  GeneNode* ptrGene = inputs.first;

  int i = 0;
  for (; i < kNumInputs; ++i) {
    ASSERT_EQ(std::addressof((*nodes)[i]), ptrGene++);
  }

  ptrGene = outputs.first;
  for (; i < kNumOutputs; ++i) {
    ASSERT_EQ(std::addressof((*nodes)[i]), ptrGene++);
  }
}

}  // namespace aimaze2::testing
