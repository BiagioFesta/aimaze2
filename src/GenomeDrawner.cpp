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
#include "Config.hpp"

namespace aimaze2 {

void GenomeDrawner::init() {
  _view.reset(
      sf::FloatRect{0.f, 0.f, Config::kWindowWidth, Config::kWindowHeight});
  _view.setViewport(sf::FloatRect{0.5f, 0.1f, 1.f, 1.f});
}

void GenomeDrawner::updateWithGenome(const Genome& iGenome) {
  buildFromGenome(iGenome);
}

void GenomeDrawner::draw(sf::RenderWindow* oRender) const {
  oRender->setView(_view);

  for (const auto& connectionSprite : _connectionSprites) {
    oRender->draw(connectionSprite.data(), connectionSprite.size(), sf::Lines);
  }

  for (const auto& nodeSprite : _nodeSprites) {
    oRender->draw(nodeSprite._sprite);
    oRender->draw(nodeSprite._label);
  }
  oRender->setView(oRender->getDefaultView());
}

GenomeDrawner::NodeSprite::NodeSprite(const sf::Vector2f& iPosition,
                                      const NodeID iNodeID)
    : _id(iNodeID), _sprite(kPixelSizeNode) {
  _sprite.setPosition(iPosition);
  _sprite.setFillColor(Config::kFillColor);
}

void GenomeDrawner::buildFromGenome(const Genome& iGenome) {
  static constexpr float kWidthScreen =
      static_cast<float>(Config::kWindowWidth) / 2.f;
  static constexpr float kHeightScreen =
      static_cast<float>(Config::kWindowHeight) / 2.f;

  _nodeSprites.clear();
  _connectionSprites.clear();

  const int numLayers = iGenome.getNumLayers();
  const float pixelsOffSetPerLayer =
      kWidthScreen / static_cast<float>(numLayers);

  std::vector<std::vector<NodeID>> idsOnLayers;
  idsOnLayers.resize(numLayers);

  for (const auto& node : iGenome.getIONodes()) {
    auto idLayer = node.getLayerID();
    if (idLayer == Genome::kIDLayerOutpus) {
      idLayer = numLayers - 1;
    }

    assert(idLayer < static_cast<int>(idsOnLayers.size()));
    idsOnLayers[idLayer].push_back(node.getNodeID());
  }

  for (const auto& node : iGenome.getHiddenNodes()) {
    auto idLayer = node.getLayerID();
    assert(idLayer < static_cast<int>(idsOnLayers.size()));
    idsOnLayers[idLayer].push_back(node.getNodeID());
  }

  sf::Vector2f positionNode;
  for (std::size_t i = 0; i < idsOnLayers.size(); ++i) {
    const float pixelOffsetPerLine =
        kHeightScreen / static_cast<float>(idsOnLayers[i].size());

    positionNode.y = 0;
    for (const auto id : idsOnLayers[i]) {
      _nodeSprites.emplace_back(positionNode, id);
      positionNode.y += pixelOffsetPerLine;
    }

    positionNode.x += pixelsOffSetPerLayer;
  }

  for (const auto& connection : iGenome.getConnections()) {
    const auto& spriteFrom = getNodeSpriteFromID(connection.getNodeFromID());
    const auto& spriteTo = getNodeSpriteFromID(connection.getNodeToID());

    _connectionSprites.emplace_back();
    _connectionSprites.back()[0].position = spriteFrom._sprite.getPosition();
    _connectionSprites.back()[0].position +=
        sf::Vector2f{kPixelSizeNode, kPixelSizeNode};
    _connectionSprites.back()[0].color = Config::kFillColor;
    _connectionSprites.back()[1].position = spriteTo._sprite.getPosition();
    _connectionSprites.back()[1].position +=
        sf::Vector2f{kPixelSizeNode, kPixelSizeNode};
    _connectionSprites.back()[1].color = Config::kFillColor;
  }
}

const GenomeDrawner::NodeSprite& GenomeDrawner::getNodeSpriteFromID(
    const NodeID iNodeID) const {
  auto itFinder = std::find_if(_nodeSprites.cbegin(),
                               _nodeSprites.cend(),
                               [iNodeID](const NodeSprite& nodeSprite) {
                                 return nodeSprite._id == iNodeID;
                               });
  if (itFinder == _nodeSprites.cend()) {
    throw std::runtime_error("");
  }

  return *itFinder;
}

}  // namespace aimaze2
