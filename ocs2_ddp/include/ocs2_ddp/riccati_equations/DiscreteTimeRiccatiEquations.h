/******************************************************************************
Copyright (c) 2017, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#pragma once

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include <ocs2_core/Dimensions.h>
#include <ocs2_core/model_data/ModelDataBase.h>

#include "ocs2_ddp/riccati_equations/RiccatiModification.h"

namespace ocs2 {

/**
 * Data cache for discrete-time Riccati equation
 */
struct DiscreteTimeRiccatiData {
  using DIMENSIONS = Dimensions<Eigen::Dynamic, Eigen::Dynamic>;
  using scalar_t = typename DIMENSIONS::scalar_t;
  using dynamic_vector_t = typename DIMENSIONS::dynamic_vector_t;
  using dynamic_matrix_t = typename DIMENSIONS::dynamic_matrix_t;

  dynamic_vector_t Sm_projectedHv_;
  dynamic_matrix_t Sm_projectedAm_;
  dynamic_matrix_t Sm_projectedBm_;
  dynamic_vector_t Sv_plus_Sm_projectedHv_;

  dynamic_matrix_t projectedHm_;
  dynamic_matrix_t projectedGm_;
  dynamic_vector_t projectedGv_;

  dynamic_matrix_t projectedKm_T_projectedGm_;
  dynamic_matrix_t projectedHm_projectedKm_;
  dynamic_vector_t projectedHm_projectedLv_;

  // risk sensitive data
  dynamic_vector_t Sigma_Sv_;
  dynamic_matrix_t I_minus_Sm_Sigma_;
  dynamic_matrix_t inv_I_minus_Sm_Sigma_;
  scalar_t sNextStochastic_;
  dynamic_vector_t SvNextStochastic_;
  dynamic_matrix_t SmNextStochastic_;
};

/**
 * This class implements the Riccati difference equations for iLQR problem.
 */
class DiscreteTimeRiccatiEquations {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  using DIMENSIONS = Dimensions<Eigen::Dynamic, Eigen::Dynamic>;
  using scalar_t = typename DIMENSIONS::scalar_t;
  using dynamic_vector_t = typename DIMENSIONS::dynamic_vector_t;
  using dynamic_matrix_t = typename DIMENSIONS::dynamic_matrix_t;

  /**
   * Constructor.
   *
   * @param [in] reducedFormRiccati: The reduced form of the Riccati equation is yield by assuming that Hessein of
   * the Hamiltonian is positive definite. In this case, the computation of Riccati equation is more efficient.
   * @param [in] isRiskSensitive: Neither the risk sensitive variant is used or not.
   */
  explicit DiscreteTimeRiccatiEquations(bool reducedFormRiccati, bool isRiskSensitive = false);

  /**
   * Default destructor.
   */
  ~DiscreteTimeRiccatiEquations() = default;

  /**
   * Sets risk-sensitive coefficient.
   */
  void setRiskSensitiveCoefficient(scalar_t riskSensitiveCoeff);

  /**
   * Computes one step Riccati difference equations.
   *
   * @param [in] projectedModelData: The projected model data.
   * @param [in] riccatiModification: The RiccatiModification.
   * @param [in] SmNext: The Riccati matrix of the next time step.
   * @param [in] SvNext: The Riccati vector of the next time step.
   * @param [in] sNext: The Riccati scalar of the next time step.
   * @param [out] projectedKm: The projected feedback controller.
   * @param [out] projectedLv: The projected feedforward controller.
   * @param [out] Sm: The current Riccati matrix.
   * @param [out] Sv: The current Riccati vector.
   * @param [out] s: The current Riccati scalar.
   */
  void computeMap(const ModelDataBase projectedModelData, const riccati_modification::Data& riccatiModification,
                  const dynamic_matrix_t& SmNext, const dynamic_vector_t& SvNext, const scalar_t& sNext, dynamic_matrix_t& projectedKm,
                  dynamic_vector_t& projectedLv, dynamic_matrix_t& Sm, dynamic_vector_t& Sv, scalar_t& s);

 private:
  /**
   * Computes one step Riccati difference equations for ILQR formulation.
   *
   * @param [in] projectedModelData: The projected model data.
   * @param [in] riccatiModification: The RiccatiModification.
   * @param [in] SmNext: The Riccati matrix of the next time step.
   * @param [in] SvNext: The Riccati vector of the next time step.
   * @param [in] sNext: The Riccati scalar of the next time step.
   * @param [out] dreCache: The discrete-time Riccati equation cache date.
   * @param [out] projectedKm: The projected feedback controller.
   * @param [out] projectedLv: The projected feedforward controller.
   * @param [out] Sm: The current Riccati matrix.
   * @param [out] Sv: The current Riccati vector.
   * @param [out] s: The current Riccati scalar.
   */
  void computeMapILQR(const ModelDataBase projectedModelData, const riccati_modification::Data& riccatiModification,
                      const dynamic_matrix_t& SmNext, const dynamic_vector_t& SvNext, const scalar_t& sNext,
                      DiscreteTimeRiccatiData& dreCache, dynamic_matrix_t& projectedKm, dynamic_vector_t& projectedLv, dynamic_matrix_t& Sm,
                      dynamic_vector_t& Sv, scalar_t& s) const;

  /**
   * Computes one step Riccati difference equations for ILEG formulation.
   *
   * @param [in] projectedModelData: The projected model data.
   * @param [in] riccatiModification: The RiccatiModification.
   * @param [in] SmNext: The Riccati matrix of the next time step.
   * @param [in] SvNext: The Riccati vector of the next time step.
   * @param [in] sNext: The Riccati scalar of the next time step.
   * @param [out] dreCache: The discrete-time Riccati equation cache date.
   * @param [out] projectedKm: The projected feedback controller.
   * @param [out] projectedLv: The projected feedforward controller.
   * @param [out] Sm: The current Riccati matrix.
   * @param [out] Sv: The current Riccati vector.
   * @param [out] s: The current Riccati scalar.
   */
  void computeMapILEG(const ModelDataBase projectedModelData, const riccati_modification::Data& riccatiModification,
                      const dynamic_matrix_t& SmNext, const dynamic_vector_t& SvNext, const scalar_t& sNext,
                      DiscreteTimeRiccatiData& dreCache, dynamic_matrix_t& projectedKm, dynamic_vector_t& projectedLv, dynamic_matrix_t& Sm,
                      dynamic_vector_t& Sv, scalar_t& s) const;

 private:
  bool reducedFormRiccati_;
  bool isRiskSensitive_;
  scalar_t riskSensitiveCoeff_ = 0.0;

  DiscreteTimeRiccatiData discreteTimeRiccatiData_;
};

}  // namespace ocs2
