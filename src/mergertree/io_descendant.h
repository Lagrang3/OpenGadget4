/*******************************************************************************
 * \copyright   This file is part of the GADGET4 N-body/SPH code developed
 * \copyright   by Volker Springel. Copyright (C) 2014-2020 by Volker Springel
 * \copyright   (vspringel@mpa-garching.mpg.de) and all contributing authors.
 *******************************************************************************/

/*! \file  io_descendant.h
 *
 *  \brief declares the class needed for the I/O of descendants relationships
 */

#ifndef DESCENDANT_IO_H
#define DESCENDANT_IO_H

#include "gadgetconfig.h"

#ifdef MERGERTREE

#include "../data/allvars.h"
#include "gadget/dtypes.h"
#include "../data/mymalloc.h"
#include "../fof/fof.h"
#include "../io/hdf5_util.h"
#include "../io/io.h"
#include "../logs/timer.h"
#include "../main/simulation.h"
#include "../mergertree/mergertree.h"
#include "../mpi_utils/mpi_utils.h"
#include "../sort/parallel_sort.h"
#include "../subfind/subfind.h"
#include "../system/system.h"

class descendant_io : public IO_Def
{
 private:
  mergertree *MergerTree;

 public:
  descendant_io(mergertree *MergerTree_ptr, MPI_Comm comm, int format);

  void mergertree_save_descendants(int num);
  void mergertree_read_descendants(int num);

  /* supplied virtual functions */
  void fill_file_header(int writeTask, int lastTask, long long *nloc_part, long long *npart);
  void read_file_header(const char *fname, int filenr, int readTask, int lastTask, long long *nloc_part, long long *npart,
                        int *nstart);
  void get_datagroup_name(int grnr, char *gname);
  void write_header_fields(hid_t);
  void read_header_fields(const char *fname);
  void read_increase_numbers(int type, int n_for_this_task);
  int get_filenr_from_header(void);
  void set_filenr_in_header(int);
  void *get_base_address_of_structure(enum arrays array, int index);
  int get_type_of_element(int index);
  void set_type_of_element(int index, int type);

  /** Header for the standard file format.
   */
  struct io_header
  {
    long long Nsubhalos;
    long long TotNsubhalos;
    int num_files;
  };
  io_header header;

  int Nsubhalos;
  long long TotNsubhalos;

 private:
  static void io_func_descsubhalonr(IO_Def *ptr, int particle, int components, void *buffer, int mode)
  {
    descendant_io *thisobj = (descendant_io *)ptr;

    if(mode == 0)
      {
        long long *out_buffer = (long long *)buffer;
        out_buffer[0]         = thisobj->MergerTree->Descendants[particle].DescSubhaloNr;
        if(out_buffer[0] == HALONR_MAX)
          out_buffer[0] = -1;
      }
    else
      {
        long long *in_buffer                                     = (long long *)buffer;
        thisobj->MergerTree->Descendants[particle].DescSubhaloNr = in_buffer[0];
        if(thisobj->MergerTree->Descendants[particle].DescSubhaloNr == -1)
          thisobj->MergerTree->Descendants[particle].DescSubhaloNr = HALONR_MAX;
      }
  }

  static void io_func_firstdescsubhalonr(IO_Def *ptr, int particle, int components, void *buffer, int mode)
  {
    descendant_io *thisobj = (descendant_io *)ptr;

    if(mode == 0)
      {
        long long *out_buffer = (long long *)buffer;
        out_buffer[0]         = thisobj->MergerTree->Descendants[particle].FirstDescSubhaloNr;
        if(out_buffer[0] == HALONR_MAX)
          out_buffer[0] = -1;
      }
    else
      {
        long long *in_buffer                                          = (long long *)buffer;
        thisobj->MergerTree->Descendants[particle].FirstDescSubhaloNr = in_buffer[0];
        if(thisobj->MergerTree->Descendants[particle].FirstDescSubhaloNr == -1)
          thisobj->MergerTree->Descendants[particle].FirstDescSubhaloNr = HALONR_MAX;
      }
  }

  static void io_func_nextsubhalonr(IO_Def *ptr, int particle, int components, void *buffer, int mode)
  {
    descendant_io *thisobj = (descendant_io *)ptr;

    if(mode == 0)
      {
        long long *out_buffer = (long long *)buffer;
        out_buffer[0]         = thisobj->MergerTree->Descendants[particle].NextProgSubhaloNr;
        if(out_buffer[0] == HALONR_MAX)
          out_buffer[0] = -1;
      }
    else
      {
        long long *in_buffer                                         = (long long *)buffer;
        thisobj->MergerTree->Descendants[particle].NextProgSubhaloNr = in_buffer[0];
        if(thisobj->MergerTree->Descendants[particle].NextProgSubhaloNr == -1)
          thisobj->MergerTree->Descendants[particle].NextProgSubhaloNr = HALONR_MAX;
      }
  }
};

#endif

#endif /* DESCENDANT_IO_H */
