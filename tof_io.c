
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
#include <string.h>
#include <errno.h>

#include "FileTopoOutput.h"

static uint8_t swap = 1;


static void charts_swap_tof_header (TOF_HEADER_T *head)
{
  int16_t i;


  charts_swap_int64_t (&head->info.start_time);
  charts_swap_int64_t (&head->info.stop_time);
  charts_swap_int64_t (&head->info.file_create_time);
  charts_swap_int64_t (&head->info.file_mod_time);

  charts_swap_double (&head->info.min_lat);
  charts_swap_double (&head->info.min_long);
  charts_swap_double (&head->info.max_lat);
  charts_swap_double (&head->info.max_long);

  for (i = 0 ; i < 6 ; i++) charts_swap_int64_t (&head->info.fill_i64[i]);

  charts_swap_int32_t (&head->info.num_shots);
  charts_swap_float (&head->info.gcs_version);
  charts_swap_float (&head->info.gcs_file_version);

  charts_swap_int32_t (&head->info.header_size);
  charts_swap_int32_t (&head->info.text_block_size);
  charts_swap_int32_t (&head->info.binary_block_size);
  charts_swap_int32_t (&head->info.record_size);

  charts_swap_int32_t (&head->info.ab_system_type);
  charts_swap_int32_t (&head->info.ab_system_number);
  charts_swap_int32_t (&head->info.rep_rate);

  charts_swap_int16_t (&head->info.data_type);
  charts_swap_int16_t (&head->info.fill_short);

  for (i = 0 ; i < 11 ; i++) charts_swap_int32_t (&head->info.fill_i32[i]);

  charts_swap_int16_t ((int16_t *) &head->info.flightline);

  for (i = 0 ; i < 7 ; i++) charts_swap_int16_t (&head->info.fill_i16[i]);
}


static void charts_swap_tof_record (TOPO_OUTPUT_T *record)
{
  charts_swap_int64_t (&record->timestamp);
  charts_swap_double (&record->latitude_first);
  charts_swap_double (&record->longitude_first);
  charts_swap_double (&record->latitude_last);
  charts_swap_double (&record->longitude_last);
  charts_swap_float (&record->elevation_first);
  charts_swap_float (&record->elevation_last);
  charts_swap_float (&record->scanner_azimuth);
  charts_swap_float (&record->nadir_angle);
  charts_swap_float (&record->result_elevation_first);
  charts_swap_float (&record->result_elevation_last);
  charts_swap_float (&record->altitude);
  charts_swap_float (&record->tbdfloat);
}


FILE *open_tof_file (char *path)
{
  FILE *fp;


  if ((fp = fopen64 (path, "rb+")) == NULL)
    {
      perror (path);
      if ((fp = fopen64 (path, "rb")) == NULL)
        {
          perror (path);
        }
      else
        {
          fprintf (stderr, "%s opened for read only!\n", path);
        }
      fflush (stderr);
    }

  return (fp);
}


