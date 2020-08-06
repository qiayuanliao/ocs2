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

#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <ocs2_core/Types.h>
#include <ocs2_core/control/FeedforwardController.h>
#include <ocs2_core/initialization/OperatingPoints.h>
#include <ocs2_oc/rollout/TimeTriggeredRollout.h>

#include <ocs2_ddp/ILQR.h>
#include <ocs2_oc/test/EXP0.h>

using namespace ocs2;

enum { STATE_DIM = 2, INPUT_DIM = 1 };

TEST(exp0_ilqr_test, exp0_ilqr_test) {
  ddp::Settings ddpSettings;
  ddpSettings.algorithm_ = ddp::algorithm::ILQR;
  ddpSettings.displayInfo_ = true;
  ddpSettings.displayShortSummary_ = true;
  ddpSettings.absTolODE_ = 1e-10;
  ddpSettings.relTolODE_ = 1e-7;
  ddpSettings.maxNumStepsPerSecond_ = 1000000;
  ddpSettings.maxNumIterations_ = 30;
  ddpSettings.minRelCost_ = 5e-4;
  ddpSettings.checkNumericalStability_ = true;
  ddpSettings.useFeedbackPolicy_ = true;
  ddpSettings.debugPrintRollout_ = false;
  ddpSettings.strategy_ = ddp_strategy::type::LINE_SEARCH;
  ddpSettings.lineSearch_.minStepLength_ = 0.0001;

  Rollout_Settings rolloutSettings;
  rolloutSettings.absTolODE_ = 1e-10;
  rolloutSettings.relTolODE_ = 1e-7;
  rolloutSettings.maxNumStepsPerSecond_ = 10000;

  // event times
  std::vector<scalar_t> eventTimes{0.1897};
  std::vector<size_t> subsystemsSequence{0, 1};
  std::shared_ptr<ModeScheduleManager> modeScheduleManagerPtr(new ModeScheduleManager({eventTimes, subsystemsSequence}));

  scalar_t startTime = 0.0;
  scalar_t finalTime = 2.0;

  // partitioning times
  std::vector<scalar_t> partitioningTimes;
  partitioningTimes.push_back(startTime);
  partitioningTimes.push_back(eventTimes[0]);
  partitioningTimes.push_back(finalTime);

  vector_t initState(2);
  initState << 0.0, 2.0;

  /******************************************************************************************************/
  /******************************************************************************************************/
  /******************************************************************************************************/
  // system rollout
  EXP0_System systemDynamics(modeScheduleManagerPtr);
  TimeTriggeredRollout timeTriggeredRollout(systemDynamics, rolloutSettings);

  // system constraints
  ConstraintBase systemConstraint;

  // system cost functions
  EXP0_CostFunction systemCostFunction(modeScheduleManagerPtr);

  // system operatingTrajectories
  vector_t stateOperatingPoint = vector_t::Zero(2);
  vector_t inputOperatingPoint = vector_t::Zero(1);
  OperatingPoints operatingTrajectories(stateOperatingPoint, inputOperatingPoint);

  /******************************************************************************************************/
  /******************************************************************************************************/
  /******************************************************************************************************/
  // ILQR - single-threaded version
  ddpSettings.nThreads_ = 1;
  ILQR ilqrST(&timeTriggeredRollout, &systemDynamics, &systemConstraint, &systemCostFunction, &operatingTrajectories, ddpSettings);
  ilqrST.setModeScheduleManager(modeScheduleManagerPtr);

  // ILQR - multi-threaded version
  ddpSettings.nThreads_ = 3;
  ILQR ilqrMT(&timeTriggeredRollout, &systemDynamics, &systemConstraint, &systemCostFunction, &operatingTrajectories, ddpSettings);
  ilqrMT.setModeScheduleManager(modeScheduleManagerPtr);

  // run single_threaded core ILQR
  if (ddpSettings.displayInfo_ || ddpSettings.displayShortSummary_) {
    std::cerr << "\n>>> single-threaded ILQR" << std::endl;
  }
  ilqrST.run(startTime, initState, finalTime, partitioningTimes);

  // run multi-threaded ILQR
  if (ddpSettings.displayInfo_ || ddpSettings.displayShortSummary_) {
    std::cerr << "\n>>> multi-threaded ILQR" << std::endl;
  }
  ilqrMT.run(startTime, initState, finalTime, partitioningTimes);

  /******************************************************************************************************/
  /******************************************************************************************************/
  /******************************************************************************************************/
  // get solution
  PrimalSolution solutionST = ilqrST.primalSolution(finalTime);
  PrimalSolution solutionMT = ilqrMT.primalSolution(finalTime);

  // get performance indices
  auto performanceIndecesST = ilqrST.getPerformanceIndeces();
  auto performanceIndecesMT = ilqrMT.getPerformanceIndeces();

  /******************************************************************************************************/
  /******************************************************************************************************/
  /******************************************************************************************************/
  const scalar_t expectedCost = 9.766;
  ASSERT_LT(fabs(performanceIndecesST.totalCost - expectedCost), 10 * ddpSettings.minRelCost_)
      << "MESSAGE: single-threaded ILQR failed in the EXP1's cost test!";
  ASSERT_LT(fabs(performanceIndecesMT.totalCost - expectedCost), 10 * ddpSettings.minRelCost_)
      << "MESSAGE: multi-threaded ILQR failed in the EXP1's cost test!";

  const scalar_t expectedISE1 = 0.0;
  ASSERT_LT(fabs(performanceIndecesST.stateInputEqConstraintISE - expectedISE1), 10 * ddpSettings.constraintTolerance_)
      << "MESSAGE: single-threaded ILQR failed in the EXP1's type-1 constraint ISE test!";
  ASSERT_LT(fabs(performanceIndecesMT.stateInputEqConstraintISE - expectedISE1), 10 * ddpSettings.constraintTolerance_)
      << "MESSAGE: multi-threaded ILQR failed in the EXP1's type-1 constraint ISE test!";

  const scalar_t expectedISE2 = 0.0;
  ASSERT_LT(fabs(performanceIndecesST.stateEqConstraintISE - expectedISE2), 10 * ddpSettings.constraintTolerance_)
      << "MESSAGE: single-threaded ILQR failed in the EXP1's type-2 constraint ISE test!";
  ASSERT_LT(fabs(performanceIndecesMT.stateEqConstraintISE - expectedISE2), 10 * ddpSettings.constraintTolerance_)
      << "MESSAGE: multi-threaded ILQR failed in the EXP1's type-2 constraint ISE test!";

  scalar_t ctrlFinalTime;
  if (ddpSettings.useFeedbackPolicy_) {
    ctrlFinalTime = dynamic_cast<LinearController*>(solutionST.controllerPtr_.get())->timeStamp_.back();
  } else {
    ctrlFinalTime = dynamic_cast<FeedforwardController*>(solutionST.controllerPtr_.get())->timeStamp_.back();
  }
  ASSERT_DOUBLE_EQ(solutionST.timeTrajectory_.back(), finalTime) << "MESSAGE: ILQR_ST failed in policy final time of trajectory!";
  ASSERT_DOUBLE_EQ(ctrlFinalTime, finalTime) << "MESSAGE: ILQR_ST failed in policy final time of controller!";
}
