
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "FileWave.h"

static uint8_t             swap, first = 1;
static WAVE_HEADER_T       l_head;

/***************************************************************************\
*                                                                           *
*   Module Name:        lidar_get_string                                    *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       December 1994                                       *
*                                                                           *
*   Purpose:            Parses the input string for the : and returns       *
*                       everything to the right.                            *
*                                                                           *
*   Arguments:          *in     -   Input string                            *
*                       *out    -   Output string                           *
*                                                                           *
*   Return Value:       None                                                *
*                                                                           *
*   Calling Routines:   envin                                               *
*                                                                           * 
*   Caveats:            THIS FUNCTION DOES NOT HANDLE NEWLINE OR CARRIAGE   *
*                       RETURN!  If you are reading a line from a file to   *
*                       pass to this function you must either use ngets     *
*                       instead of fgets or strip the \n and, on Windows,   *
*                       \r from the line.                                   *
*                                                                           *          
\***************************************************************************/

void lidar_get_string (char *in, char *out)
{
  int32_t       i, start, length;

  start = 0;
  length = 0;
    
  strcpy (out, (char *) (strchr (in, ':') + 1));

  /*  Search for first non-blank character.   */
    
  for (i = 0 ; i < strlen (out) ; i++)
    {
      if (out[i] != ' ')
        {
          start = i;
          break;
        }
    }

  /*  Search for last non-blank character.    */
    
  for (i = strlen (out) ; i >= 0 ; i--)
    {
      if (out[i] != ' ' && out[i] != 0)
        {
          length = (i + 1) - start;
          break;
        }
    }

  strncpy (out, &out[start], length);
  out[length] = 0;
}



