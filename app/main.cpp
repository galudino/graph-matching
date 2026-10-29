///
/// \file       main.cpp
/// \brief      Entry point for the starter command-line application.
///
/// \author     galudino
/// \date       2026-10-24
///

#include <starter/greeting.hpp>
#include <starter/ring_buffer.hpp>
#include <starter/sensor.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <print>

///
/// \brief      Application entry point.
///
/// \return     EXIT_SUCCESS on successful execution; EXIT_FAILURE if an
///             exception reaches the top level.
///
int main() try {
    std::println("{}", starter::make_greeting("Codespaces"));

    // Queue a few readings from the C library, then drain them in order.
    auto readings = starter::ring_buffer<int, 4>{};

    while (readings.push(sensor_read())) {
    }

    while (const auto reading = readings.pop()) {
        std::println("Sensor reading: {}", *reading);
    }

    return EXIT_SUCCESS;
} catch (const std::exception &error) {
    // C stdio cannot throw, so reporting the failure cannot fail the same way.
    std::fputs(error.what(), stderr);
    std::fputc('\n', stderr);

    return EXIT_FAILURE;
}
