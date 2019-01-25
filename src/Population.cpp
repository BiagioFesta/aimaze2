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

namespace {

float RndProbability(aimaze2::ConfigEvolution::RndEngine* iRndEngine) {
  static constexpr float kMaxFloat = std::numeric_limits<float>::max();

  // TODO(biagio): to check
  std::uniform_real_distribution<float> rnd(0, std::nextafter(1.0, kMaxFloat));
  return rnd(*iRndEngine);
}

}  // anonymous namespace

namespace aimaze2 {

Population::Population() : _innovationHistory(0) {}

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
  assert(iIndexGenome < _genomes.size());
  return &(_genomes[iIndexGenome]);
}

void Population::setAllFitness(std::vector<float> iFitness) {
  _fitness = std::move(iFitness);
}

void Population::naturalSelection(ConfigEvolution::RndEngine* ioRndEngine) {
  speciate();
  killEmptySpecies();
  adjustFitnessWithinSpecies();
  sortSpecies();
  cullSpecies();
  updateFitnessSpecies();
  killStaleSpecies();
  evolutionEpoch(ioRndEngine);
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
  _sumOfFitnessSum = 0.f;

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

    _sumOfFitnessSum += sumFitness;
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

void Population::cullSpecies() {
  for (auto& species : _species) {
    species.cullLower(ConfigEvolution::kPercentageCullSpecies);
  }
}

void Population::evolutionEpoch(ConfigEvolution::RndEngine* ioRndEngine) {
  assert(_sumOfFitnessSum != 0.f);

  const std::size_t sizePopulation = _genomes.size();

  std::vector<IndexGenome> newPopulationIndices;
  newPopulationIndices.reserve(sizePopulation);

  for (std::size_t s = 0; s < _species.size(); ++s) {
    const auto& species = _species[s];
    assert(!species.isEmpty());

    newPopulationIndices.push_back(species[0]);

    const float ratioFitnessSum = species.getSumFitness() / _sumOfFitnessSum;
    const int numChildren =
        ratioFitnessSum * static_cast<float>(_genomes.size()) - 1;
    for (int i = 0; i < numChildren; ++i) {
      const float probability = ::RndProbability(ioRndEngine);

      if (probability < ConfigEvolution::kProbabilityCloneParent) {
        newPopulationIndices.push_back(
            pickIndexGenomeFromSpecies(s, ioRndEngine));
      } else {
        const IndexGenome parentA = pickIndexGenomeFromSpecies(s, ioRndEngine);
        const IndexGenome parentB = pickIndexGenomeFromSpecies(s, ioRndEngine);

        const auto indices = std::minmax(
            parentA,
            parentB,
            [this](const IndexGenome iIndexA, const IndexGenome iIndexB) {
              assert(iIndexA < _fitness.size());
              assert(iIndexB < _fitness.size());
              return _fitness[iIndexA] > _fitness[iIndexB];
            });

        assert(indices.first < _genomes.size());
        assert(indices.second < _genomes.size());
        assert(_fitness[indices.first] >= _fitness[indices.second]);

        newPopulationIndices.push_back(_genomes.size());
        _genomes.push_back(Genome::Crossover(
            &_genomes[indices.first], &_genomes[indices.second], ioRndEngine));
      }
    }
  }  // for all s species

  assert(newPopulationIndices.size() <= sizePopulation);

  // TODO(biagio): do something when newpop < sizePop

  std::vector<Genome> newPopulation;
  newPopulation.reserve(sizePopulation);

  for (const IndexGenome index : newPopulationIndices) {
    assert(index < _genomes.size());
    newPopulation.push_back(_genomes[index]);
    newPopulation.back().mutate(ioRndEngine, &_innovationHistory);
  }

  _genomes = std::move(newPopulation);
  assert(_genomes.size() == sizePopulation);
}

Population::IndexGenome Population::pickIndexGenomeFromSpecies(
    const std::size_t iIndexSpecies,
    ConfigEvolution::RndEngine* ioRndEngine) const {
  assert(iIndexSpecies < _species.size());

  const Species& species = _species[iIndexSpecies];
  const float sumFitness = species.getSumFitness();

  std::uniform_real_distribution<float> rndPick(0.f, sumFitness);
  const float rndPickValue = rndPick(*ioRndEngine);

  float runningSum = 0.f;
  for (const Species::IndexGenome indexGenome : species) {
    assert(indexGenome < _fitness.size());
    assert(indexGenome < _genomes.size());

    runningSum += _fitness[indexGenome];
    if (rndPickValue < runningSum) {
      return indexGenome;
    }
  }

  assert(false);
  return std::numeric_limits<Species::IndexGenome>::max();
}

}  // namespace aimaze2
