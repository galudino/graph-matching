///
/// \file       sensor_tests.cpp
/// \brief      Unit tests for the sensor library.
///
/// \author     galudino
/// \date       2026-10-24
///

#include <starter/sensor.h>

#include <doctest/doctest.h>

TEST_CASE("sensor_read returns the simulated measurement") { CHECK(sensor_read() == 42); }
