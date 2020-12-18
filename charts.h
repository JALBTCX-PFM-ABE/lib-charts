
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/

#ifndef __CHARTS_H__
#define __CHARTS_H__


#ifdef  __cplusplus
extern "C" {
#endif

#define __STDC_FORMAT_MACROS
#define __STDC_LIMIT_MACROS

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>


#ifndef MAX
#define MAX(x,y) 	( ((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x,y) 	( ((x) < (y)) ? (x) : (y))
#endif


#undef CHARTS_DEBUG


  void charts_cvtime (int64_t micro_sec, int32_t *year, int32_t *jday, int32_t *hour, int32_t *minute, float *second);
  void charts_jday2mday (int32_t year, int32_t jday, int32_t *mon, int32_t *mday);
  void charts_swap_int32_t (int32_t *word);
  void charts_swap_float (float *word);
  void charts_swap_double (double *word);
  void charts_swap_int64_t (int64_t *word);
  void charts_swap_int16_t (int16_t *word);
  void lidar_get_string (char *in, char *out);


#ifdef  __cplusplus
}
#endif


#endif
