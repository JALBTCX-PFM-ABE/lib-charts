
/*****************************************************************************
 * FileHydroOutput.h     Header
 *
 * Purpose:    This header file hold the definition of the hydrographic inputs 
 *             structure for use with the the inputs file.
 *          
 *
 * Revision History:
 *
 * 03/07/02 DR    -added DKS defs..  Were hard coded as 0x01/2/4/8/10 in the 
 *                                   code before.
 *
 * 08/02/03 JD    -modified for NAVO data types
 *
 * 04/08/04 JD    -added text header info as per GCS_file_formats.pdf
 *
 ****************************************************************************/

#ifndef HOFHead
#define HOFHead

#ifdef  __cplusplus
extern "C" {
#endif


#include "charts.h"


/* Full header will be a 16k block.  */

/* 
   Full Output header is 256k:

   Header Text    8k -has WAY more than I can use but keep extra for future
   Binary         8k -Binary - date, start/stop timestamp, planning info (points).... will also contain most of the 
                  items in the "text" block in BIN form
*/

#define    HOF_HEAD_TEXT_BLK_SIZE    (8 * 1024)
#define    HOF_HEAD_BIN_BLK_SIZE     (8 * 1024)
#define    HOF_HEAD_BIN_OFFSET       (HOF_HEAD_TEXT_BLK_SIZE)

#define    HOF_HEAD_SIZE             (HOF_HEAD_TEXT_BLK_SIZE +  HOF_HEAD_BIN_BLK_SIZE)


#define    HOF_NEXT_RECORD           (-1)


/*  DEFINES FOR BITS IN STATUS BYTE IN OUTPUTS (below)...  */

#define    AU_STATUS_DELETED_BIT   0x01        /* B0  */
#define    AU_STATUS_KEPT_BIT      0x02        /* B1  */
#define    AU_STATUS_SWAPPED_BIT   0x04        /* B2  */

/*
    DKS will use the above bit defs (for DELETED/KEPT/SWAPPED) in addition to 
    the bits listed below...
*/

#define    AU_DKS_TOPO_BIT         0x08        /* B3  */
#define    AU_DKS_SLOPE_CHANGE_BIT 0x10        /* B4  */


/*
  DEFINE BITS IN "suspect_status" (in struture below...)
  None  = Not an suspect/feature/outlier
  B0 = Suspect/Anomaly Flag (set in Fledermaus by operator and/or by AU)
  B1 = Feature/Target Flag (set in Fledermaus by operator and/or by AU) 
  B2 = AU detected Flier/outlier flag (set by AU) 
  B3 = AU detected suspect/anomaly (set by AU) [FUTURE USE IN TRACING]
  B4 = AU detected Feature/Target (set by AU) [NOT USED YET] [FUTURE USE IN 
       TRACING]
*/

#define  SUSPECT_STATUS_SUSPECT_BIT    0x01    /*  B0 = suspect/anomaly [FLEDERMAUS CAN CHANGE]  */
#define  SUSPECT_STATUS_FEATURE_BIT    0x02    /*  B1 = feature/target [FLEDERMAUS CAN CHANGE]  */
#define  SUSPECT_STATUS_OUTLIER_BIT    0x04    /*  B2 = outlier/flier  */
#define  SUSPECT_STATUS_AU_SUSPECT_BIT 0x08    /*  B3 = AU suspect/anomaly  */
#define  SUSPECT_STATUS_AU_FEATURE_BIT 0x10    /*  B4 = AU feature/target  */


#define  PMT               0
#define  APD               1
#define  IR                2
#define  RAMAN             3


typedef struct
{
  int64_t        start_time;       /* time in microseconds from 01/01/1970 */
  int64_t        stop_time;        /* time in microseconds from 01/01/1970 */
  int64_t        file_create_time; /* timestamp when file was created...   */
  int64_t        file_mod_time;    /* timestamp when file was last modified **TBR**  */

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

  int16_t        data_type;        /* 0 = Normal (DGPS), 1 = kinematic (KGPS)  */
  int16_t        fill_short;

  int32_t        fill_i32[11];

  uint16_t       flightline;       /* Line number is flightline/10 and version is remainder (ie. 13 is 1-3)  */
  int16_t        fill_i16[7];
    
  char           UserName[64];     /* Who processed this (whole) line last */
  char           TBD[960];

  char           project[128];
  char           mission[128];
  char           dataset[128];
} HOF_INFO_T;


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
} HOF_TEXT_T;


