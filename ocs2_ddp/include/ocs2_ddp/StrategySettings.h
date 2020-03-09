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

#include <string>

#include <ocs2_core/OCS2NumericTraits.h>
#include <ocs2_ddp/HessianCorrection.h>

namespace ocs2 {

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
namespace ddp_strategy {

/**
 * @brief The DDP strategy enum
 * Enum used in selecting either LINE_SEARCH, LEVENBERG_MARQUARDT, or TRUST_REGION strategies.
 */
enum class type { LINE_SEARCH, LEVENBERG_MARQUARDT };

/**
 * Get string name of DDP_Strategy type
 * @param [in] strategy: DDP_Strategy type enum
 */
std::string toString(type strategy);

/**
 * Get DDP_Strategy type from string name, useful for reading config file
 * @param [in] name: DDP_Strategy name
 */
type fromString(const std::string& name);

}  // namespace ddp_strategy

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
namespace line_search {

/**
 * This structure contains the settings for the Line-Search strategy.
 */
struct Settings {
  /** Minimum step length of line-search strategy. */
  double minStepLength_ = 0.05;
  /** Maximum step length of line-search strategy. */
  double maxStepLength_ = 1.0;
  /** Line-search strategy contraction rate. */
  double contractionRate_ = 0.5;
  /** Armijo coefficient, c defined as f(u + a*p) < f(u) + c*a dfdu.dot(p)  */
  double armijoCoefficient_ = 1e-4;
  /** The Hessian correction strategy. */
  hessian_correction::Strategy hessianCorrectionStrategy_ = hessian_correction::Strategy::DIAGONAL_SHIFT;
  /** The multiple used for correcting the Hessian for numerical stability of the Riccati backward pass.*/
  double hessianCorrectionMultiple_ = OCS2NumericTraits<double>::limitEpsilon();
};  // end of Settings

/**
 * This function loads the "Line_Search" variables from a config file.
 * Here, we use the INFO format which was created specifically for the property tree library (refer to www.goo.gl/fV3yWA).
 * @param [in] filename: File name which contains the configuration data.
 * @param [in] fieldName: Field name which contains the configuration data.
 * @param [in] verbose: Flag to determine whether to print out the loaded settings or not (The default is true).
 */
Settings load(const std::string& filename, const std::string& fieldName, bool verbose = true);

}  // namespace line_search

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
namespace levenberg_marquardt {

/**
 * This structure contains the settings for the Levenberg-Marquardt strategy.
 */
struct Settings {
  /** Minimum pho (the ratio between actual reduction and predicted reduction) to accept the iteration's solution.
   * minAcceptedPho_ should be [0, 0.25);
   * */
  double minAcceptedPho_ = 0.25;
  /** The default ratio of geometric progression for Riccati multiple. */
  double riccatiMultipleDefaultRatio_ = 2.0;
  /** The default scalar-factor of geometric progression for Riccati multiple. */
  double riccatiMultipleDefaultFactor_ = 1e-6;
  /** Maximum number of successive rejections of the iteration's solution. */
  size_t maxNumSuccessiveRejections_ = 5;
};  // end of Settings

/**
 * This function loads the "Levenberg_Marquardt" variables from a config file.
 * Here, we use the INFO format which was created specifically for the property tree library (refer to www.goo.gl/fV3yWA).
 * @param [in] filename: File name which contains the configuration data.
 * @param [in] fieldName: Field name which contains the configuration data.
 * @param [in] verbose: Flag to determine whether to print out the loaded settings or not (The default is true).
 */
Settings load(const std::string& filename, const std::string& fieldName, bool verbose = true);

}  // namespace levenberg_marquardt

}  // namespace ocs2
