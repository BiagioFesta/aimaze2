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

namespace {

constexpr int kNumInputs = 3;
constexpr int kNumOutputs = 2;
constexpr int kNumBias = 1;
constexpr int kNumTotalNodes = kNumInputs + kNumOutputs + kNumBias;
constexpr int kNumInputsCross = 3;
constexpr int kNumOutputsCross = 1;

aimaze2::Genome BuildGenomeA() {
  using aimaze2::Genome;
  using aimaze2::InnovationHistory;

  InnovationHistory innovationHistory(1);

  Genome genome =
      Genome::CreateSimpleGenome(::kNumInputsCross, ::kNumOutputsCross);
  auto inputs = genome.getMutableInputNodes().first;
  auto outputs = genome.getMutableOutputNodes().first;

  auto node1 = inputs[0].getNodeID();
  auto node2 = inputs[1].getNodeID();
  auto node3 = inputs[2].getNodeID();
  auto node4 = outputs[0].getNodeID();

  genome.addConnection(node1, node4, 1.f, &innovationHistory);
  genome.addConnection(node2, node4, 1.f, &innovationHistory);
  genome.addConnection(node3, node4, 1.f, &innovationHistory);

  auto node5 = genome.addNode(node2, node4, &innovationHistory, false);
  auto node6 = genome.addNode(node5, node4, &innovationHistory, false);

  innovationHistory.getNextInnovationNumAndIncrement();

  genome.addConnection(node3, node5, 1.f, &innovationHistory);
  genome.addConnection(node1, node6, 1.f, &innovationHistory);

  return genome;
}

aimaze2::Genome BuildGenomeB() {
  using aimaze2::Genome;
  using aimaze2::InnovationHistory;

  InnovationHistory innovationHistory(1);

  Genome genome =
      Genome::CreateSimpleGenome(::kNumInputsCross, ::kNumOutputsCross);
  auto inputs = genome.getMutableInputNodes().first;
  auto outputs = genome.getMutableOutputNodes().first;

  auto node1 = inputs[0].getNodeID();
  auto node2 = inputs[1].getNodeID();
  auto node3 = inputs[2].getNodeID();
  auto node4 = outputs[0].getNodeID();

  genome.addConnection(node1, node4, 1.f, &innovationHistory);
  genome.addConnection(node2, node4, 1.f, &innovationHistory);
  genome.addConnection(node3, node4, 1.f, &innovationHistory);

  auto node5 = genome.addNode(node2, node4, &innovationHistory, false);

  innovationHistory.getNextInnovationNumAndIncrement();
  innovationHistory.getNextInnovationNumAndIncrement();

  genome.addConnection(node1, node5, 1.f, &innovationHistory);

  return genome;
}

}  // anonymous namespace

