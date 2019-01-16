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
#ifndef AIMAZE2__POPULATION__HPP
#define AIMAZE2__POPULATION__HPP
#include <vector>
#include "Genome.hpp"

namespace aimaze2 {

class Population {
 public:
  void init(const std::size_t iSizePopulation,
            const int iNumInputs,
            const int iNumOutpus);

  Genome* getMutableGenome(const std::size_t iIndexGenome) noexcept;

 private:
  std::vector<Genome> _genomes;
};

}  // namespace aimaze2

#endif  // AIMAZE2__POPULATION__HPP
