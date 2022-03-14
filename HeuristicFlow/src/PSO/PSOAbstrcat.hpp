/*
 Copyright © 2022  TokiNoBug
This file is part of Heuristic.

    Heuristic is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Heuristic is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Heuristic.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef Heu_PSOABSTRCAT_HPP
#define Heu_PSOABSTRCAT_HPP

#include "../../Global"
#include "PSOOption.hpp"
#include "PSOParameterPack.hpp"

#ifdef Heu_DO_OUTPUT
#include <iostream>
#endif

namespace Heu
{

/**
 * @brief Base-base class for PSO solvers. Some fundamental typedefs and functions here.
 * 
 * @tparam Var_t Type of determination vector.
 * @tparam Record Record trainning curve or not.
 * @tparam Arg_t Any other parameters
 */
template<class Var_t,
    class Fitness_t,
    RecordOption Record,
    class Arg_t,
         typename PSOParameterPack<Var_t,Fitness_t,Arg_t>::iFun_t _iFun_,
         typename PSOParameterPack<Var_t,Fitness_t,Arg_t>::fFun_t _fFun_>
class PSOAbstract :
        public PSOParameterPack<Var_t,Fitness_t,Arg_t>,
        public PSOParameterPack<Var_t,Fitness_t,Arg_t>::template iFunBody<_iFun_>,
        public PSOParameterPack<Var_t,Fitness_t,Arg_t>::template fFunBody<_fFun_>
{
public:
    using Base_t = PSOParameterPack<Var_t,Fitness_t,Arg_t>;
    Heu_MAKE_PSOPARAMETERPACK_TYPES
    
    class Point
    {
    public:
        Var_t position;
        Fitness_t fitness;
    };

    class Particle : public Point
    {
    public:
        Var_t velocity;
        //bool isCalculated;
        Point pBest;

    };

public:
    PSOAbstract() {};
    virtual ~PSOAbstract() {};

    inline void setOption(const PSOOption & opt) {
        _option=opt;
    }

    inline const PSOOption & option() const {
        return _option;
    }

    inline size_t generation() const {
        return _generation;
    }

    inline size_t failTimes() const {
        return _failTimes;
    }

    inline const Var_t & posMin() const {
        return _posMin;
    }

    inline const Var_t & posMax() const {
        return _posMax;
    }

    inline const Var_t & velocityMax() const {
        return _velocityMax;
    }

    inline const std::vector<Particle> & population() const {
        return _population;
    }

    inline const Point & globalBest() const {
        return gBest;
    }

    inline void setPVRange(const Var_t & pMin,
            const Var_t & pMax,
            const Var_t & vMax) {
        _posMin=pMin;
        _posMax=pMax;
        _velocityMax=vMax;
    }

    void initializePop() {
        _population.resize(_option.populationSize);

        for(Particle & i : _population) {

            if constexpr (Base_t::HasParameters) {
                this->runiFun(&i.position,&i.velocity,&_posMin,&_posMax,&_velocityMax,&this->_arg);
                this->runfFun(&i.position,&this->_arg,&i.fitness);
            }
            else {
                this->runiFun(&i.position,&i.velocity,&_posMin,&_posMax,&_velocityMax);
                this->runfFun(&i.position,&i.fitness);
            }
            i.pBest=i;
        }

        gBest=_population.front();
        _generation=0;
        _failTimes=0;
    }

    virtual void run() {
        _generation=0;
        _failTimes=0;

        while(true) {
            _generation++;
            calculateAll();
            updatePGBest();
            if(_generation>_option.maxGeneration) {
#ifdef Heu_DO_OUTPUT
                    std::cout<<"Terminated by max generation limit"<<std::endl;
#endif
                    break;
            }

            if(_option.maxFailTimes>0&&_failTimes>_option.maxFailTimes) {
#ifdef Heu_DO_OUTPUT
                    std::cout<<"Terminated by max failTime limit"<<std::endl;
#endif
                    break;
            }
#ifdef Heu_DO_OUTPUT
            std::cout<<"Generation "<<_generation
                        //<<" , elite fitness="<<_eliteIt->fitness()
                       <<std::endl;
#endif
            updatePopulation();

            customOptAfterEachGeneration();
        }
        _generation--;

    }

protected:
    PSOOption _option;
    size_t _generation;
    size_t _failTimes;

    Var_t _posMin;
    Var_t _posMax;
    Var_t _velocityMax;

    std::vector<Particle> _population;

    Point gBest;

    virtual void calculateAll() {
#ifdef Heu_USE_THREADS
        static const int32_t thN=HfGlobal::threadNum();
#pragma omp parallel for
        for(int32_t begIdx=0;begIdx<thN;begIdx++) {
            for(int32_t i=begIdx;i<_population.size();i+=thN) {
                Particle * ptr=&_population[i];
                if constexpr (Base_t::HasParameters)
                    this->runfFun(&ptr->position,&this->_arg,&ptr->fitness);
                else
                    this->runfFun(&ptr->position,&ptr->fitness);
            }
        }
#else
        for(Particle & i : _population) {
            if constexpr (Base_t::HasParameters)
                this->runfFun(&i.position,&this->_arg,&i.fitness);
            else
                this->runfFun(&i.position,&i.fitness);

        }
#endif
    }

    virtual void updatePGBest()=0;

    virtual void updatePopulation()=0;

    virtual void customOptAfterEachGeneration() {};
};

