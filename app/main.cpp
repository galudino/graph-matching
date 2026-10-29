///
/// \file       main.cpp
/// \brief      Entry point for the graph_matching command-line application.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/greeting.hpp>
#include <graph_matching/ring_buffer.hpp>
#include <graph_matching/sensor.h>

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
    std::println("{}", graph_matching::make_greeting("Codespaces"));

    // Queue a few readings from the C library, then drain them in order.
    auto readings = graph_matching::ring_buffer<int, 4>{};

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
