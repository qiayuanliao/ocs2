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

#ifndef BALLBOT_PARAMETERS_OCS2_H_
#define BALLBOT_PARAMETERS_OCS2_H_

#include <iostream>
#include <string>
#include <vector>

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace ocs2 {
namespace ballbot {

template <typename SCALAR_T>
class BallbotParameters {
 public:
  /**
   * Constructor.
   */
  BallbotParameters() : ballRadius_(0.125), wheelRadius_(0.064), heightBallCenterToBase_(0.317) {}

  /**
   * Default destructor.
   */
  ~BallbotParameters() = default;

  /**
   * Displays the ballbot's parameters.
   */
  inline void display() {
    std::cerr << "Ballbot parameters: " << std::endl;
    std::cerr << "ballRadius:   " << ballRadius_ << std::endl;
    std::cerr << "wheelRadius:   " << wheelRadius_ << std::endl;
    std::cerr << "heightBallCenterToBase:   " << heightBallCenterToBase_ << std::endl;
  }

 public:
  // For safety, these parameters cannot be modified
  SCALAR_T ballRadius_;              // [m]
  SCALAR_T wheelRadius_;             // [m]
  SCALAR_T heightBallCenterToBase_;  // [m]
};

}  // namespace ballbot
}  // namespace ocs2

#endif  // BALLBOT_PARAMETERS_OCS2_H_
