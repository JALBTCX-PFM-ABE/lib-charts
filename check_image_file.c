
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

#include "FileImage.h"

/*  dump_image_file  */


int32_t main (int32_t argc, char *argv[])
{
  FILE                *fp, *jpg_fp;
  char                fn[512], jpg_name[512];
  int32_t             i, num_recs;
  uint32_t            prev_size = -1;
  int64_t             prev_time = -1;
  IMAGE_HEADER_T      head;
  IMAGE_INDEX_T       image_index;


  if (argc < 2)
    {
      fprintf (stderr, "\n\nUsage: check_image_file IMG_FILENAME\n\n");
      exit (-1);
    }

  if ((fp = open_image_file (argv[1])) == NULL)
    {
      perror (argv[1]);
      exit (-1);
    }


  fprintf (stderr, "%s\n", argv[1]);


  image_read_header (fp, &head);

  num_recs = head.text.number_images;


  for (i = 0 ; i < num_recs ; i++)
    {
      image_get_metadata (fp, i + 1, &image_index);

      if (i)
        {
          if (image_index.timestamp - prev_time == 2000000)
            fprintf (stderr, "2 second gap following record %d\n", i);

          if (image_index.image_size == prev_size)
            fprintf (stderr, "Duplicate images at records %d and %d - %d %d\n", i, i + 1, image_index.image_size, prev_size);
        }

      prev_time = image_index.timestamp;
      prev_size = image_index.image_size;
    }

  fclose (fp);

  return (0);
}
