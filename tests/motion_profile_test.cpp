#include <fstream>
#include <tuple>
#include "gtest/gtest.h"
#include "unitscpp/unitscpp.h"
#include "../control/trapezoidal_motion_profile.h"
#include <fstream>

using namespace muan;

TEST(DriveTest, DistDriven) {
  Length distance = -50 * ft;
  TrapezoidalMotionProfile<Length> motion_profile(distance, maxHighRobotSpeed,
                                                  maxHighRobotSpeed / (2 * s));
  Time t;
  Velocity last_speed = motion_profile.CalculateDerivative(0 * s);
  Length last_distance = motion_profile.Calculate(0 * s);
  std::ofstream out("out.csv");
  for (t = 0; !motion_profile.finished(t); t += .05 * s) {
    Velocity cur_speed = motion_profile.CalculateDerivative(t);
    Acceleration accel = (cur_speed - last_speed) / (.05 * s);
    EXPECT_LE(cur_speed, maxHighRobotSpeed);
    EXPECT_LE(accel.to(ft / s / s),
              (maxHighRobotSpeed / (2 * s)).to(ft / s / s) + .0001)
        << "Robot accelerating too quickly";
    EXPECT_GE(accel.to(ft / s / s),
              (-maxHighRobotSpeed / (2 * s)).to(ft / s / s) - .0001)
        << "Robot deccelerating too quickly";
    last_speed = cur_speed;

    Length cur_dist = motion_profile.Calculate(t);
    out << t << ", " << cur_dist << ", " << cur_speed << ", "
        << motion_profile.CalculateSecondDerivative(t) << std::endl;
  }
  EXPECT_NEAR(motion_profile.Calculate(t).to(ft), distance.to(ft), 0.00001);
}