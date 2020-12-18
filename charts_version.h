
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


/*********************************************************************************************

    This library is public domain software that was developed by 
    the U.S. Naval Oceanographic Office.

    This is a work of the US Government. In accordance with 17 USC 105,
    copyright protection is not available for any work of the US Government.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*********************************************************************************************/

#ifndef CHARTS_VERSION

#define     CHARTS_VERSION     "PFM Software - charts library V1.33 - 06/01/15"

#endif


/*

    Version 1.00
    08/02/03
    Jan C. Depner

    First version.


    Version 1.01
    04/08/08
    Jan C. Depner

    Added this file to track version changes.  Replaced %Ld formats with "%"PRIu64 so things work properly
    on Windows.


    Version 1.02
    04/25/08
    Jan C. Depner

    Fixed leap year error in charts_cvtime.c


    Version 1.03
    04/28/08
    Jan C. Depner

    Added tide status to dump routine.


    Version 1.04
    06/02/08
    Jan C. Depner

    Added hof_get_errors function.


    Version 1.05
    03/12/09
    Jan C. Depner

    Added depth check (< -998.0) to hof_uncertainty calc.


    Version 1.06
    05/27/09
    Jan C. Depner

    Added start_timestamp and end_timestamp to WAVE_HEADER_T structure.


    Version 1.07
    05/28/09
    Jan C. Depner

    Added file_size to image file header structure.


    Version 1.08
    06/08/09
    Jan C. Depner

    Removed LAS I/O.  Now using open source package liblas.


    Version 1.09
    07/29/09
    Jan C. Depner

    Added code to retrieve the ac_zero_level from the hardware header block of the wave (.inh) files.


    Version 1.10
    10/27/10
    Jan C. Depner

    Now reads the unknown shot data as NV_U_BYTE so we can try to figure out what it is.


    Version 1.11
    09/08/11
    Kevin M Byrd

    Added pos_get_timestamp() function to pos_io.c and FilePOSOutput.h and it returns a NV_INT64.
    It returns the time in microseconds from 1970 so that it may be used in conjunction with
    cvtime to convert to a year, julian day, hour, minute, second format.
    Also tweeked the WEEK_OFFSET defined variable to negate a bad conversion calculation for when
    the midnight flag is on and needing to calculate time_found.  Code altered where WEEK_OFFSET was
    defined and where "...checking for crossing midnight at end of GPS week..." code snippet is at.


    Version 1.12
    10/06/11
    Jan C. Depner

    Added 3 decimal places to seconds in dump_hof, dump_tof, and dump_wave.


    Version 1.20
    07/27/12
    Jan C. Depner

    Changed the write routines in hof_io.c and tof_io.c to pass the structures by reference instead of
    by value.  Hopefully, this will speed some things up a bit.


    Version 1.30
    09/07/12
    Jan C. Depner

    Modified the waveform reader to support FileVersion 1.5.


    Version 1.31
    05/07/14
    Jan C. Depner (PFM Software)

    Got rid of local definitions of DEG_TO_RAD and RAD_TO_DEG (now use NV_RAD_TO_DEG and NV_DEG_TO_RAD).


    Version 1.32
    Jan C. Depner (PFM Software)
    07/21/14

    Replaced nvtypes.h data types with stdint.h data types (e.g. uint32_t instead of NV_U_INT32).


    Version 1.33
    Jan C. Depner (PFM Software)
    06/01/15

    Apparently, when I replaced the nvtypes definitions I screwed up the endian checking - DOH!

*/
