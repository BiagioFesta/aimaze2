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
#include "GenomeDrawner.hpp"
#include <cassert>
#include <vector>
#include "Config.hpp"

namespace aimaze2 {

void GenomeDrawner::updateWithGenome(const Genome& iGenome) {
  updateNodeSprites(iGenome);
  updateConnectionSprites(iGenome);
}

void GenomeDrawner::draw(sf::RenderWindow* oRender) const {
  sf::View view(
      sf::FloatRect{0.f, 0.f, Config::kWindowWidth, Config::kWindowHeight});
  view.setViewport(sf::FloatRect{0.5f, 0.03f, 1.f, 1.f});
  oRender->setView(view);

  drawConnections(oRender);
  drawNodes(oRender);

  oRender->setView(oRender->getDefaultView());
}

void GenomeDrawner::updateNodeSprites(const Genome& iGenome) {
  static constexpr float kWidthFrame = Config::kWindowWidth / 2.5;
  static constexpr float kHeightFrame = Config::kWindowWidth / 5;
  const auto numLayers = iGenome.getNumLayers();

  std::vector<std::vector<NodeID>> nodesPerLayer(numLayers);
  for (const auto& node : iGenome.getIONodes()) {
    auto layer = node.getLayerID();
    if (layer == Genome::kIDLayerOutpus) {
      layer = numLayers - 1;
    }
    const auto nodeID = node.getNodeID();

    assert(layer < static_cast<int>(nodesPerLayer.size()));
    nodesPerLayer[layer].push_back(nodeID);
  }
  for (const auto& node : iGenome.getHiddenNodes()) {
    auto layer = node.getLayerID();
    const auto nodeID = node.getNodeID();
    assert(layer < static_cast<int>(nodesPerLayer.size()));
    nodesPerLayer[layer].push_back(nodeID);
  }

  const float kLayerOffset = kWidthFrame / numLayers;

  _nodeSprites.clear();
  for (std::size_t layer = 0; layer < nodesPerLayer.size(); ++layer) {
    const auto numNodesOnLayer = nodesPerLayer[layer].size();
    const float kNodeOffset = kHeightFrame / numNodesOnLayer;

    for (std::size_t n = 0; n < numNodesOnLayer; ++n) {
      const NodeID nodeID = nodesPerLayer[layer][n];
      auto& sprite = _nodeSprites.emplace(nodeID, kRadiusNode).first->second;
      sprite.setFillColor(Config::kFillColor);
      sprite.setPosition({layer * kLayerOffset, n * kNodeOffset});
    }
  }
}

void GenomeDrawner::updateConnectionSprites(const Genome& iGenome) {
  _connectionSprites.clear();
  for (const auto& connection : iGenome.getConnections()) {
    const auto nodeFromID = connection.getNodeFromID();
    const auto nodeToID = connection.getNodeToID();

    const auto& spriteFrom = _nodeSprites.at(nodeFromID);
    const auto& spriteTo = _nodeSprites.at(nodeToID);

    LineShape line{spriteFrom.getPosition(), spriteTo.getPosition()};
    line[0].color = Config::kFillColor;
    line[0].position += sf::Vector2f{kRadiusNode, kRadiusNode};
    line[1].color = Config::kFillColor;
    line[1].position += sf::Vector2f{kRadiusNode, kRadiusNode};

    _connectionSprites.push_back(line);
  }
}

void GenomeDrawner::drawConnections(sf::RenderWindow* oRender) const {
  for (const auto& connection : _connectionSprites) {
    oRender->draw(connection.data(), connection.size(), sf::Lines);
  }
}

void GenomeDrawner::drawNodes(sf::RenderWindow* oRender) const {
  for (const auto& node : _nodeSprites) {
    oRender->draw(node.second);
  }
}

}  // namespace aimaze2
