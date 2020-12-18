
/*****************************************************************************
 * FileWave.h      Header
 *
 * Purpose:        This header file hold the definition of the .inh file 
 *                 structure 
 *
 * Revision History:
 *
 ****************************************************************************/

#ifndef __WAVE_H__
#define __WAVE_H__

#ifdef  __cplusplus
extern "C" {
#endif


#include "charts.h"


#define    WAVE_NEXT_RECORD           (-1)


typedef struct
{
  char           file_type[128];
  uint8_t        endian;

  float          software_version;
  float          file_version;

  int32_t        header_size;
  int32_t        text_block_size;
  int32_t        bin_block_size;
  int32_t        hardware_block_size;
  int32_t        haps_block_size;
  int32_t        other_block_size;

  int16_t        record_size;
  int16_t        shot_data_size;
  int16_t        wave_form_size;
  int16_t        pmt_size;
  int16_t        apd_size;
  int16_t        ir_size;
  int16_t        raman_size;

  int8_t         ab_system_type;
  int8_t         ab_system_number;
  int16_t        system_rep_rate;

  char           project[64];
  char           mission[64];
  char           dataset[64];
  char           flightline_number[64];
  int16_t        coded_fl_number;
  char           flight_date[64];
  char           start_time[64];
  char           end_time[64];
  int64_t        start_timestamp;  /* time in microseconds from 01/01/1970 */
  int64_t        end_timestamp;    /* time in microseconds from 01/01/1970 */
  int32_t        number_shots;
  char           dataset_create_date[64];
  char           dataset_create_time[64];
  uint16_t       ac_zero_offset[4];  /*  These aren't stored in the ASCII portion of the header.
                                         They are stored in the hardware portion of the header
                                         at offset 572 from the beginning of the hardware
                                         header block.  */
} WAVE_HEADER_T;

typedef struct
{
  int64_t        timestamp;        /* time in microseconds from 01/01/1970 */
  uint8_t        *shot_data;       /* undefined shot data */
  uint8_t        *pmt;
  uint8_t        *apd;
  uint8_t        *ir;
  uint8_t        *raman;
} WAVE_DATA_T;


  int32_t wave_read_header (FILE *fp, WAVE_HEADER_T *head);
  FILE *open_wave_file (char *path);
  int32_t wave_read_record (FILE *fp, int32_t num, WAVE_DATA_T *record);
  void wave_dump_record (WAVE_DATA_T record);


#ifdef  __cplusplus
}
#endif


#endif
