
/*****************************************************************************
 * FileImage.h     Header
 *
 * Purpose:        This header file hold the definition of the image file 
 *                 structure 
 *
 * Revision History:
 * 02/07/15 DR    -wrote it
 *
 * 02/09/17 DR    -added FileImageOpen_PATH for opening with passed filename...
 *
 * 08/21/03 JD    -modified for NAVO data types
 *
 ****************************************************************************/

#ifndef HOF_TOF_Image
#define HOF_TOF_Image

#ifdef  __cplusplus
extern "C" {
#endif


#include "charts.h"


#define IMAGE_HEAD_TEXT_BLK_SIZE    (8 * 1024)
#define IMAGE_HEAD_BIN_BLK_SIZE     (8 * 1024)
#define IMAGE_HEAD_BIN_OFFSET       (IMAGE_HEAD_TEXT_BLK_SIZE)

#define IMAGE_HEAD_SIZE   (IMAGE_HEAD_TEXT_BLK_SIZE + IMAGE_HEAD_BIN_BLK_SIZE)



typedef struct
{
  int64_t     timestamp;            /* timestamp                           */
  int64_t     byte_offset;          /* offset in data area                 */
  int32_t     image_size;           /* size in bytes of jpeg               */
  int32_t     image_number;         /* image number in line                */
} OLD_IMAGE_INDEX_T;

typedef struct
{
  int64_t     timestamp;            /* timestamp                           */
  int64_t     byte_offset;          /* offset in data area                 */
  int32_t     image_size;           /* size in bytes of jpeg               */
  int32_t     image_number;         /* image number in line                */
  char        fill[160];
} IMAGE_INDEX_T;

typedef struct
{
  char        file_type[128];
  uint8_t     endian;

  float       software_version;
  float       file_version;

  int32_t     header_size;
  int32_t     text_block_size;
  int32_t     bin_block_size;

  char        UserName[64];     /* Who processed this (whole) line last */

  char        project[64];
  char        mission[64];
  char        dataset[64];
  char        flightline_number[64];
  int16_t     coded_fl_number;
  char        flight_date[64];
  char        start_time[64];
  char        end_time[64];
  int64_t     start_timestamp;  /* time in microseconds from 01/01/1970 */
  int64_t     end_timestamp;    /* time in microseconds from 01/01/1970 */
  int32_t      number_images;
  char        dataset_create_date[64];
  char        dataset_create_time[64];

  int32_t     record_size;      /* index record size  */
  int32_t     block_size;       /* index block size  */
  int32_t     data_size;        /* size of data block (images) */
} IMAGE_TEXT_T;


typedef struct
{
  int32_t     ImageHeaderSize;      /* Includes all text, info and index   */
                                    /* (offset to data)                    */
                                    /* (ImageTextHeaderSize + IndexSize)   */
  int32_t     ImageTextHeaderSize;  /* IMAGE_HEADER_SIZE                   */
  int32_t     IndexSize;            /* includes index info and index       */
                                    /* (sizeof(IMAGE_INDEX_INFO_T) +       */
                                    /* (sizeof(IMAGE_INDEX_T) *            */
                                    /* image_count)                        */
  int32_t     ImageIndexEntrySize;  /* sizeof(IMAGE_INDEX_T)               */
  int32_t     number_images;
  int32_t     fill_int;

  int64_t     start_timestamp;    
  int64_t     end_timestamp;        

  uint16_t    flightline;    
  uint16_t    fill_short;
  int32_t     fill_int2[15];        /* Future use...                       */
} IMAGE_INFO_T;


typedef struct
{
  /*  8k Text info block */

  IMAGE_TEXT_T text;
  char         text_block[(IMAGE_HEAD_TEXT_BLK_SIZE) - sizeof(IMAGE_TEXT_T)];


  /* 8k Binary block */

  IMAGE_INFO_T info;
  char         bin_block[(IMAGE_HEAD_BIN_BLK_SIZE) - sizeof(IMAGE_INFO_T)];
} IMAGE_HEADER_T;


  int32_t image_read_header (FILE *fp, IMAGE_HEADER_T *head);
  FILE *open_image_file (char *path);
  int32_t image_get_metadata (FILE *fp, int32_t rec_num, IMAGE_INDEX_T *image_index);
  int32_t image_find_record (FILE *fp, int64_t timestamp);
  uint8_t *image_read_record (FILE *fp, int64_t timestamp, uint32_t *size, int64_t *image_time);
  uint8_t *image_read_record_recnum (FILE *fp, int32_t recnum, uint32_t *size, int64_t *image_time);
  int64_t dump_image (char *file, int64_t timestamp, char *path);


#ifdef  __cplusplus
}
#endif


#endif
