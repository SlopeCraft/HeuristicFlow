/*
 Copyright © 2022  TokiNoBug
This file is part of HeuristicFlow.

    HeuristicFlow is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    HeuristicFlow is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HeuristicFlow.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef Heu_NSGABASE_HPP
#define Heu_NSGABASE_HPP

#include "MOGABase.hpp"

#ifdef Heu_NSGA_USE_THREADS
#ifndef Heu_USE_THREADS
#error You allowed parallelize in NSGA2 but not in global.  \
    Macro Heu_NSGA2_USE_THREADS can only be defined when Heu_USE_THREADS is defined.
#endif
#endif

namespace Heu {

template<typename Var_t,
        size_t ObjNum,
        DoubleVectorOption DVO,
        FitnessOption fOpt,
        RecordOption rOpt,
        PFOption pfOpt,
        class Args_t,
         typename GAAbstract<Var_t,FitnessVec_t<DVO,ObjNum>,Args_t>::initializeFun _iFun_,
         typename GAAbstract<Var_t,FitnessVec_t<DVO,ObjNum>,Args_t>::fitnessFun _fFun_,
         typename GAAbstract<Var_t,FitnessVec_t<DVO,ObjNum>,Args_t>::crossoverFun _cFun_,
         typename GAAbstract<Var_t,FitnessVec_t<DVO,ObjNum>,Args_t>::mutateFun _mFun_>
class NSGABase
    : public MOGABase<Var_t,ObjNum,DVO,fOpt,rOpt,pfOpt,Args_t,
            _iFun_,_fFun_,_cFun_,_mFun_>
{
public:
    NSGABase() {};
    virtual ~NSGABase() {};

    using Base_t = MOGABase<Var_t,ObjNum,DVO,fOpt,rOpt,pfOpt,Args_t,
        _iFun_,_fFun_,_cFun_,_mFun_>;
    Heu_MAKE_GABASE_TYPES
    
    /**
     * @brief basical unit for NS
     * 
     */
    struct infoUnitBase
    {
    public:
        /** @brief Genes in population that strong domain this gene
        */
        size_t domainedByNum;

        /** @brief iterator to related gene
        */
        GeneIt_t iterator;
    };  //  infoUnitBase

    void initializePop() {
        sortSpace.clear();
        sortSpace.reserve(2*this->_option.populationSize);
        Base_t::initializePop();
    }

protected:
    std::vector<infoUnitBase*> sortSpace;
    std::list<std::vector<infoUnitBase*>> pfLayers;

    inline static bool sortByDominatedNum(const infoUnitBase * A,const infoUnitBase* B) {
        return (A->domainedByNum)<(B->domainedByNum);
    }

    //calculate domainedByNum
    virtual void calculateDominatedNum() {
        const size_t popSizeBefore=sortSpace.size();
#ifdef Heu_NSGA_USE_THREADS
        static const int64_t thN=HfGlobal::threadNum();
#pragma omp parallel for
        for(int64_t begIdx=0;begIdx<thN;begIdx++) {

            for(int64_t ed=begIdx;ed<popSizeBefore;ed+=thN) {
                sortSpace[ed]->domainedByNum=0;
                for(size_t er=0;er<popSizeBefore;er++) {
                    if(er==ed)
                        continue;
                    sortSpace[ed]->domainedByNum+=
                            Pareto<ObjNum,DVO,fOpt>::isStrongDominate(&(sortSpace[er]->iterator->_Fitness),
                                           &(sortSpace[ed]->iterator->_Fitness));
                }
            }
        }

#else
        for(size_t ed=0;ed<popSizeBefore;ed++) {
            sortSpace[ed]->domainedByNum=0;
            for(size_t er=0;er<popSizeBefore;er++) {
                if(er==ed)
                    continue;
                sortSpace[ed]->domainedByNum+=
                        Pareto<ObjNum,DVO,fOpt>::isStrongDominate(&(sortSpace[er]->iterator->_Fitness),
                                       &(sortSpace[ed]->iterator->_Fitness));
            }
        }
#endif

    }   //calculateDominatedNum()

    virtual void divideLayers() {
        std::sort(sortSpace.begin(),sortSpace.end(),sortByDominatedNum);
        pfLayers.clear();
        const size_t popSizeBef=sortSpace.size();
        size_t curDM=-1;
        for(auto i : sortSpace) {
            if(curDM!=i->domainedByNum) {
                curDM=i->domainedByNum;
                pfLayers.emplace_back();
                pfLayers.back().reserve(popSizeBef);
            }
            pfLayers.back().emplace_back(i);
        }
    }

    void updatePF(const infoUnitBase ** pfs,const size_t curFrontSize) {
            this->_pfGenes.clear();
            for(size_t i=0;i<curFrontSize;i++) {
                this->_pfGenes.emplace(&*(pfs[i]->iterator));
            }
            if(this->prevFrontSize!=curFrontSize) {
                this->_failTimes=0;
                this->prevFrontSize=curFrontSize;
            }
            else {
                size_t checkSum=this->makePFCheckSum();

                if(this->prevPFCheckSum==checkSum) {
                    this->_failTimes++;
                } else {
                    this->_failTimes=0;
                    this->prevPFCheckSum=checkSum;
                }
            }
    }   //updatePF()


};  //NSGABase


#define Heu_MAKE_NSGABASE_TYPES \
Heu_MAKE_GABASE_TYPES \
using infoUnitBase_t = typename Base_t::infoUnitBase; \
using Fitness_t = typename Base_t::Fitness_t;

}   // Heu

#endif  //  NSGABASE_HPP