typedef struct
{
  /*  8k Text info block */

  HOF_TEXT_T     text;
  char           text_block[(HOF_HEAD_TEXT_BLK_SIZE) - sizeof(HOF_TEXT_T)];


  /* 8k Flightline block */

  HOF_INFO_T     info;
  char           bin_block[(HOF_HEAD_BIN_BLK_SIZE) - sizeof(HOF_INFO_T)];
} HOF_HEADER_T;


typedef struct
{
  int64_t        timestamp;        /* time in microseconds from 01/01/1970 */
  int16_t        haps_version;
  int16_t        position_conf;


  /*
    STATUS DEF (SEE ABOVE FOR BIT SETTINGS)
        bit 0 = deleted    (1); 
        bit 1 = kept       (2); 
        bit 2 = swapped    (4); 
        rest TBD. 
    Only one of kept or swapped may be used at once and they can only be used for shots with a second depth; killed can be used for any shots) 
  */

  char           status;
    

  /*
    Suggested Delete, Keep, Swap value from AU (Theodore's algorithm; SEE ABOVE FOR BIT SETTINGS))
  */

  char           suggested_dks;    
    

  /*
    SUSPECT STATUS (SEE ABOVE FOR BIT SETTINGS)
  */

  char           suspect_status;
  char           tide_status;          /* Lower two bits:                  */
                                       /* 0 - no tide corection applied.   */
                                       /* 1 - predicted tide applied.      */
                                       /* 2 - preliminary tide applied.    */
                                       /* 3 - verified tide applied.       */
                                       /* Other 6 bits are TBD.            */
  double         latitude;
  double         longitude;
  double         sec_latitude;
  double         sec_longitude;


  /*
    The following four fields are redundant data that can be used for the loading of the data into the GCS software (as well as Fledermaus...).  It
    is processing mode independent and also specifies the processing mode...
  */

  /*
    IMPORTANT NOTE:  correct_depth and tide_cor_depth are actually elevations.  That is, they are negative depths.
  */

  float          correct_depth;       /* Copied from "tide_cor_depth" or "kgps_elevation", UNLESS we did a Shoreline Depth Swap (AKA topo 
                                         depth swap - abdc = 72), this point was "Shallow Water Processed" (abdc = 74), or this is land 
                                         data (abdc 70) in which case it's copied from kgps_topo (with datum shift applied if needed).  */
  float          correct_sec_depth;   /* Copied from "sec_depth" or "kgps_sec_elev"  */
  int16_t        abdc;                /* from either "ab_dep_conf" or "kgps_abd_conf": 13 - land, unknown elevation < 70 - no good,
                                         >= 70 - good, 70 - land, known elevation, 72 - shoreline depth, 74 - shallow water algorithm.
                                         If this value is over 100 we have added 100 to the original value to override GCS making a point
                                         invalid.  If the value is less than -100 we have manually invalidated a point that was automatically
                                         invalidated by GCS (less than 70) and was manually validated  by a user.*/
  int16_t        sec_abdc;            /* Same as above  */
  char           data_type;           /* 0 = normal (DGPS), 1 = kinematic GPS  */
  char           land_mode;           /* Attempt at land detection  */


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

  char           tbd[1];


  /* Likely to be used for "CUBE" integration.... */

  float          future_use[2];


  /* Traditional mode info... */

  float          tide_cor_depth;
  float          reported_depth;
  float          result_depth;
  float          sec_depth;
  float          wave_height;
  float          elevation;
  float          topo;
  float          altitude;
    

  /* KGPS processing mode info */

  float          kgps_elevation;
  float          kgps_res_elev;
  float          kgps_sec_elev;
  float          kgps_topo;
  float          kgps_datum;
  float          kgps_water_level;
    

  float          k;                   /* K Value TBD      */
  float          intensity;           /* Intensity or bottom reflectance TBD      */


  /*  IMPORTANT NOTE : The terms first and second in variable names are used to denote primary and secondary returns.  That is, those returns
      that are accepted as true (primary) and those that are deemed incorrect (secondary).  These do not correspond to first return and second
      return.  To find first return you need to check to see which of bot_bin_first (primary) and bot_bin_second (secondary) has the smallest 
      number (ie, the least number of bins from the surface.  */


  float          bot_conf;            /* bottom signal strength for channel of first depth  */
  float          sec_bot_conf;        /* bottom signal strength for channel of second depth  */
  float          nadir_angle;         /* degrees off nadir (usually around 20 degrees)  */
  float          scanner_azimuth;     /* Variable angle (- = left side, + = right side; +- 60)  */
  float          sfc_fom_apd;         /* surface signal strength for GAPD channel  */
  float          sfc_fom_ir;          /* surface signal strength for IR channel  */
  float          sfc_fom_ram;         /* surface signal strength for Raman channel  */


  /* Future items... [Reserve space for them now], No Bottom At   [***TBD***]  */

  float          no_bottom_at;
  float          no_bottom_at2;

 

  /* Conf (both modes same) */

  int32_t        depth_conf;          /* Coded confidence; internal use.. */
  int32_t        sec_depth_conf;      /* Coded confidence; internal use.. */

  int32_t        warnings;            /* Coded bit warnings...            */
  int32_t        warnings2;           /* Coded bit warnings...            */
  int32_t        warnings3;           /* Coded bit warnings...[Future use for algorithm]  */

  /* New Algorithm stuff... */

  uint16_t       calc_bfom_thresh_times10[2]; /* MK's Calculated BFOM thresh times by 10;      */
  char           calc_bot_run_required[2];    /* MK's calculated HAP      */


  char           tbd2[2];
    

  /* Bottom/surface stuff (early calcs in AUTO) */

  int16_t        bot_bin_first;       /* Bin of the primary return (not necessarily first return) */
  int16_t        bot_bin_second;      /* Bin of the secondary return (not necessarily second return) */
                                      /* First return has smallest bin number */
  int16_t        bot_bin_used_pmt;    /* These get mixed around if the primary and secondary are on */ 
  int16_t        sec_bot_bin_used_pmt;/* different channels  */     
  int16_t        bot_bin_used_apd;    /* ditto */ 
  int16_t        sec_bot_bin_used_apd;/* ditto */

  uint8_t        bot_channel;         /* Channel of the primary return (see bot_bin_first) */
  uint8_t        sec_bot_chan;        /* Channel of the secondary return (see bot_bin_second) */
  uint8_t        sfc_bin_apd;         
  uint8_t        sfc_bin_ir;             
  uint8_t        sfc_bin_ram;         
  uint8_t        sfc_channel_used;     


  /* Abbreviated confidences (each mode has one for each depth/elevation) */

  char           ab_dep_conf;
  char           sec_ab_dep_conf;
  char           kgps_abd_conf;
  char           kgps_sec_abd_conf;

  char           tbd3[2];
}  HYDRO_OUTPUT_T;                        /* Whole output file definition... */


FILE *open_hof_file (char *path);
int32_t hof_read_header (FILE *fp, HOF_HEADER_T *head);
int32_t hof_read_record (FILE *fp, int32_t num, HYDRO_OUTPUT_T *record);
int32_t hof_write_header (FILE *fp, HOF_HEADER_T head);
int32_t hof_write_record (FILE *fp, int32_t num, HYDRO_OUTPUT_T *record);
void hof_get_uncertainty (HYDRO_OUTPUT_T *record, float *h_error, float *v_error, float in_depth, int32_t abdc);
void hof_dump_record (HYDRO_OUTPUT_T *record);


#ifdef  __cplusplus
}
#endif


#endif
