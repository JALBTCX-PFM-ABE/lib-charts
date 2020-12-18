
/*****************************************************************************

  FilePOSOutput.h

  The POS file:

  This file is a subset of the DGPS/POS data as collected in the air.  This 
  file is created in a mission/POS directory during the downloading of a 
  mission (where  mission  is the mission name). The extension of the file is
  .pos.  The file format is binary and the data will be 200 Hz. The file does 
  NOT have a header.


  The POS SBET file:

  This file is NOT created during the download process. It will be created 
  after processing of GPS and POS data by the user in POSPROC. The filename 
  will be in the form  sbet_xxx.out  where the xxx part of the name is 
  controlled by the user processing the data. The file location is also user 
  controlled. This file is identical in structure to the POS Airborne file 
  (above), except that IMU and POS data have been post processed.

*****************************************************************************/


#ifndef POSHead
#define POSHead


#ifdef  __cplusplus
extern "C" {
#endif


#include "charts.h"


typedef struct
{
  double         gps_time;         /* seconds in GPS week                  */
  double         latitude;         /* radians                              */
  double         longitude;        /* radians                              */
  double         altitude;         /* meters from ellipsoid                */
  double         x_velocity;       /* m/sec                                */
  double         y_velocity;       /* m/sec                                */
  double         z_velocity;       /* m/sec                                */
  double         roll;             /* radians                              */
  double         pitch;            /* radians                              */
  double         platform_heading; /* radians  ** see below                */
  double         wander_angle;     /* radians  ** see below                */
  double         x_body_accel;     /* m/sec                                */
  double         y_body_accel;     /* m/sec                                */
  double         z_body_accel;     /* m/sec                                */
  double         x_body_ang_rate;  /* m/sec                                */
  double         y_body_ang_rate;  /* m/sec                                */
  double         z_body_ang_rate;  /* m/sec                                */
} POS_OUTPUT_T;

  /*

  ** To get a "true heading" the following equation should be used:
  true_heading = platform_heading - wander_angle

  */


  uint8_t get_pos_file (char *hof_tof_file, char *pos_file);
  FILE *open_pos_file (char *path);
  int64_t pos_find_record (FILE *fp, POS_OUTPUT_T *pos, int64_t timestamp);
  int64_t pos_get_start_timestamp ();
  int64_t pos_get_end_timestamp ();
  int64_t pos_get_timestamp (POS_OUTPUT_T pos);
  int32_t pos_read_record (FILE *fp, POS_OUTPUT_T *pos);
  int32_t pos_read_record_num (FILE *fp, POS_OUTPUT_T *pos, int32_t recnum);
  void pos_dump_record (POS_OUTPUT_T pos);


#ifdef  __cplusplus
}
#endif

#endif
