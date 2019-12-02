#include <ocs2_core/control/LinearController.h>
#include <ocs2_oc/rollout/Rollout_Settings.h>
#include <ocs2_oc/rollout/StateTriggeredRollout.h>
#include "include/ocs2_oc/test/ball_dynamics_staterollout.h"
#include "include/ocs2_oc/test/dynamics_hybrid_slq_test.h"
#include "include/ocs2_oc/test/pendulum_dynamics_staterollout.h"
#include "ocs2_core/Dimensions.h"

#include <gtest/gtest.h>

TEST(StateRolloutTests, rolloutTestBallDynamics) {

  using DIMENSIONS = ocs2::Dimensions<2, 1>;
  using controller_t = ocs2::ControllerBase<2, 1>;

  using scalar_t = typename DIMENSIONS::scalar_t;
  using state_vector_t = typename DIMENSIONS::state_vector_t;
  using time_interval_t = std::pair<scalar_t, scalar_t>;
  using input_vector_t = typename DIMENSIONS::input_vector_t;
  using input_state_matrix_t = typename DIMENSIONS::input_state_matrix_t;

  using scalar_array_t = typename DIMENSIONS::scalar_array_t;
  using size_array_t = typename DIMENSIONS::size_array_t;
  using state_vector_array_t = typename DIMENSIONS::state_vector_array_t;
  using input_vector_array_t = typename DIMENSIONS::input_vector_array_t;
  using input_state_matrix_array_t = typename DIMENSIONS::input_state_matrix_array_t;
  using dynamic_vector_t = typename DIMENSIONS::dynamic_vector_t;

  using time_interval_array_t = std::vector<time_interval_t>;

  // Construct State TriggerdRollout Object
  ocs2::Rollout_Settings RolloutSettings;
  ocs2::ballDyn dynamics;
  ocs2::StateTriggeredRollout<2,1> rollout(dynamics, RolloutSettings);
  // Construct Variables for run
  // Simulation time
  scalar_t t0 = 0;
  scalar_t t1 = 10;
  // Initial State
  state_vector_t initState(2, 0);
  initState[0] = 1;
  // Initial Event times (none)
  scalar_array_t eventTimes(1, t0);
  // Controller (time constant zero controller)
  // Controller (time constant zero controller)
   scalar_array_t timestamp(1, t0);

   input_vector_t bias;
   bias << 0;
   input_vector_array_t biasArray(1, bias);

   input_state_matrix_t gain;
   gain << 1, 0;
   input_state_matrix_array_t gainArray(1, gain);
   ocs2::LinearController<2, 1> control(timestamp, biasArray, gainArray);
   ocs2::LinearController<2, 1>* controller = &control;

  // Trajectory storage
  scalar_array_t timeTrajectory(0);
  size_array_t eventsPastTheEndIndeces(0);
  state_vector_array_t stateTrajectory(0);
  input_vector_array_t inputTrajectory(0);
  // Output State
  state_vector_t finalState;
  // Run
  finalState =
      rollout.run(t0, initState, t1, controller, eventTimes, timeTrajectory, eventsPastTheEndIndeces, stateTrajectory, inputTrajectory);

  for (int i = 0; i < timeTrajectory.size(); i++) {
    // Test 1: Energy Conservation
    double energy = 9.81 * stateTrajectory[i][0] + 0.5 * stateTrajectory[i][1] * stateTrajectory[i][1];
    EXPECT_LT(std::fabs(energy - 9.81*stateTrajectory[0][0] + 0.5*stateTrajectory[0][1]*stateTrajectory[0][1]), 1e-6);
    // Test 2a: No Significant penetration of first Guard Surface
    EXPECT_GT(stateTrajectory[i][0], -1e-6);
    // Test 2b: No Significant penetration of second Guard Surface, after first event
    if(i>eventsPastTheEndIndeces[0])
    {
    	EXPECT_GT(-stateTrajectory[i][0] + 0.1 +timeTrajectory[i]/50, -1e-6);
    }
    // Optional output of state and time trajectories
    if (false) {
      std::cout << i << ";" << timeTrajectory[i] << ";" << stateTrajectory[i][0] << ";" << stateTrajectory[i][1] << ";"
                << inputTrajectory[i] << std::endl;
    }
  }
}

