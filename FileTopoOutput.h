
/*****************************************************************************
 * FileTopoOutput.h         Header
 *
 * Purpose:    This header file hold the definition of the topographic inputs 
 *             structure for use with the the inputs file.
 *              
 *
 * Revision History:
 *
 * 02/07/29 DR  -cleanup from SHOALSNT
 *
 * 02/08/15 DR  -continued implimentation as per DS/AP DD 
 *
 * 02/09/05 DR  -added HAPS back to header...
 *
 * 02/10/31 DR  -added new define for status as a byte of bits (for 
 *               deleted/kept,swapped) instead of a short.  
 *              -killed flag was removed (use bit in status)
 *              -defines for bits added 
 * 
 * 03/06/09 DR  -added username to header...
 *
 * 03/06/13 DR  -redefined TOPO_OUTPUT structure.. Removed Flightline, 
 *               Taps version, reduced intensity to char (now 64 bytes/record)
 *
 * 03/06/27 DR  -changed 1 to first and 2 to second in some names...
 *
 * 08/02/03 JD  -modified for NAVO data types
 *
 * 04/08/04 JD  -added text header info as per GCS_file_formats.pdf
 *
 ****************************************************************************/

#ifndef TOFHead
#define TOFHead


#ifdef  __cplusplus
extern "C" {
#endif


#include "charts.h"


/* Full header will be a 16k block. */


/*

Full Output header is 256k:

Header Text        8k -has WAY more than I can use but keep extra for future
Binary             8k -Binary - date, start timestamp, stop timestamp, planning info (points).... will also contain most 
                   of the items in the "text" block in BIN form

*/


#define  TOF_HEAD_TEXT_BLK_SIZE        (8 * 1024)
#define  TOF_HEAD_BIN_BLK_SIZE         (8 * 1024)
#define  TOF_HEAD_BIN_OFFSET           (TOF_HEAD_TEXT_BLK_SIZE)


#define  TOF_HEAD_SIZE                 (TOF_HEAD_TEXT_BLK_SIZE +  TOF_HEAD_BIN_BLK_SIZE)


#define  TOF_NEXT_RECORD               (-1)


typedef struct
{
  int64_t        start_time;       /* time in microseconds from 01/01/1970 */
  int64_t        stop_time;        /* time in microseconds from 01/01/1970 */
  int64_t        file_create_time; /* timestamp when file was created...   */
  int64_t        file_mod_time;    /* timestamp when file was last         */
                                   /* modified **TBR**                     */

  double         min_lat;
  double         min_long;
  double         max_lat;
  double         max_long;

  int64_t        fill_i64[6];

  int32_t        num_shots;
  float          gcs_version;
  float          gcs_file_version;

  int32_t        header_size;
  int32_t        text_block_size;
  int32_t        binary_block_size;
  int32_t        record_size;

  int32_t        ab_system_type;
  int32_t        ab_system_number;
  int32_t        rep_rate;

  int16_t        data_type;        /* 0 = normal (DGPS), 1 = kinematic GPS */
  int16_t        fill_short;

  int32_t        fill_i32[11];

  uint16_t       flightline;       /* Line number is flightline/10 and     */
                                   /* version is remainder (ie. 13 is      */
                                   /* 1-3)                                 */
  int16_t        fill_i16[7];
    
  char           UserName[64];     /* Who processed this (whole) line last */

  char           TBD[960];

  char           project[128];
  char           mission[128];
  char           dataset[128];
} TOF_INFO_T;


typedef struct
{
  char           file_type[128];
  uint8_t        endian;

  float          software_version;
  float          file_version;

  int32_t        header_size;
  int32_t        text_block_size;
  int32_t        bin_block_size;
  int32_t        record_size;

  int8_t         ab_system_type;
  int8_t         ab_system_number;
  int16_t        system_rep_rate;

  char           UserName[64];     /* Who processed this (whole) line last */

  char           project[64];
  char           mission[64];
  char           dataset[64];
  char           flightline_number[64];
  int16_t        coded_fl_number;
  char           flight_date[64];
  char           start_time[64];
  char           end_time[64];
  int64_t        start_timestamp;  /* time in microseconds from 01/01/1970 */
  int64_t        end_timestamp;    /* time in microseconds from 01/01/1970 */
  int32_t        number_shots;
  char           dataset_create_date[64];
  char           dataset_create_time[64];
  double         line_min_lat;
  double         line_max_lat;
  double         line_min_lon;
  double         line_max_lon;
} TOF_TEXT_T;


typedef struct
{
  /*  8k Text info block */

  TOF_TEXT_T     text;
  char           text_block[(TOF_HEAD_TEXT_BLK_SIZE) - sizeof(TOF_TEXT_T)];


  /* 8k Flightline block */

  TOF_INFO_T     info;
  char           bin_block[(TOF_HEAD_BIN_BLK_SIZE) - sizeof(TOF_INFO_T)];
} TOF_HEADER_T;


typedef struct
{
  int64_t        timestamp;

  double         latitude_first;
  double         longitude_first;
  double         latitude_last;
  double         longitude_last;

  float          elevation_first;        
  float          elevation_last;

  float          scanner_azimuth;
  float          nadir_angle;

  char           conf_first;  /*  Below 50 is bad  */
  char           conf_last;   /*  Below 50 is bad  */

  uint8_t        intensity_first;        
  uint8_t        intensity_last;        


  /*  The classification_status field is now (04/29/2016) based on the ASPRS LAS 1.4 spec and the 2013
      ASPRS paper - "LAS Domain Profile Description: Topo-Bathy Lidar", July 17, 2013.

      0   -  Created, never classified
      1   -  Unclassified
      2   -  Ground
      3   -  Low Vegetation
      4   -  Medium Vegetation
      5   -  High Vegetation
      6   -  Building
      7   -  Low Point (noise)
      9   -  Water
      10  -  Rail
      11  -  Road Surface
      12  -  Overlap Points (use overlap bit for LAS 1.4)
      13  -  Wire - Guard (Shield)
      14  -  Wire - Conductor (phase)
      15  -  Transmission Tower
      16  -  Wire-structure Connector (e.g. Insulator)
      17  -  Bridge Deck
      18  -  High Noise
      40  -  Bathymetric point (e.g. seafloor or riverbed; also known as submerged topography)
      41  -  Water surface (sea/river/lake surface from bathymetric or topographic-bathymetric lidar;
             distinct from point class 9 which is used in topographic only lidar and only designates
             "water", not "water surface")
      42  -  Derived water surface (synthetic water surface location used in computing refraction at water surface)
      43  -  Submerged object, not otherwise specified (e.g. wreck, rock, submerged piling)
      44  -  International Hydrographic Organization (IHO) S-57 object, not otherwise specified
      45  -  No-bottom-found-at (bathymetric lidar point for which no detectable bottom return was received)

  */

  uint8_t        classification_status;

  uint8_t        tbd_1;

  char           pos_conf;
  char           tbd_2;

  float          result_elevation_first;
  float          result_elevation_last;
  float          altitude;
  float          tbdfloat;
} TOPO_OUTPUT_T;


  FILE *open_tof_file (char *path);
  int32_t tof_read_header (FILE *fp, TOF_HEADER_T *head);
  int32_t tof_read_record (FILE *fp, int32_t num, TOPO_OUTPUT_T *record);
  int32_t tof_write_header (FILE *fp, TOF_HEADER_T head);
  int32_t tof_write_record (FILE *fp, int32_t num, TOPO_OUTPUT_T *record);
  void tof_dump_record (TOPO_OUTPUT_T *record);


#ifdef  __cplusplus
}
#endif

#endif