namespace aimaze2::testing {

TEST(TestGenome, CreateSimpleGenome) {
  Genome genome = Genome::CreateSimpleGenome(::kNumInputs, ::kNumOutputs);

  ASSERT_EQ(genome.getTotalNumNodes(), ::kNumTotalNodes);

  const auto inputs = genome.getMutableInputNodes();
  const auto outputs = genome.getMutableOutputNodes();

  ASSERT_EQ(inputs.second, ::kNumInputs);
  ASSERT_EQ(outputs.second, ::kNumOutputs);

  const auto nodes = genome.getMutableIONodes();
  GeneNode* ptrGene = inputs.first;

  int i = 0;
  for (; i < ::kNumInputs; ++i) {
    ASSERT_EQ(std::addressof((*nodes)[i]), ptrGene++);
    ASSERT_EQ((*nodes)[i].getNodeType(), GeneNode::NodeType::INPUT);
    ASSERT_EQ((*nodes)[i].getLayerID(), Genome::kIDLayerInputs);
    ASSERT_FALSE((*nodes)[i].isBiasNode());
  }

  ptrGene = outputs.first;
  for (; i < ::kNumOutputs; ++i) {
    ASSERT_EQ(std::addressof((*nodes)[i]), ptrGene++);
    ASSERT_EQ((*nodes)[i].getNodeType(), GeneNode::NodeType::OUTPUT);
    ASSERT_EQ((*nodes)[i].getLayerID(), Genome::kIDLayerOutpus);
    ASSERT_FALSE((*nodes)[i].isBiasNode());
  }
}

TEST(TestGenome, AddConnection) {
  Genome genome = Genome::CreateSimpleGenome(::kNumInputs, ::kNumOutputs);
  InnovationHistory innovationHistory(0);

  const auto nodeAID = genome.getMutableInputNodes().first[0].getNodeID();
  const auto nodeBID = genome.getMutableOutputNodes().first[0].getNodeID();

  ASSERT_EQ(genome.getNumConnections(), 0);
  genome.addConnection(nodeAID, nodeBID, 1.f, &innovationHistory);
  ASSERT_EQ(genome.getNumConnections(), 1);
}

TEST(TestGenome, AddNode) {
  Genome genome = Genome::CreateSimpleGenome(::kNumInputs, ::kNumOutputs);
  InnovationHistory innovationHistory(0);

  const auto nodeAID = genome.getMutableInputNodes().first[0].getNodeID();
  const auto nodeBID = genome.getMutableOutputNodes().first[0].getNodeID();

  genome.addConnection(nodeAID, nodeBID, 1.f, &innovationHistory);
  ASSERT_EQ(genome.getNumConnections(), 1);

  ASSERT_EQ(genome.getTotalNumNodes(), ::kNumTotalNodes);
  ASSERT_EQ(genome.getNumHiddenNodes(), 0);

  genome.addNode(nodeAID, nodeBID, &innovationHistory, true);

  ASSERT_EQ(genome.getTotalNumNodes(), ::kNumTotalNodes + 1);
  ASSERT_EQ(genome.getNumHiddenNodes(), 1);
  ASSERT_EQ(genome.getNumConnections(), 4);
  ASSERT_EQ(genome.getNumActiveConnections(), 3);
}

TEST(TestGenome, FeedForwardOnlyIO) {
  Genome genome = Genome::CreateSimpleGenome(::kNumInputs, ::kNumOutputs);
  InnovationHistory innovationHistory(0);

  const auto inputs = genome.getMutableInputNodes().first;
  const auto outputs = genome.getMutableOutputNodes().first;

  genome.feedForward();
  ASSERT_EQ(outputs[0].getPureValue(), 0.f);
  ASSERT_EQ(outputs[1].getPureValue(), 0.f);

  inputs[0].setValue(10.f);
  inputs[1].setValue(2.f);

  genome.addConnection(
      inputs[0].getNodeID(), outputs[0].getNodeID(), 0.5f, &innovationHistory);

  genome.feedForward();
  ASSERT_EQ(outputs[0].getPureValue(), 5.f);
  ASSERT_EQ(outputs[1].getPureValue(), 0.f);

  genome.addConnection(
      inputs[1].getNodeID(), outputs[0].getNodeID(), 2.f, &innovationHistory);

  genome.feedForward();
  ASSERT_EQ(outputs[0].getPureValue(), 9.f);
  ASSERT_EQ(outputs[1].getPureValue(), 0.f);
}

TEST(TestGenome, FeedForwardOneHidden) {
  Genome genome = Genome::CreateSimpleGenome(::kNumInputs, ::kNumOutputs);
  InnovationHistory innovationHistory(0);

  const auto inputs = genome.getMutableInputNodes().first;
  const auto outputs = genome.getMutableOutputNodes().first;

  inputs[0].setValue(10.f);
  inputs[1].setValue(2.f);

  genome.addConnection(
      inputs[0].getNodeID(), outputs[0].getNodeID(), 0.5f, &innovationHistory);
  genome.addConnection(
      inputs[1].getNodeID(), outputs[0].getNodeID(), 2.f, &innovationHistory);

  genome.getMutableConnections();
  genome.addNode(
      inputs[0].getNodeID(), outputs[0].getNodeID(), &innovationHistory, false);

  genome.feedForward();
  ASSERT_EQ(outputs[0].getPureValue(), 4.5f);
  ASSERT_EQ(outputs[1].getPureValue(), 0.f);
}

TEST(TestGenome, Crossover) {
  ConfigEvolution::RndEngine rndEngine;

  Genome genomeA = ::BuildGenomeA();
  Genome genomeB = ::BuildGenomeB();

  Genome child = Genome::Crossover(&genomeA, &genomeB, &rndEngine);
}

}  // namespace aimaze2::testing