int32_t wave_read_header (FILE *fp, WAVE_HEADER_T *head)
{
  int64_t     long_pos;
  char        varin[1024], info[1024];
  int16_t     tmpi16;


  char *ngets (char *s, int32_t size, FILE *stream);


  fseeko64 (fp, 0LL, SEEK_SET);


  /*  Read each entry.    */

  head->header_size = 0;
  while (ngets (varin, sizeof (varin), fp) != NULL)
    {
      if (!strcmp (varin, "EOF")) break;


      /*  Put everything to the right of the colon into 'info'.   */
            
      if (strchr (varin, ':') != NULL) strcpy (info, (char *) (strchr (varin, ':') + 1));


      /*  Check input for matching strings and load values if found.  */
            
      if (strstr (varin, "FileType:") != NULL) lidar_get_string (varin, head->file_type);

      if (strstr (varin, "EndianType:") != NULL) if (strstr (info, "Little")) head->endian = 1;

      if (strstr (varin, "SoftwareVersionNumber:") != NULL) sscanf (info, "%f", &head->software_version);

      if (strstr (varin, "FileVersionNumber:") != NULL) sscanf (info, "%f", &head->file_version);


      if (strstr (varin, "HeaderSize:") != NULL) sscanf (info, "%d", &head->header_size);

      if (strstr (varin, "TextBlockSize:") != NULL) sscanf (info, "%d", &head->text_block_size);

      if (strstr (varin, "BinaryBlockSize:") != NULL) sscanf (info, "%d", &head->bin_block_size);

      if (strstr (varin, "HardwareBlockSize:") != NULL) sscanf (info, "%d", &head->hardware_block_size);

      if (strstr (varin, "HapsBlockSize:") != NULL) sscanf (info, "%d", &head->haps_block_size);

      if (strstr (varin, "OtherBlockSize:") != NULL) sscanf (info, "%d", &head->other_block_size);

      if (strstr (varin, "RecordSize:") != NULL) sscanf (info, "%hd", &head->record_size);

      if (strstr (varin, "ShotDataSize:") != NULL) sscanf (info, "%hd", &head->shot_data_size);

      if (strstr (varin, "WaveformSize:") != NULL) sscanf (info, "%hd", &head->wave_form_size);

      if (strstr (varin, "DeepWaveSize:") != NULL) sscanf (info, "%hd", &head->pmt_size);

      if (strstr (varin, "ShallowWaveSize:") != NULL) sscanf (info, "%hd", &head->apd_size);

      if (strstr (varin, "IRWaveSize:") != NULL) sscanf (info, "%hd", &head->ir_size);

      if (strstr (varin, "RamanWaveSize:") != NULL) sscanf (info, "%hd", &head->raman_size);

      if (strstr (varin, "ABSystemType:") != NULL)
        {
          sscanf (info, "%hd", &tmpi16);
          head->ab_system_type = (int8_t) tmpi16;
        }

      if (strstr (varin, "ABSystemNumber:") != NULL)
        {
          sscanf (info, "%hd", &tmpi16);
          head->ab_system_number = (int8_t) tmpi16;
        }

      if (strstr (varin, "SystemRepRate:") != NULL) sscanf (info, "%hd", &head->system_rep_rate);


      if (strstr (varin, "Project:") != NULL) lidar_get_string (varin, head->project);

      if (strstr (varin, "Mission:") != NULL) lidar_get_string (varin, head->mission);

      if (strstr (varin, "Dataset:") != NULL) lidar_get_string (varin, head->dataset);

      if (strstr (varin, "FlightlineNumber:") != NULL) lidar_get_string (varin, head->flightline_number);

      if (strstr (varin, "CodedFLNumber:") != NULL) sscanf (info, "%hd", &head->coded_fl_number);

      if (strstr (varin, "FlightDate:") != NULL) lidar_get_string (varin, head->flight_date);

      if (strstr (varin, "StartTime:") != NULL) lidar_get_string (varin, head->start_time);

      if (strstr (varin, "EndTime:") != NULL) lidar_get_string (varin, head->end_time);

      if (strstr (varin, "StartTimestamp:") != NULL) sscanf (info, "%"PRIu64, &head->start_timestamp);

      if (strstr (varin, "EndTimestamp:") != NULL) sscanf (info, "%"PRIu64, &head->end_timestamp);

      if (strstr (varin, "NumberShots:") != NULL) sscanf (info, "%d", &head->number_shots);

      if (strstr (varin, "DatasetCreateDate:") != NULL) lidar_get_string (varin, head->dataset_create_date);

      if (strstr (varin, "DatasetCreateTime:") != NULL) lidar_get_string (varin, head->dataset_create_time);

      long_pos = ftello64 (fp);
      if (head->header_size && long_pos >= head->header_size) break;
    }


  /*  Get the ac_zero_offset data.  */

  long_pos = head->text_block_size + head->bin_block_size + 572;
  fseeko64 (fp, long_pos, SEEK_SET);

  fread (&head->ac_zero_offset, sizeof (uint16_t), 4, fp);


  /*  Move past the entire header block.  */

  fseeko64 (fp, head->header_size, SEEK_SET);

  return (0);
}


FILE *open_wave_file (char *path)
{
  FILE *fp;

  int32_t big_endian ();


  swap = (uint8_t) big_endian ();
  first = 1;

  if ((fp = fopen64 (path, "rb")) == NULL)
    {
      perror (path);
    }
  else
    {
      wave_read_header (fp, &l_head);


      /*  Subtract the timestamp size from the shot data size since we're actually going to read the timestamp.  */

      l_head.shot_data_size -= sizeof (int64_t);


      /*  As of GCS file FileVersion 1.5 the shot data size is misreported in the ASCII header.  It may be right in the 
          binary portion of the header but I don't have that format info.  The record size is correct but there are 8 
          empty bytes on the end of the record NOT in the shot data.  DOH!  */

      if (l_head.file_version > 1.4) l_head.shot_data_size -= 8;
    }


  return (fp);
}


/*  Note that we're counting from 1 not 0.  Not my idea!  */

/*  RIDICULOUSLY IMPORTANT NOTE:  Make sure that you static "record" in the calling routine since we are allocating the
    memory for the waveforms here!  DOH!!!  */

