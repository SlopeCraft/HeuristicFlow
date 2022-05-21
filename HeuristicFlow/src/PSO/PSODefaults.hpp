

#ifndef HEU_PSODEFAULTS_HPP
#define HEU_PSODEFAULTS_HPP

#include "InternalHeaderCheck.h"
#include "PSO.hpp"

namespace heu {

namespace internal {
template <typename Var_t, bool isVarEigenTypes>
struct __impl_PSODefaults {
  // Candidate function for initialization
  inline static void impl_iFun(Var_t *x, Var_t *v, const Var_t *xMin, const Var_t *xMax, const Var_t *) {
    for (int idx = 0; idx < xMin->size(); idx++) {
      x->operator[](idx) = randD(xMin->operator[](idx), xMax->operator[](idx));
      v->operator[](idx) = 0;
    }
  }
};

template <typename Var_t>
struct __impl_PSODefaults<Var_t, true> {
  // Candidate function for initialization
  inline static void impl_iFun(Var_t *x, Var_t *v, const Var_t *xMin, const Var_t *xMax, const Var_t *) {
    x->setRandom(xMin->size(), 1);
    (*x) *= (*xMax - *xMin) / 2;
    (*x) += (*xMin + *xMax) / 2;
    v->setZero(xMin->size(), 1);
  }
};
}  // namespace internal

/**
 * \ingroup HEU_PSO
 * \struct PSODefaults
 * \brief PSODefaults provides candidate functions for initialization
 *
 * This struct has a specialization for `Args_t` is `void`.
 *
 * \sa PSODefaults<Var_t, isVarEigenTypes, void>
 *
 * \tparam Var_t Type of decision variable
 * \tparam isVarEigenTypes If `Var_t` is Eigen's Array/Matrix(s).
 * \tparam Args_t Pseudo-global variables type in solver.
 */
template <typename Var_t, bool isVarEigenTypes = false, typename Args_t = void>
struct PSODefaults {
  static_assert(!std::is_same<Args_t, void>::value, "Wrong specialization of PSODefaults");

  /**
   * \brief Default initialize function with args
   *
   * \param pos Position to be initialized
   * \param velocity Velocity to be initialized
   * \param pMin Minmum position
   * \param pMax Maximum position
   * \param vMax Maximum speed (absolute value)
   *
   * \sa PSODefaults<Var_t, isVarEigenTypes, void>::iFun
   */
  inline static void iFun(Var_t *pos, Var_t *velocity, const Var_t *pMin, const Var_t *pMax, const Var_t *vMax,
                          const Args_t *) {
    internal::__impl_PSODefaults<Var_t, isVarEigenTypes>::impl_iFun(pos, velocity, pMin, pMax, vMax);
  }
};

/**
 * \ingroup HEU_PSO
 * \struct PSODefaults<Var_t, isVarEigenTypes, void>
 * \brief Specialization when `Args_t` is `void`.
 *
 * \sa PSODefaults
 *
 * \tparam Var_t
 * \tparam isVarEigenTypes
 */
template <typename Var_t, bool isVarEigenTypes>
struct PSODefaults<Var_t, isVarEigenTypes, void> {
  /**
   * \brief Default initialize function without args
   *
   * \param pos Position to be initialized
   * \param velocity Velocity to be initialized
   * \param pMin Minmum position
   * \param pMax Maximum position
   * \param vMax Maximum speed (absolute value)
   *
   * \sa PSODefaults::iFun
   */
  inline static void iFun(Var_t *pos, Var_t *velocity, const Var_t *pMin, const Var_t *pMax, const Var_t *vMax) {
    internal::__impl_PSODefaults<Var_t, isVarEigenTypes>::impl_iFun(pos, velocity, pMin, pMax, vMax);
  }
};

}  // namespace heu

#endif  //  HEU_PSODEFAULTS_HPP
