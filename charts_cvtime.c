
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

#include "charts.h"

void charts_cvtime (int64_t micro_sec, int32_t *year, int32_t *jday, 
                    int32_t *hour, int32_t *minute, float *second)
{
    static int32_t       tz_set = 0;
    struct tm            time_struct, *time_ptr = &time_struct;
    time_t               tv_sec;
    int32_t              msec;


    if (!tz_set)
    {
#ifdef NVWIN3X
  #if defined (__MINGW64__) || defined (__MINGW32__)
        putenv("TZ=GMT");
        tzset();
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
    

    tv_sec = micro_sec / 1000000;
    time_ptr = localtime (&tv_sec);

    msec = micro_sec % 1000000;

    *year = (short) time_ptr->tm_year;
    *jday = (short) time_ptr->tm_yday + 1;
    *hour = (short) time_ptr->tm_hour;
    *minute = (short) time_ptr->tm_min;
    *second = (float) time_ptr->tm_sec + (float) ((double) msec / 1000000.);
}



void charts_jday2mday (int32_t year, int32_t jday, int32_t *mon, int32_t *mday)
{
  static int32_t              months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int32_t                     l_year;

  l_year = year;

  if (year < 1899) l_year += 1900;


  /*  If the year is evenly divisible by 4 but not by 100, or it's evenly divisible by 400, this is a leap year.  */

  if ((!(l_year % 4) && (l_year % 100)) || !(l_year % 400))
    {
      months[1] = 29;
    }
  else
    {
      months[1] = 28;
    }


    *mday = jday;
    for (*mon = 0 ; *mon < 12 ; (*mon)++)
    {
        if (*mday - months[*mon] <= 0) break;
        *mday -= months[*mon];
    }
}
