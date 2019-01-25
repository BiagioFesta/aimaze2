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
#ifndef AIMAZE2__SPECIES__HPP
#define AIMAZE2__SPECIES__HPP
#include <vector>
#include "Genome.hpp"

namespace aimaze2 {

class Species {
 public:
  using IndexGenome = std::size_t;
  using Container = std::vector<IndexGenome>;
  using const_iterator = Container::const_iterator;
  using iterator = Container::iterator;

  explicit Species(const Genome& iRepresentative);

  void addGenome(const IndexGenome iIndexGenome);
  std::size_t size() const noexcept;
  IndexGenome operator[](const std::size_t iIndex) const noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  iterator begin() noexcept;
  iterator end() noexcept;
  void killAll() noexcept;
  bool isEmpty() const noexcept;

  const Genome& getRepresentative() const noexcept;

  float getMaxFitness() const noexcept;
  void setMaxFitness(const float iMaxFitness) noexcept;

  float getSumFitness() const noexcept;
  void setSumFitness(const float iSumFitness) noexcept;

  void increaseStaleness() noexcept;
  int getStaleness() const noexcept;

  void cullLower(const float iPercentage);

 private:
  Genome _representative;  // TODO(biagio): one day try to use shared_ptr
  Container _genomeIndices;
  float _maxFitness = 0.f;
  float _sumFitness = 0.f;  // TODO(biagio): I don't think you need this
  int _staleness = 0;
};

}  // namespace aimaze2

#endif  // AIMAZE2__SPECIES__HPP
