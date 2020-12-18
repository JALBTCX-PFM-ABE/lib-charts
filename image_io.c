
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
#include <errno.h>
#include <string.h>

#include "FileImage.h"

static uint8_t swap = 1;
static IMAGE_HEADER_T l_head;
static IMAGE_INDEX_T *records = NULL;
static int32_t old = 0;


static void charts_swap_image_header (IMAGE_INFO_T *info)
{
  charts_swap_int32_t (&info->ImageHeaderSize);
  charts_swap_int32_t (&info->ImageTextHeaderSize);
  charts_swap_int32_t (&info->IndexSize);
  charts_swap_int32_t (&info->ImageIndexEntrySize);
  charts_swap_int32_t (&info->number_images);

  charts_swap_int64_t (&info->start_timestamp);
  charts_swap_int64_t (&info->end_timestamp);

  charts_swap_int16_t ((int16_t *) &info->flightline);
}


int32_t image_read_header (FILE *fp, IMAGE_HEADER_T *head)
{
  int32_t      ret;
  int64_t      long_pos;
  char         varin[1024], info[1024];

  char *ngets (char *s, int32_t size, FILE *stream);
  int32_t big_endian ();


  swap = 0;


  /*  Check for the new format file.  If the first four characters are 
      "File" it's the new format, otherwise read the binary header.  */

  fseeko64 (fp, 0LL, SEEK_SET);
  ngets (varin, sizeof (varin), fp);

  if (!strncmp (varin, "File", 4))
    {
      fseeko64 (fp, 0LL, SEEK_SET);


      /*  Read each entry.    */

      head->text.header_size = 0;
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


          if (strstr (varin, "Downloaded By:") != NULL) lidar_get_string (varin, head->text.UserName);


          if (strstr (varin, "HeaderSize:") != NULL) sscanf (info, "%d", &head->text.header_size);

          if (strstr (varin, "TextBlockSize:") != NULL) sscanf (info, "%d", &head->text.text_block_size);

          if (strstr (varin, "BinaryBlockSize:") != NULL) sscanf (info, "%d", &head->text.bin_block_size);


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

          if (strstr (varin, "NumberImages:") != NULL) sscanf (info, "%d", &head->text.number_images);

          if (strstr (varin, "IndexRecordSize:") != NULL) sscanf (info, "%d", &head->text.record_size);

          if (strstr (varin, "IndexBlockSize:") != NULL) sscanf (info, "%d", &head->text.block_size);

          if (strstr (varin, "FileCreateDate:") != NULL) lidar_get_string (varin, head->text.dataset_create_date);

          if (strstr (varin, "FileCreateTime:") != NULL) lidar_get_string (varin, head->text.dataset_create_time);

          long_pos = ftello64 (fp);
          if (head->text.header_size && long_pos >= head->text.header_size) break;
        }


      /*  Make sure we're past the header.  */

      fseeko64 (fp, (int64_t) head->text.header_size, SEEK_SET);

      ret = 0;
    }
  else
    {
      /*  Early versions were screwed up.  */

      fseeko64 (fp, (int64_t) IMAGE_HEAD_SIZE, SEEK_SET);

      fread (&head->info, sizeof (IMAGE_INFO_T), 1, fp);


      /*  Swap the INFO block if needed.  */

      if (swap) charts_swap_image_header (&head->info);


      head->text.start_timestamp = head->info.start_timestamp;
      head->text.end_timestamp = head->info.end_timestamp;
      head->text.number_images = head->info.number_images;

      ret = 1;
    }


  head->text.data_size = l_head.text.data_size;


  return (ret);
}



FILE *open_image_file (char *path)
{
  FILE                  *fp;
  int32_t               i;
  OLD_IMAGE_INDEX_T     old_record;


  int32_t big_endian ();


  swap = (uint8_t) big_endian ();


  l_head.text.data_size = 0;


  /*  Brute force!  Load the index into memory, it ain't big anyway.  */

  if ((fp = fopen64 (path, "rb")) == NULL)
    {
      perror (path);
    }
  else
    {
      old = image_read_header (fp, &l_head);

      if (records) free (records);

      records = (IMAGE_INDEX_T *) calloc (l_head.text.number_images, sizeof (IMAGE_INDEX_T));

      if (records == NULL)
        {
          perror ("Allocating image index");
          exit (-1);
        }

      if (old)
        {
          for (i = 0 ; i < l_head.text.number_images ; i++)
            {
              fread (&old_record, sizeof (OLD_IMAGE_INDEX_T), 1, fp);
                
              if (swap)
                {
                  charts_swap_int64_t (&old_record.timestamp);
                  charts_swap_int64_t (&old_record.byte_offset);

                  charts_swap_int32_t (&old_record.image_size);
                  charts_swap_int32_t (&old_record.image_number);
                }
              records[i].timestamp = old_record.timestamp;
              records[i].byte_offset = old_record.byte_offset;
              records[i].image_size = old_record.image_size;
              records[i].image_number = old_record.image_number;

              l_head.text.data_size += records[i].image_size;
            }
        }
      else
        {
          for (i = 0 ; i < l_head.text.number_images ; i++)
            {
              fread (&records[i], sizeof (IMAGE_INDEX_T), 1, fp);

              if (swap)
                {
                  charts_swap_int64_t (&records[i].timestamp);
                  charts_swap_int64_t (&records[i].byte_offset);

                  charts_swap_int32_t (&records[i].image_size);
                  charts_swap_int32_t (&records[i].image_number);
                }

              l_head.text.data_size += records[i].image_size;
            }
        }
    }

  return (fp);
}


