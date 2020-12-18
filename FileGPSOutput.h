
/*****************************************************************************

  FileGPS.h

  The GPS file:

  This file is a subset of the GPS/POS data as collected in the air.  This 
  file is created in a mission/POS directory during the downloading of a 
  mission (where  mission  is the mission name). The extension of the file is
  .gps.  The file format is binary. The file does NOT have a header.

*****************************************************************************/


#ifndef __GPSHead__
#define __GPSHead__


#ifdef  __cplusplus
extern "C" {
#endif


#include "charts.h"


typedef struct
{
  double gps_time;                /* GPS time                                  */
  double Time1;                   /* POS Time1 // S.B. UTC                     */
  double Time2;                   /* POS Time2 // S.B. sec since turnon        */
  float HDOP;
  float VDOP;
  float fill_float[10];

  int16_t TimeType;               /* SB. Bit 2 (UTC) and Bit5 (POS time) only  */
  int16_t Mode;                   /* 0 = No data from receiver                 */
                                  /* 1 = 2d C/A                                */
                                  /* 2 = 3d C/A                                */
                                  /* 3 = 2d DGPS                               */
                                  /* 4 = 3d DGPS                               */
                                  /* 5 = float RTK                             */
                                  /* 6 = Integer wide lane RTK                 */
                                  /* 7 = Integer narrow lane RTK               */

  int16_t num_sats;               /* Number of satellites tracked...           */
  int16_t week;
  int16_t fill_short[8];
} GPS_OUTPUT_T;


  FILE *open_gps_file (char *path);
  int32_t gps_read_record (FILE *fp, GPS_OUTPUT_T *gps);
  void gps_dump_record (GPS_OUTPUT_T gps);


#ifdef  __cplusplus
}
#endif

#endif
