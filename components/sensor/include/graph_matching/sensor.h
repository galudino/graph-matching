///
/// \file       sensor.h
/// \brief      Public interface for the example C sensor driver.
///
/// \details    Demonstrates a C-compatible interface that can be
///             consumed by C++ without name mangling.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_SENSOR_H
#define GRAPH_MATCHING_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Reads a simulated sensor measurement.
///
/// \return     Simulated sensor value.
///
int sensor_read(void);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_SENSOR_H
