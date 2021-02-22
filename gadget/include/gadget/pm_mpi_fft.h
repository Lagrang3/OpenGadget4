/*******************************************************************************
 * \copyright   This file is part of the GADGET4 N-body/SPH code developed
 * \copyright   by Volker Springel. Copyright (C) 2014-2020 by Volker Springel
 * \copyright   (vspringel@mpa-garching.mpg.de) and all contributing authors.
 *******************************************************************************/

/*! \file  pm_mpi_fft.h
 *
 *  \brief declaration of a class for carrying out different variants of parallel FFT transforms
 */

#ifndef PM_MPI_FFT_H
#define PM_MPI_FFT_H

#include <vector>
#include "gadget/setcomm.h"

#ifdef DOUBLEPRECISION_FFTW
typedef double fft_real;
typedef fftw_complex fft_complex;
#else
typedef float fft_real;
typedef fftwf_complex fft_complex;
#endif

#ifndef FFTW
#define CONCAT(prefix, name) prefix##name
#ifdef DOUBLEPRECISION_FFTW
#define FFTW(x) CONCAT(fftw_, x)
#else
#define FFTW(x) CONCAT(fftwf_, x)
#endif
#endif

extern template class std::vector<size_t>;
extern template class std::vector<int>;

struct fft_plan
{
  fft_plan(int ntask, int ngridx, int ngridy, int ngridz);

  int NgridX, NgridY, NgridZ;
  int Ngridz, Ngrid2;

  FFTW(plan) forward_plan_zdir;
  FFTW(plan) forward_plan_xdir;
  FFTW(plan) forward_plan_ydir;

  FFTW(plan) backward_plan_zdir;
  FFTW(plan) backward_plan_ydir;
  FFTW(plan) backward_plan_xdir;

#ifndef FFT_COLUMN_BASED

  std::vector<int> slab_to_task; /*!< Maps a slab index to the task responsible for the slab */
  std::vector<int> slabs_x_per_task;
  std::vector<int> first_slab_x_of_task; /*!< Array containing the index of the first slab of each task */
  std::vector<int> slabs_y_per_task;     /*!< Array containing the number of slabs each task is responsible for */
  std::vector<int> first_slab_y_of_task; /*!< Array containing the index of the first slab of each task */

  int nslab_x, slabstart_x, nslab_y, slabstart_y;
  int largest_x_slab; /*!< size of the largest slab in x direction */
  int largest_y_slab; /*!< size of the largest slab in y direction */

#else
  size_t max_datasize;
  size_t fftsize;

  int firstcol_XY, ncol_XY, lastcol_XY;
  int firstcol_XZ, ncol_XZ, lastcol_XZ;
  int firstcol_ZY, ncol_ZY, lastcol_ZY;

  int transposed_firstcol, transposed_ncol;
  int second_transposed_firstcol, second_transposed_ncol;
  size_t second_transposed_ncells;

  //  int pivotcol; /* to go from column number to task */
  //   int avg;
  //   int tasklastsection;

  std::vector<size_t> offsets_send_A, offsets_recv_A, offsets_send_B, offsets_recv_B, offsets_send_C, offsets_recv_C, offsets_send_D,
      offsets_recv_D;

  std::vector<size_t> count_send_A, count_recv_A, count_send_B, count_recv_B, count_send_C, count_recv_C, count_send_D, count_recv_D,
      count_send_13, count_recv_13, count_send_23, count_recv_23, count_send_13back, count_recv_13back, count_send_23back,
      count_recv_23back;
#endif
};

class pm_mpi_fft : public setcomm, public fft_plan
{
 public:
  pm_mpi_fft(MPI_Comm comm, int nx, int ny, int nz);

  void my_slab_based_fft(fft_real *data, fft_real *workspace, int forward);

  void my_column_based_fft(fft_real *data, fft_real *workspace, int forward);

  void my_slab_transposeA(fft_real *field, fft_real *scratch);
  void my_slab_transposeB(fft_real *field, fft_real *scratch);

  void my_fft_swap23(fft_real *data, fft_real *out);
  void my_fft_swap13(fft_real *data, fft_real *out);
  void my_fft_swap23back(fft_real *data, fft_real *out);
  void my_fft_swap13back(fft_real *data, fft_real *out);

 private:
#ifndef FFT_COLUMN_BASED

  void my_slab_transpose(void *av, void *bv, int *sx, int *firstx, int *sy, int *firsty, int nx, int ny, int nz, int mode);

#else
  void my_fft_column_remap(fft_complex *data, int Ndims[3], int in_firstcol, int in_ncol, fft_complex *out, int perm[3],
                           int out_firstcol, int out_ncol, size_t *offset_send, size_t *offset_recv, size_t *count_send,
                           size_t *count_recv, size_t just_count_flag);

  void my_fft_column_transpose(fft_real *data, int Ndims[3], /* global dimensions of data cube */
                               int in_firstcol, int in_ncol, /* first column and number of columns */
                               fft_real *out, int perm[3], int out_firstcol, int out_ncol, size_t *count_send, size_t *count_recv,
                               size_t just_count_flag);

#endif
};

#endif