int32_t wave_read_record (FILE *fp, int32_t num, WAVE_DATA_T *record)
{
  int32_t ret;
  int64_t long_pos;


  if (!num)
    {
      fprintf (stderr, "Zero is not a valid INH record number\n");
      fflush (stderr);
      return (0);
    }


  if (num != WAVE_NEXT_RECORD)
    {
      fseeko64 (fp, (int64_t) l_head.header_size + (int64_t) (num - 1) * (int64_t) l_head.record_size, SEEK_SET);
    }
  else
    {
      long_pos = ftello64 (fp);
      if (long_pos < l_head.header_size) fseeko64 (fp, (int64_t) l_head.header_size, SEEK_SET);
    }


  if (first)
    {
      record->shot_data = (uint8_t *) calloc (l_head.shot_data_size, sizeof (uint8_t));
      record->pmt = (uint8_t *) calloc (l_head.pmt_size, sizeof (uint8_t));
      record->apd = (uint8_t *) calloc (l_head.apd_size, sizeof (uint8_t));
      record->ir = (uint8_t *) calloc (l_head.ir_size, sizeof (uint8_t));
      record->raman = (uint8_t *) calloc (l_head.raman_size, sizeof (uint8_t));

      if (record->raman == NULL)
        {
          perror ("Allocating wave memory");
          exit (-1);
        }
      first = 1;
    }


  /*  Read the timestamp.  */

  fread (&record->timestamp, sizeof (int64_t), 1, fp);

  if (swap) charts_swap_int64_t (&record->timestamp);


  /*  Read the shot data (Optech proprietary info that we don't care about).  */

  ret = fread (record->shot_data, l_head.shot_data_size, 1, fp);


  /*  Read the waveform data.  */

  ret = fread (record->pmt, l_head.pmt_size, 1, fp);
  ret = fread (record->apd, l_head.apd_size, 1, fp);
  ret = fread (record->ir, l_head.ir_size, 1, fp);
  ret = fread (record->raman, l_head.raman_size, 1, fp);


  return (ret);
}


void wave_dump_record (WAVE_DATA_T record)
{
  int32_t         i, j, start, end, year, day, hour, minute, month, mday;
  float           second;


  charts_cvtime (record.timestamp, &year, &day, &hour, &minute, &second);
  charts_jday2mday (year, day, &month, &mday);
  month++;

  printf ("*****************************\n");
  printf ("timestamp : ");
  printf ("%"PRIu64, record.timestamp);
  printf ("    %d-%02d-%02d (%d) %02d:%02d:%08.5f\n", year + 1900, month, mday, day, hour, minute, second);


  printf ("\n*****************  PMT waveform values  *****************\n");

  for (i = 0 ; i < l_head.pmt_size ; i += 10)
    {
      if (i >= l_head.pmt_size) break;

      start = i;
      end = MIN (i + 10, l_head.pmt_size);

      printf ("%04d-%04d : ", start, end - 1);

      for (j = start ; j < end ; j++) printf ("%05d ", record.pmt[j]);

      printf ("\n");
    }

  printf ("\n");

  printf ("\n*****************  APD waveform values  *****************\n");

  for (i = 0 ; i < l_head.apd_size ; i += 10)
    {
      if (i >= l_head.apd_size) break;

      start = i;
      end = MIN (i + 10, l_head.apd_size);

      printf ("%04d-%04d : ", start, end - 1);

      for (j = start ; j < end ; j++) printf ("%05d ", record.apd[j]);

      printf ("\n");
    }

  printf ("\n");

  printf ("\n*****************  IR waveform values  *****************\n");

  for (i = 0 ; i < l_head.ir_size ; i += 10)
    {
      if (i >= l_head.ir_size) break;

      start = i;
      end = MIN (i + 10, l_head.ir_size);

      printf ("%04d-%04d : ", start, end - 1);

      for (j = start ; j < end ; j++) printf ("%05d ", record.ir[j]);

      printf ("\n");
    }

  printf ("\n");

  printf ("\n*****************  RAMAN waveform values  *****************\n");

  for (i = 0 ; i < l_head.raman_size ; i += 10)
    {
      if (i >= l_head.raman_size) break;

      start = i;
      end = MIN (i + 10, l_head.raman_size);

      printf ("%04d-%04d : ", start, end - 1);

      for (j = start ; j < end ; j++) printf ("%05d ", record.raman[j]);

      printf ("\n");
    }

  printf ("\n");

  fflush (stdout);
}
