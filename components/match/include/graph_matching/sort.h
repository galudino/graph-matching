///
/// \file       sort.h
/// \brief      In-place sorts of coordinate-format edge lists.
///
/// \details    Each function sorts three parallel arrays (row, col, val)
///             together, by the key order in its name: R is row index
///             (ascending), C is column index (ascending), and W is weight
///             (descending).
///
///             Adapted from course-provided starter code.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_SORT_H
#define GRAPH_MATCHING_SORT_H

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Sort by row, then descending weight, then column.
///
/// \param      row     Row index of each entry.
/// \param      col     Column index of each entry.
/// \param      val     Weight of each entry.
/// \param      nnz     Number of entries.
///
void sort_rwc(int *row, int *col, double *val, int nnz);

///
/// \brief      Sort by row, then column, then descending weight.
///
/// \param      row     Row index of each entry.
/// \param      col     Column index of each entry.
/// \param      val     Weight of each entry.
/// \param      nnz     Number of entries.
///
void sort_rcw(int *row, int *col, double *val, int nnz);

///
/// \brief      Sort by descending weight, then row, then column.
///
/// \param      row     Row index of each entry.
/// \param      col     Column index of each entry.
/// \param      val     Weight of each entry.
/// \param      nnz     Number of entries.
///
void sort_wrc(int *row, int *col, double *val, int nnz);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_SORT_H