int32_t tof_read_header (FILE *fp, TOF_HEADER_T *head)
{
  int64_t     long_pos;
  char        varin[1024], info[1024];
  int16_t     tmpi16;


  char *ngets (char *s, int32_t size, FILE *stream);
  int32_t big_endian ();


  swap = 0;


  fseeko64 (fp, 0LL, SEEK_SET);



  /*  Read each entry.    */

  head->text.header_size = 0;
  head->text.line_min_lat = 0.0;
  head->text.line_max_lat = 0.0;
  head->text.line_min_lon = 0.0;
  head->text.line_max_lon = 0.0;
  while (ngets (varin, sizeof (varin), fp) != NULL)
    {
      if (!strcmp (varin, "EOF")) break;


      /*  Put everything to the right of the colon into 'info'.   */
            
      if (strchr (varin, ':') != NULL) strcpy (info, (char *) (strchr (varin, ':') + 1));


      /*  Check input for matching strings and load values if found.  */
            
      if (strstr (varin, "FileType:") != NULL) lidar_get_string (varin, head->text.file_type);

      if (strstr (varin, "EndianType:") != NULL)
        {
          if (strstr (info, "Little")) 
            {
              head->text.endian = 1;
              if (big_endian ()) swap = 1;
            }
          else
            {
              head->text.endian = 1;
              if (!big_endian ()) swap = 1;
            }
        }


      if (strstr (varin, "SoftwareVersionNumber:") != NULL) sscanf (info, "%f", &head->text.software_version);

      if (strstr (varin, "FileVersionNumber:") != NULL) sscanf (info, "%f", &head->text.file_version);


      if (strstr (varin, "Processed By:") != NULL) lidar_get_string (varin, head->text.UserName);


      if (strstr (varin, "HeaderSize:") != NULL) sscanf (info, "%d", &head->text.header_size);

      if (strstr (varin, "TextBlockSize:") != NULL) sscanf (info, "%d", &head->text.text_block_size);

      if (strstr (varin, "BinaryBlockSize:") != NULL) sscanf (info, "%d", &head->text.bin_block_size);

      if (strstr (varin, "RecordSize:") != NULL) sscanf (info, "%d", &head->text.record_size);


      if (strstr (varin, "ABSystemType:") != NULL)
        {
          sscanf (info, "%hd", &tmpi16);
          head->text.ab_system_type = (int8_t) tmpi16;
        }

      if (strstr (varin, "ABSystemNumber:") != NULL)
        {
          sscanf (info, "%hd", &tmpi16);
          head->text.ab_system_number = (int8_t) tmpi16;
        }

      if (strstr (varin, "SystemRepRate:") != NULL) sscanf (info, "%hd", &head->text.system_rep_rate);


      if (strstr (varin, "Project:") != NULL) lidar_get_string (varin, head->text.project);

      if (strstr (varin, "Mission:") != NULL) lidar_get_string (varin, head->text.mission);

      if (strstr (varin, "Dataset:") != NULL) lidar_get_string (varin, head->text.dataset);

      if (strstr (varin, "FlightlineNumber:") != NULL) lidar_get_string (varin, head->text.flightline_number);

      if (strstr (varin, "CodedFLNumber:") != NULL) sscanf (info, "%hd", &head->text.coded_fl_number);

      if (strstr (varin, "FlightDate:") != NULL) lidar_get_string (varin, head->text.flight_date);

      if (strstr (varin, "StartTime:") != NULL) lidar_get_string (varin, head->text.start_time);

      if (strstr (varin, "EndTime:") != NULL) lidar_get_string (varin, head->text.end_time);

      if (strstr (varin, "StartTimestamp:") != NULL) sscanf (info, "%"PRIu64, &head->text.start_timestamp);

      if (strstr (varin, "EndTimestamp:") != NULL) sscanf (info, "%"PRIu64, &head->text.end_timestamp);

      if (strstr (varin, "NumberShots:") != NULL) sscanf (info, "%d", &head->text.number_shots);

      if (strstr (varin, "FileCreateDate:") != NULL) lidar_get_string (varin, head->text.dataset_create_date);

      if (strstr (varin, "FileCreateTime:") != NULL) lidar_get_string (varin, head->text.dataset_create_time);

      if (strstr (varin, "LineMinLat:") != NULL) sscanf (info, "%lf", &head->text.line_min_lat);

      if (strstr (varin, "LineMaxLat:") != NULL) sscanf (info, "%lf", &head->text.line_max_lat);

      if (strstr (varin, "LineMinLong:") != NULL) sscanf (info, "%lf", &head->text.line_min_lon);

      if (strstr (varin, "LineMaxLong:") != NULL) sscanf (info, "%lf", &head->text.line_max_lon);


      long_pos = ftello64 (fp);
      if (head->text.header_size && long_pos >= head->text.header_size) break;
    }


  /*  Make sure we're past the header.  */

  fseeko64 (fp, (int64_t) head->text.header_size, SEEK_SET);


  return (0);
}


