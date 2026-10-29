///
/// \file       sort.c
/// \brief      Quicksorts of coordinate-format edge lists.
///
/// \details    Adapted from course-provided starter code.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/sort.h>

///
/// \brief      Swap entries x and y of three parallel arrays.
///
/// \param      row     Row index of each entry.
/// \param      col     Column index of each entry.
/// \param      val     Weight of each entry.
/// \param      x       First entry.
/// \param      y       Second entry.
///
static void swap_entries(int *row, int *col, double *val, int x, int y) {
    const int tmp_row = row[x];
    row[x] = row[y];
    row[y] = tmp_row;

    const int tmp_col = col[x];
    col[x] = col[y];
    col[y] = tmp_col;

    const double tmp_val = val[x];
    val[x] = val[y];
    val[y] = tmp_val;
}

///
/// \brief      Quicksort entries left through right by row, column, then descending weight.
///
/// \param      row     Row index of each entry.
/// \param      col     Column index of each entry.
/// \param      val     Weight of each entry.
/// \param      left    First entry of the range.
/// \param      right   Last entry of the range.
///
static void quick_sort_rcw(int *row, int *col, double *val, int left, int right) {
    int i = left;
    int j = right;
    const int pivot = (left + right) / 2;
    const int pivot_row = row[pivot];
    const int pivot_col = col[pivot];
    const double pivot_val = val[pivot];

    while (i < j) {
        while (row[i] < pivot_row || (row[i] == pivot_row && col[i] < pivot_col) ||
               (row[i] == pivot_row && col[i] == pivot_col && val[i] > pivot_val)) {
            i++;
        }
        while (row[j] > pivot_row || (row[j] == pivot_row && col[j] > pivot_col) ||
               (row[j] == pivot_row && col[j] == pivot_col && val[j] < pivot_val)) {
            j--;
        }
        if (i <= j) {
            swap_entries(row, col, val, i, j);
            i++;
            j--;
        }
    }

    if (left < j) {
        quick_sort_rcw(row, col, val, left, j);
    }
    if (i < right) {
        quick_sort_rcw(row, col, val, i, right);
    }
}

void sort_rcw(int *row, int *col, double *val, int nnz) {
    // The quicksort reads the middle entry, so an empty range must not reach it.
    if (nnz < 2) {
        return;
    }

    quick_sort_rcw(row, col, val, 0, nnz - 1);
}

///
/// \brief      Quicksort entries left through right by row, descending weight, then column.
///
/// \param      row     Row index of each entry.
/// \param      col     Column index of each entry.
/// \param      val     Weight of each entry.
/// \param      left    First entry of the range.
/// \param      right   Last entry of the range.
///
static void quick_sort_rwc(int *row, int *col, double *val, int left, int right) {
    int i = left;
    int j = right;
    const int pivot = (left + right) / 2;
    const int pivot_row = row[pivot];
    const int pivot_col = col[pivot];
    const double pivot_val = val[pivot];

    while (i < j) {
        while (row[i] < pivot_row || (row[i] == pivot_row && val[i] > pivot_val) ||
               (row[i] == pivot_row && val[i] == pivot_val && col[i] < pivot_col)) {
            i++;
        }
        while (row[j] > pivot_row || (row[j] == pivot_row && val[j] < pivot_val) ||
               (row[j] == pivot_row && val[j] == pivot_val && col[j] > pivot_col)) {
            j--;
        }
        if (i <= j) {
            swap_entries(row, col, val, i, j);
            i++;
            j--;
        }
    }

    if (left < j) {
        quick_sort_rwc(row, col, val, left, j);
    }
    if (i < right) {
        quick_sort_rwc(row, col, val, i, right);
    }
}

void sort_rwc(int *row, int *col, double *val, int nnz) {
    // The quicksort reads the middle entry, so an empty range must not reach it.
    if (nnz < 2) {
        return;
    }

    quick_sort_rwc(row, col, val, 0, nnz - 1);
}

///
/// \brief      Quicksort entries left through right by descending weight, row, then column.
///
/// \param      row     Row index of each entry.
/// \param      col     Column index of each entry.
/// \param      val     Weight of each entry.
/// \param      left    First entry of the range.
/// \param      right   Last entry of the range.
///
static void quick_sort_wrc(int *row, int *col, double *val, int left, int right) {
    int i = left;
    int j = right;
    const int pivot = (left + right) / 2;
    const int pivot_row = row[pivot];
    const int pivot_col = col[pivot];
    const double pivot_val = val[pivot];

    while (i < j) {
        while (val[i] > pivot_val || (val[i] == pivot_val && row[i] < pivot_row) ||
               (val[i] == pivot_val && row[i] == pivot_row && col[i] < pivot_col)) {
            i++;
        }
        while (val[j] < pivot_val || (val[j] == pivot_val && row[j] > pivot_row) ||
               (val[j] == pivot_val && row[j] == pivot_row && col[j] > pivot_col)) {
            j--;
        }
        if (i <= j) {
            swap_entries(row, col, val, i, j);
            i++;
            j--;
        }
    }

    if (left < j) {
        quick_sort_wrc(row, col, val, left, j);
    }
    if (i < right) {
        quick_sort_wrc(row, col, val, i, right);
    }
}

void sort_wrc(int *row, int *col, double *val, int nnz) {
    // The quicksort reads the middle entry, so an empty range must not reach it.
    if (nnz < 2) {
        return;
    }

    quick_sort_wrc(row, col, val, 0, nnz - 1);
}
