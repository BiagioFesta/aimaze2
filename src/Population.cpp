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
#include "Population.hpp"
#include <algorithm>
#include <cassert>
#include <utility>

namespace aimaze2 {

void Population::init(const std::size_t iSizePopulation,
                      const int iNumInputs,
                      const int iNumOutpus) {
  _genomes.resize(iSizePopulation,
                  Genome::CreateSimpleGenome(iNumInputs, iNumOutpus));
  _species.clear();
}

const Genome& Population::getGenome(const std::size_t iIndexGenome) const
    noexcept {
  assert(iIndexGenome < _genomes.size());
  return _genomes[iIndexGenome];
}

Genome* Population::getMutableGenome(const std::size_t iIndexGenome) noexcept {
  return &(_genomes[iIndexGenome]);
}

void Population::setAllFitness(std::vector<float> iFitness) {
  _fitness = std::move(iFitness);
}

void Population::naturalSelection() {
  speciate();
  adjustFitnessWithinSpecies();
  updateFitnessSpecies();
  sortSpecies();
  killEmptySpecies();
  killStaleSpecies();
}

std::size_t Population::getPopulationSize() const noexcept {
  return _genomes.size();
}

std::size_t Population::getSpeciesSize() const noexcept {
  return _species.size();
}

void Population::speciate() {
  for (auto& species : _species) {
    species.killAll();
  }

  for (std::size_t i = 0; i < _genomes.size(); ++i) {
    const auto& freeGenome = _genomes[i];

    const auto itFinder = std::find_if(
        _species.begin(),
        _species.end(),
        [&freeGenome](const Species& iSpecies) {
          return iSpecies.getRepresentative().isSameSpecie(freeGenome);
        });

    if (itFinder != _species.end()) {
      // Push genome in that similar species
      itFinder->addGenome(i);
    } else {
      // Create new species
      _species.emplace_back(freeGenome);
      _species.back().addGenome(i);
    }
  }  // for each genome
}

void Population::adjustFitnessWithinSpecies() {
  for (const auto& species : _species) {
    const auto sizeSpecies = species.size();
    for (const auto genomeIndex : species) {
      assert(genomeIndex < _fitness.size());
      _fitness[genomeIndex] /= sizeSpecies;
    }
  }
}

void Population::updateFitnessSpecies() {
  for (auto& species : _species) {
    float maxFitness = std::numeric_limits<float>::min();
    float sumFitness = 0.f;

    for (const auto indexGenome : species) {
      assert(indexGenome < _fitness.size());
      const float fitnessGenome = _fitness[indexGenome];
      sumFitness += fitnessGenome;
      maxFitness = std::max(maxFitness, fitnessGenome);
    }

    if (maxFitness <= species.getMaxFitness()) {
      species.increaseStaleness();
    }

    species.setMaxFitness(maxFitness);
    species.setSumFitness(sumFitness);
  }
}

void Population::sortSpecies() {
  // sort withint species best first
  for (auto& species : _species) {
    std::sort(species.begin(),
              species.end(),
              [this](const Species::IndexGenome iIndexGenomeA,
                     const Species::IndexGenome iIndexGenomeB) {
                assert(iIndexGenomeA < _fitness.size());
                assert(iIndexGenomeB < _fitness.size());
                return _fitness[iIndexGenomeA] > _fitness[iIndexGenomeB];
              });
  }

  // sort all species best first
  std::sort(_species.begin(),
            _species.end(),
            [this](const Species& iSpeciesA, const Species& iSpeciesB) {
              if (iSpeciesA.isEmpty()) {
                return false;
              } else if (iSpeciesB.isEmpty()) {
                return true;
              }
              return _fitness[iSpeciesA[0]] > _fitness[iSpeciesB[0]];
            });
}

void Population::killEmptySpecies() {
  _species.erase(std::remove_if(_species.begin(),
                                _species.end(),
                                [](const Species& iSpecies) {
                                  return iSpecies.isEmpty();
                                }),
                 _species.end());
}

void Population::killStaleSpecies() {
  _species.erase(std::remove_if(_species.begin(),
                                _species.end(),
                                [](const Species& iSpecies) {
                                  return iSpecies.getStaleness() >=
                                         ConfigEvolution::kMaxStalenessSpecies;
                                }),
                 _species.end());
}

void Population::evolutionEpoch() {
  std::vector<Genome> newPopulation;
  newPopulation.reserve(_genomes.size());

  for (const auto& species : _species) {
  }
}

}  // namespace aimaze2