/*  Note that we're counting from 1 not 0.  Not my idea!  */

int32_t tof_read_record (FILE *fp, int32_t num, TOPO_OUTPUT_T *record)
{
  int32_t ret;
  int64_t long_pos;


  if (!num)
    {
      fprintf (stderr, "Zero is not a valid TOF record number\n");
      fflush (stderr);
      return (0);
    }


  if (num != TOF_NEXT_RECORD)
    {
      fseeko64 (fp, (int64_t) TOF_HEAD_SIZE + (int64_t) (num - 1) * (int64_t) sizeof (TOPO_OUTPUT_T), SEEK_SET);
    }
  else
    {
      long_pos = ftello64 (fp);
      if (long_pos < TOF_HEAD_SIZE) fseeko64 (fp, (int64_t) TOF_HEAD_SIZE, SEEK_SET);
    }


  ret = fread (record, sizeof (TOPO_OUTPUT_T), 1, fp);


  if (swap) charts_swap_tof_record (record);


  return (ret);
}


int32_t tof_write_header (FILE *fp, TOF_HEADER_T head)
{
  fseeko64 (fp, 0LL, SEEK_SET);

  if (swap) charts_swap_tof_header (&head);

  fwrite (&head, sizeof (TOF_HEADER_T), 1, fp);

  return (0);
}


/*  Note that we're counting from 1 not 0.  Not my idea!  */

int32_t tof_write_record (FILE *fp, int32_t num, TOPO_OUTPUT_T *record)
{
  int32_t ret;


  if (!num)
    {
      fprintf (stderr, "Zero is not a valid TOF record number\n");
      fflush (stderr);
      return (0);
    }


  if (num != TOF_NEXT_RECORD) fseeko64 (fp, (int64_t) TOF_HEAD_SIZE + (int64_t) (num - 1) * (int64_t) sizeof (TOPO_OUTPUT_T), SEEK_SET);


  if (swap) charts_swap_tof_record (record);


  ret = fwrite (record, sizeof (TOPO_OUTPUT_T), 1, fp);


  return (ret);
}


void tof_dump_record (TOPO_OUTPUT_T *record)
{
  int32_t         year, day, hour, minute, month, mday;
  float           second;


  charts_cvtime (record->timestamp, &year, &day, &hour, &minute, &second);
  charts_jday2mday (year, day, &month, &mday);
  month++;

  printf ("*****************************\n");
  printf ("timestamp : ");
  printf ("%"PRIu64, record->timestamp);
  printf ("    %d-%02d-%02d (%d) %02d:%02d:%08.5f\n", year + 1900, month, mday, day, hour, minute, second);
  printf ("latitude first : %f\n", record->latitude_first);
  printf ("longitude first : %f\n", record->longitude_first);
  printf ("latitude last : %f\n", record->latitude_last);
  printf ("longitude last : %f\n", record->longitude_last);
  printf ("elevation first : %f\n", record->elevation_first);
  printf ("elevation last : %f\n", record->elevation_last);
  printf ("scanner azimuth : %f\n", record->scanner_azimuth);
  printf ("nadir angle : %f\n", record->nadir_angle);
  printf ("confidence first : %d\n", record->conf_first);
  printf ("confidence last : %d\n", record->conf_last);
  printf ("intensity first : %hd\n", (int16_t) record->intensity_first);
  printf ("intensity last : %hd\n", (int16_t) record->intensity_last);
  printf ("classification status : %x\n", record->classification_status);
  printf ("TBD 1 : %x\n", record->tbd_1);
  printf ("position conf : %d\n", record->pos_conf);
  printf ("TBD 2 : %x\n", record->tbd_2);
  printf ("result elevation first : %f\n", record->result_elevation_first);
  printf ("result elevation last : %f\n", record->result_elevation_last);
  printf ("altitude : %f\n", record->altitude);
  printf ("TBD float : %f\n", record->tbdfloat);
}