#define Heu_MAKE_PSOABSTRACT_TYPES \
Heu_MAKE_PSOPARAMETERPACK_TYPES \
using Point_t = typename Base_t::Point; \
using Particle_t = typename Base_t::Particle;





///partial specialization for PSO with recording
template<class Var_t,class Fitness_t,class Arg_t,
         typename PSOParameterPack<Var_t,Fitness_t,Arg_t>::iFun_t _iFun_,
         typename PSOParameterPack<Var_t,Fitness_t,Arg_t>::fFun_t _fFun_>
class PSOAbstract<Var_t,Fitness_t,RECORD_FITNESS,Arg_t,_iFun_,_fFun_>
        : public PSOAbstract<Var_t,Fitness_t,DONT_RECORD_FITNESS,Arg_t,_iFun_,_fFun_>
{
public:
    using Base_t = PSOAbstract<Var_t,Fitness_t,DONT_RECORD_FITNESS,Arg_t,_iFun_,_fFun_>;
    Heu_MAKE_PSOABSTRACT_TYPES

    const std::vector<Fitness_t> & record() const {
        return _record;
    }

    virtual Fitness_t bestFitness() const=0;

    virtual void run() {
        this->_generation=0;
        this->_failTimes=0;
        _record.clear();
        _record.reserve(this->_option.maxGeneration+1);
        while(true) {
            this->_generation++;
            this->calculateAll();
            this->updatePGBest();
            _record.emplace_back(bestFitness());
            if(this->_generation>this->_option.maxGeneration) {
#ifdef Heu_DO_OUTPUT
                    std::cout<<"Terminated by max generation limit"<<std::endl;
#endif
                    break;
                }
                if(this->_option.maxFailTimes>0
                        &&this->_failTimes>this->_option.maxFailTimes) {
#ifdef Heu_DO_OUTPUT
                    std::cout<<"Terminated by max failTime limit"<<std::endl;
#endif
                    break;
                }
#ifdef Heu_DO_OUTPUT
                std::cout<<"Generation "<<this->_generation
                        //<<" , elite fitness="<<_eliteIt->fitness()
                       <<std::endl;
#endif
                this->updatePopulation();
                this->customOptAfterEachGeneration();
        }
        this->_generation--;

    }
protected:
    std::vector<Fitness_t> _record;
};

}


#endif // PSOABSTRCAT_HPP