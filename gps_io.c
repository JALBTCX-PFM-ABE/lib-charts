
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

#include "FileGPSOutput.h"

static uint8_t swap = 1;
static int64_t start_timestamp, end_timestamp, start_week;
static int32_t year, month, day, start_record, end_record;


static void charts_swap_gps (GPS_OUTPUT_T *gps)
{
  int32_t i;


  charts_swap_double (&gps->gps_time);
  charts_swap_double (&gps->Time1);
  charts_swap_double (&gps->Time2);
  charts_swap_float (&gps->HDOP);
  charts_swap_float (&gps->VDOP);
  for (i = 0 ; i < 10 ; i++) charts_swap_float (&gps->fill_float[i]);
  charts_swap_int16_t (&gps->TimeType);
  charts_swap_int16_t (&gps->Mode);
  charts_swap_int16_t (&gps->num_sats);
  charts_swap_int16_t (&gps->week);
  for (i = 0 ; i < 8 ; i++) charts_swap_int16_t (&gps->fill_short[i]);
}


FILE *open_gps_file (char *path)
{
  FILE                   *fp;
  GPS_OUTPUT_T           gps;
  time_t                 tv_sec;
  struct tm              tm;
  static int32_t         tz_set = 0;


  int32_t big_endian ();


  sscanf (&path[strlen (path) - 15], "%02d%02d%02d", &year, &month, &day);


  /*  tm struct wants years since 1900!!!  */

  tm.tm_year = year + 100;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = 0.0;
  tm.tm_min = 0.0;
  tm.tm_sec = 0.0;
  tm.tm_isdst = -1;

  if (!tz_set)
    {
#ifdef NVWIN3X
  #if defined (__MINGW64__) || defined (__MINGW32__)
      _putenv("TZ=GMT");
      _tzset();
  #else
      _putenv("TZ=GMT");
      _tzset();
  #endif
#else
      putenv("TZ=GMT");
      tzset();
#endif
      tz_set = 1;
    }


  /*  Get seconds from the epoch (01-01-1970) for the date in the filename. 
      This will also give us the day of the week for the GPS seconds of
      week calculation.  */

  tv_sec = mktime (&tm);


  /*  Subtract the number of days since Saturday midnight (Sunday morning) in seconds.  */

  tv_sec = tv_sec - (tm.tm_wday * 86400);
  start_week = tv_sec;


  /*  We have to assume that the file is little endian since there is no
      header and no field that we can use to deduce what it is.  */

  swap = (uint8_t) big_endian ();


  if ((fp = fopen (path, "rb")) == NULL)
    {
      return ((FILE *) NULL);
    }
  else
    {
      fread (&gps, sizeof (GPS_OUTPUT_T), 1, fp);
      if (swap) charts_swap_gps (&gps);
      start_timestamp = (int64_t) (((double) start_week + gps.gps_time) * 1000000.0);
      start_record = 0;


      fseeko64 (fp, -sizeof (GPS_OUTPUT_T), SEEK_END);

      fread (&gps, sizeof (GPS_OUTPUT_T), 1, fp);
      if (swap) charts_swap_gps (&gps);
      end_timestamp = (int64_t) (((double) start_week + gps.gps_time) * 1000000.0);

      end_record = ftell (fp) / sizeof (GPS_OUTPUT_T);

      fseek (fp, 0, SEEK_SET);
    }

  return (fp);
}


int32_t gps_read_record (FILE *fp, GPS_OUTPUT_T *gps)
{
  if (!fread (gps, sizeof (GPS_OUTPUT_T), 1, fp)) return (-1);
  if (swap) charts_swap_gps (gps);

  return (0);
}


void gps_dump_record (GPS_OUTPUT_T gps)
{
  fprintf (stderr, "GPS seconds of week : %f\n", gps.gps_time);
  fprintf (stderr, "POS Time1           : %f\n", gps.Time1);
  fprintf (stderr, "POS Time2           : %f\n", gps.Time2);
  fprintf (stderr, "HDOP                : %f\n", gps.HDOP);
  fprintf (stderr, "VDOP                : %f\n", gps.VDOP);
  fprintf (stderr, "Time type           : %hd\n", gps.TimeType);
  fprintf (stderr, "Mode                : %hd\n", gps.Mode);
  fprintf (stderr, "Num sats            : %hd\n", gps.num_sats);
  fprintf (stderr, "Week                : %hd\n", gps.week);
  fflush (stderr);
}
