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
#include "Species.hpp"
#include <cassert>

namespace aimaze2 {

Species::Species(const Genome& iRepresentative)
    : _representative(iRepresentative) {}

void Species::addGenome(const IndexGenome iIndexGenome) {
  _genomeIndices.push_back(iIndexGenome);
}

std::size_t Species::size() const noexcept { return _genomeIndices.size(); }

Species::IndexGenome Species::operator[](const std::size_t iIndex) const
    noexcept {
  assert(iIndex < _genomeIndices.size());
  return _genomeIndices[iIndex];
}

Species::const_iterator Species::begin() const noexcept {
  return _genomeIndices.cbegin();
}

Species::const_iterator Species::end() const noexcept {
  return _genomeIndices.cend();
}

Species::iterator Species::begin() noexcept { return _genomeIndices.begin(); }

Species::iterator Species::end() noexcept { return _genomeIndices.end(); }

void Species::killAll() noexcept { _genomeIndices.clear(); }

bool Species::isEmpty() const noexcept { return _genomeIndices.empty(); }

const Genome& Species::getRepresentative() const noexcept {
  return _representative;
}

float Species::getMaxFitness() const noexcept { return _maxFitness; }

void Species::setMaxFitness(const float iMaxFitness) noexcept {
  _maxFitness = iMaxFitness;
}

float Species::getSumFitness() const noexcept { return _sumFitness; }

void Species::setSumFitness(const float iSumFitness) noexcept {
  _sumFitness = iSumFitness;
}

void Species::increaseStaleness() noexcept { ++_staleness; }

int Species::getStaleness() const noexcept { return _staleness; }

void Species::cullLower(const float iPercentage) {
  assert(iPercentage >= 0.f && iPercentage <= 1.f);
  const std::size_t toErase =
      static_cast<float>(_genomeIndices.size()) * iPercentage;
  _genomeIndices.erase(_genomeIndices.end() - toErase, _genomeIndices.end());
}

}  // namespace aimaze2
