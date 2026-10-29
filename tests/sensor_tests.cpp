///
/// \file       sensor_tests.cpp
/// \brief      Unit tests for the sensor library.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/sensor.h>

#include <doctest/doctest.h>

TEST_CASE("sensor_read returns the simulated measurement") { CHECK(sensor_read() == 42); }