TEST(StateRolloutTests, rolloutTestPendulumDynamics) {
  using DIMENSIONS = ocs2::Dimensions<2, 1>;
  using controller_t = ocs2::ControllerBase<2, 1>;

  using scalar_t = typename DIMENSIONS::scalar_t;
  using state_vector_t = typename DIMENSIONS::state_vector_t;
  using time_interval_t = std::pair<scalar_t, scalar_t>;
  using input_vector_t = typename DIMENSIONS::input_vector_t;
  using input_state_matrix_t = typename DIMENSIONS::input_state_matrix_t;

  using scalar_array_t = typename DIMENSIONS::scalar_array_t;
  using size_array_t = typename DIMENSIONS::size_array_t;
  using state_vector_array_t = typename DIMENSIONS::state_vector_array_t;
  using input_vector_array_t = typename DIMENSIONS::input_vector_array_t;
  using input_state_matrix_array_t = typename DIMENSIONS::input_state_matrix_array_t;
  using dynamic_vector_t = typename DIMENSIONS::dynamic_vector_t;

  using time_interval_array_t = std::vector<time_interval_t>;
  // Construct State TriggerdRollout Object
  ocs2::Rollout_Settings rolloutSettings;
  ocs2::pendulum_dyn dynamics;
  ocs2::StateTriggeredRollout<2, 1> rollout(dynamics, rolloutSettings);
  // Create Logic Rules
  ocs2::pendulum_logic logic;
  ocs2::pendulum_logic* logicRules = &logic;
  // Construct Variables for run
  // Simulation time
  scalar_t t0 = 0;
  scalar_t t1 = 15;
  // Initial State
  state_vector_t initState(2, 0);
  initState[0] = 3.1415;
  // Initial Event times (none)
  scalar_array_t eventTimes(1, t0);
  // Controller (time constant zero controller)
  scalar_array_t timestamp(1, t0);
  // bias Array of Controller
  input_vector_t bias;
  bias << 0;
  input_vector_array_t biasArray(1, bias);
  // gain Array of Controller
  input_state_matrix_t gain;
  gain << 0, 0;
  input_state_matrix_array_t gainArray(1, gain);

  ocs2::LinearController<2, 1> control(timestamp, biasArray, gainArray);
  ocs2::LinearController<2, 1>* controller = &control;

  // Trajectory storage
  scalar_array_t timeTrajectory(0);
  size_array_t eventsPastTheEndIndeces(0);
  state_vector_array_t stateTrajectory(0);
  input_vector_array_t inputTrajectory(0);
  // Output State
  state_vector_t finalState;
  // Run
  finalState =
      rollout.run(t0, initState, t1, controller, eventTimes, timeTrajectory, eventsPastTheEndIndeces, stateTrajectory, inputTrajectory);

  // logicRules->display();

  scalar_t energyPrevious = 9.81*2; // Initial energy (pendulum in upright position h = 2*L)
  size_t eventCounter = 0;

  for (int i = 0; i < timeTrajectory.size(); i++) {
    // Test 1: No Significant penetration of Guard Surface
    EXPECT_GT(stateTrajectory[i][0], -1e-6);
    // Test 2: No Significant loss of energy along trajectory (apart from due to damping during bounce)
    scalar_t h = 1 - std::cos(stateTrajectory[i][0]); // height at time i (since length = 1)
    scalar_t vx = stateTrajectory[i][1] * std::cos(stateTrajectory[i][0]); // x component velocity
    scalar_t vy = stateTrajectory[i][1] * std::sin(stateTrajectory[i][0]); // y component velocity
    scalar_t vsq = std::pow(vx,2) + std::pow(vy,2); // squared velocity
    scalar_t E = 9.81*h + 0.5*vsq;

    if (i != eventsPastTheEndIndeces[eventCounter])
    {EXPECT_LT(std::fabs(E-energyPrevious), 1e-5);}
    else{eventCounter++;}
    energyPrevious = E;

    // Optional output of state and time trajectories
    if (false) {
      std::cout << i << ";" << timeTrajectory[i] << ";" << stateTrajectory[i][0] << ";" << stateTrajectory[i][1] << ";"
                << inputTrajectory[i] << std::endl;
    }
  }
}

