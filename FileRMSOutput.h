
/*****************************************************************************

  FileRMSOutput.h

  The RMS file:

  This file is NOT created during the download process. It will be created 
  after processing of GPS and POS data by the user in POSPROC. The filename 
  will be in the form  smrmsg_xxx.out  where the xxx part of the name is 
  controlled by the user processing the data. The file location is also user 
  controlled.

*****************************************************************************/


#ifndef RMSHead
#define RMSHead


#ifdef  __cplusplus
extern "C" {
#endif


#include "charts.h"


#define ARC_TO_DEG   0.016
#define DEG_TO_ARC   62.5


typedef struct
{
  double         gps_time;         /* seconds in GPS week                  */
  double         north_pos_rms;    /* meters                               */
  double         south_pos_rms;    /* meters                               */
  double         down_pos_rms;     /* meters                               */
  double         north_vel_rms;    /* m/sec                                */
  double         south_vel_rms;    /* m/sec                                */
  double         down_vel_rms;     /* m/sec                                */
  double         roll_rms;         /* arc minutes                          */
  double         pitch_rms;        /* arc minutes                          */
  double         heading_rms;      /* arc minutes                          */
} RMS_OUTPUT_T;


  uint8_t get_rms_file (char *hof_tof_file, char *rms_file);
  FILE *open_rms_file (char *path);
  int64_t rms_find_record (FILE *fp, RMS_OUTPUT_T *rms, int64_t timestamp);
  int64_t rms_get_start_timestamp ();
  int64_t rms_get_end_timestamp ();
  int32_t rms_read_record (FILE *fp, RMS_OUTPUT_T *rms);
  int32_t rms_read_record_num (FILE *fp, RMS_OUTPUT_T *rms, int32_t recnum);
  void rms_dump_record (RMS_OUTPUT_T rms);


#ifdef  __cplusplus
}
#endif

#endif