/*  Note that we're counting from 1 not 0.  Not my idea!  */

int32_t image_get_metadata (FILE *fp, int32_t rec_num, IMAGE_INDEX_T *image_index)
{
  int32_t     real_num;


  real_num = rec_num - 1;

  if (real_num < 0 || real_num > l_head.text.number_images) return (-1);

  image_index->timestamp = records[real_num].timestamp;
  image_index->byte_offset = records[real_num].byte_offset;
  image_index->image_size = records[real_num].image_size;
  image_index->image_number = records[real_num].image_number;

  return (0);
}


/*  Returns the nearest record number to "timestamp".  Note that we're counting
    from 1 not 0.  Not my idea!  */

int32_t image_find_record (FILE *fp, int64_t timestamp)
{
  int32_t        i, j;


  if (timestamp < l_head.text.start_timestamp || timestamp > l_head.text.end_timestamp) return (0);

  j = -1;
  for (i = 0 ; i < l_head.text.number_images ; i++)
    {
      if (timestamp <= records[i].timestamp)
        {
          if (!i)
            {
              j = i;
            }
          else
            {
              if ((timestamp - records[i - 1].timestamp) < (records[i].timestamp - timestamp))
                {
                  j = i - 1;
                }
              else
                {
                  j = i;
                }
            }
          break;
        }
    }

  return (j + 1);
}



/*  This function tries to find the record based on the timestamp.  Returns NULL on failure or the 
    image if it succeeds.  You must free the image in the calling program.  */

uint8_t *image_read_record (FILE *fp, int64_t timestamp, uint32_t *size, int64_t *image_time)
{
  uint8_t          *image;
  int32_t          i, j;


  if (timestamp < l_head.text.start_timestamp || timestamp > l_head.text.end_timestamp) return (NULL);

  j = -1;
  for (i = 0 ; i < l_head.text.number_images ; i++)
    {
      if (timestamp <= records[i].timestamp)
        {
          if (!i)
            {
              j = i;
            }
          else
            {
              if ((timestamp - records[i - 1].timestamp) < (records[i].timestamp - timestamp))
                {
                  j = i - 1;
                }
              else
                {
                  j = i;
                }
            }
          break;
        }
    }


  /*  Make sure we got an image.  */

  if (j == -1 || records[j].image_size == 0) return ((uint8_t *) NULL);


  *image_time = records[j].timestamp;


  image = (uint8_t *) malloc (records[j].image_size);
  if (image == NULL)
    {
      perror ("Allocating image memory");
      exit (-1);
    }


  fseeko64 (fp, records[j].byte_offset, SEEK_SET);
  fread (image, records[j].image_size, 1, fp);

  *size = records[j].image_size;

  return (image);
}


/*  This call reads the image at record "recnum".  Returns NULL on failure or the 
    image if it succeeds.  You must free the image in the calling program.  */

uint8_t *image_read_record_recnum (FILE *fp, int32_t recnum, uint32_t *size, int64_t *image_time)
{
  uint8_t         *image;
  int32_t         j;


  if (recnum < 1 || recnum > l_head.text.number_images) return (NULL);

  j = recnum - 1;


  if (records[j].image_size == 0) return (NULL);


  *image_time = records[j].timestamp;

  image = (uint8_t *) malloc (records[j].image_size);
  if (image == NULL)
    {
      perror ("Allocating image memory");
      exit (-1);
    }


  fseeko64 (fp, records[j].byte_offset, SEEK_SET);
  fread (image, records[j].image_size, 1, fp);

  *size = records[j].image_size;

  return (image);
}


/*  Given the .hof or .tof filename this function will write a .jpg file
    from the .img file of the same name.  The .jpg will be the image nearest
    to the timestamp entered and the filename will be in the argument
    'path'.  The file must be in the same directory as the .hof or .tof
    file.  This returns the timestamp of the picture or 0 (failed).  */

int64_t dump_image (char *file, int64_t timestamp, char *path)
{
  uint8_t      *image;
  char         img_file[512];
  uint32_t     size;
  FILE         *lfp, *dfp;
  int64_t      ret = 0;


  strcpy (img_file, file);
  strcpy (&img_file[strlen (img_file) - 4], ".img");

  if ((lfp = open_image_file (img_file)) != NULL)
    {
      image = image_read_record (lfp, timestamp, &size, &ret);

      if (size)
        {
          if (image)
            {
              if ((dfp = fopen64 (path, "wb")) != NULL)
                {
                  fwrite (image, size, 1, dfp);

                  fclose (dfp);

                  free (image);
                }
            }

          fclose (lfp);
        }
    }

  return (ret);
}