TEST(StateRolloutTests, runHybridDynamics) {
  using DIMENSIONS = ocs2::Dimensions<3, 1>;
  using controller_t = ocs2::ControllerBase<3, 1>;

  using scalar_t = typename DIMENSIONS::scalar_t;
  using state_vector_t = typename DIMENSIONS::state_vector_t;
  using time_interval_t = std::pair<scalar_t, scalar_t>;
  using input_vector_t = typename DIMENSIONS::input_vector_t;
  using input_state_matrix_t = typename DIMENSIONS::input_state_matrix_t;

  using scalar_array_t = typename DIMENSIONS::scalar_array_t;
  using size_array_t = typename DIMENSIONS::size_array_t;
  using state_vector_array_t = typename DIMENSIONS::state_vector_array_t;
  using input_vector_array_t = typename DIMENSIONS::input_vector_array_t;
  using input_state_matrix_array_t = typename DIMENSIONS::input_state_matrix_array_t;
  using dynamic_vector_t = typename DIMENSIONS::dynamic_vector_t;

  using time_interval_array_t = std::vector<time_interval_t>;
  // Create Logic Rules
  std::vector<double> eventTimes(0);
  std::vector<size_t> subsystemsSequence{1};
  std::shared_ptr<ocs2::hybridSysLogic> logicRules(new ocs2::hybridSysLogic(eventTimes, subsystemsSequence));
  // Construct State TriggerdRollout Object
  ocs2::Rollout_Settings rolloutSettings;
  ocs2::hybridSysDynamics dynamics;
  ocs2::StateTriggeredRollout<3, 1> rollout(dynamics, rolloutSettings);
  // Construct Variables for run
  // Simulation time
  scalar_t t0 = 0;
  scalar_t t1 = 5;
  // Initial State
  state_vector_t initState(2, 0, 1);
  initState[0] = 5;
  initState[1] = 2;
  // Controller (time constant zero controller)
  scalar_array_t timestamp(1, t0);
  input_vector_t bias;
  bias << 0;
  input_vector_array_t biasArray(1, bias);

  input_state_matrix_t gain;
  gain << 0, 0, 0;
  input_state_matrix_array_t gainArray(1, gain);
  ocs2::LinearController<3, 1> control(timestamp, biasArray, gainArray);
  ocs2::LinearController<3, 1>* Controller = &control;

  // Trajectory storage
  scalar_array_t timeTrajectory(0);
  size_array_t eventsPastTheEndIndeces(0);
  state_vector_array_t stateTrajectory(0);
  input_vector_array_t inputTrajectory(0);
  // Output State
  state_vector_t finalState;
  // Run
  finalState =
      rollout.run(t0, initState, t1, Controller, eventTimes, timeTrajectory, eventsPastTheEndIndeces, stateTrajectory, inputTrajectory);

  // logicRules->display();

  for (int i = 0; i < timeTrajectory.size(); i++) {
    // Test 1: No Significant penetration of Guard Surface
    dynamic_vector_t currentGuardValues;
    scalar_t currentTime = timeTrajectory[i];
    state_vector_t currentState = stateTrajectory[i];
    dynamics.computeGuardSurfaces(currentTime,currentState,currentGuardValues);

    EXPECT_GT(currentGuardValues[0], -1e-6);
    EXPECT_GT(currentGuardValues[1], -1e-6);
    // Optional output of state and time trajectories
    if (false) {
      std::cout << i << ";" << timeTrajectory[i] << ";" << stateTrajectory[i][0] << ";" << stateTrajectory[i][1] << ";"
                << logicRules->getSubSystemTime(timeTrajectory[i]) << ";" << inputTrajectory[i] << std::endl;
    }
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
