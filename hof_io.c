
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
#include <math.h>
#include <errno.h>

#include "FileHydroOutput.h"
#include "hof_errors.h"

#ifndef NV_DEG_TO_RAD
  #define       NV_DEG_TO_RAD   0.017453293L
#endif

static uint8_t swap = 1;


static void charts_swap_hof_header (HOF_HEADER_T *head)
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


static void charts_swap_hof_record (HYDRO_OUTPUT_T *record)
{
  int16_t i;


  charts_swap_int64_t (&record->timestamp);
  charts_swap_int16_t (&record->haps_version);
  charts_swap_int16_t (&record->position_conf);
  charts_swap_double (&record->latitude);
  charts_swap_double (&record->longitude);
  charts_swap_double (&record->sec_latitude);
  charts_swap_double (&record->sec_longitude);
  charts_swap_float (&record->correct_depth);
  charts_swap_float (&record->correct_sec_depth);
  charts_swap_int16_t (&record->abdc);
  charts_swap_int16_t (&record->sec_abdc);

  for (i = 0 ; i < 2 ; i++) charts_swap_float (&record->future_use[i]);

  charts_swap_float (&record->tide_cor_depth);
  charts_swap_float (&record->reported_depth);
  charts_swap_float (&record->result_depth);
  charts_swap_float (&record->sec_depth);
  charts_swap_float (&record->wave_height);
  charts_swap_float (&record->elevation);
  charts_swap_float (&record->topo);
  charts_swap_float (&record->altitude);
  charts_swap_float (&record->kgps_elevation);
  charts_swap_float (&record->kgps_res_elev);
  charts_swap_float (&record->kgps_sec_elev);
  charts_swap_float (&record->kgps_topo);
  charts_swap_float (&record->kgps_datum);
  charts_swap_float (&record->kgps_water_level);
  charts_swap_float (&record->k);
  charts_swap_float (&record->intensity);
  charts_swap_float (&record->bot_conf);
  charts_swap_float (&record->sec_bot_conf);
  charts_swap_float (&record->nadir_angle);
  charts_swap_float (&record->scanner_azimuth);
  charts_swap_float (&record->sfc_fom_apd);
  charts_swap_float (&record->sfc_fom_ir);
  charts_swap_float (&record->sfc_fom_ram);
  charts_swap_float (&record->no_bottom_at);
  charts_swap_float (&record->no_bottom_at2);
  charts_swap_int32_t (&record->depth_conf);
  charts_swap_int32_t (&record->sec_depth_conf);
  charts_swap_int32_t (&record->warnings);
  charts_swap_int32_t (&record->warnings2);
  charts_swap_int32_t (&record->warnings3);

  for (i = 0 ; i < 2 ; i++) charts_swap_int16_t ((int16_t *) &record->calc_bfom_thresh_times10[i]);

  charts_swap_int16_t (&record->bot_bin_first);
  charts_swap_int16_t (&record->bot_bin_second);
  charts_swap_int16_t (&record->bot_bin_used_pmt);
  charts_swap_int16_t (&record->sec_bot_bin_used_pmt);
  charts_swap_int16_t (&record->bot_bin_used_apd);
  charts_swap_int16_t (&record->sec_bot_bin_used_apd);
}


FILE *open_hof_file (char *path)
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
          fflush (stderr);
        }
    }

  return (fp);
}


int32_t hof_read_header (FILE *fp, HOF_HEADER_T *head)
{
  int64_t      long_pos;
  char         varin[1024], info[1024];
  int16_t      tmpi16;


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

int32_t hof_read_record (FILE *fp, int32_t num, HYDRO_OUTPUT_T *record)
{
  int32_t ret;
  int64_t long_pos;


  if (!num)
    {
      fprintf (stderr, "Zero is not a valid HOF record number\n");
      fflush (stderr);
      return (0);
    }


  if (num != HOF_NEXT_RECORD)
    {
      fseeko64 (fp, (int64_t) HOF_HEAD_SIZE + (int64_t) (num - 1) * (int64_t) sizeof (HYDRO_OUTPUT_T), SEEK_SET);
    }
  else
    {
      long_pos = ftello64 (fp);
      if (long_pos < HOF_HEAD_SIZE) fseeko64 (fp, (int64_t) HOF_HEAD_SIZE, SEEK_SET);
    }


  ret = fread (record, sizeof (HYDRO_OUTPUT_T), 1, fp);


  if (swap) charts_swap_hof_record (record);


  return (ret);
}


int32_t hof_write_header (FILE *fp, HOF_HEADER_T head)
{
  fseeko64 (fp, 0LL, SEEK_SET);

  if (swap) charts_swap_hof_header (&head);

  fwrite (&head, sizeof (HOF_HEADER_T), 1, fp);

  return (0);
}


/*  Note that we're counting from 1 not 0.  Not my idea!  */

int32_t hof_write_record (FILE *fp, int32_t num, HYDRO_OUTPUT_T *record)
{
  int32_t ret;


  if (!num)
    {
      fprintf (stderr, "Zero is not a valid HOF record number\n");
      fflush (stderr);
      return (0);
    }


  if (num != HOF_NEXT_RECORD) fseeko64 (fp, (int64_t) HOF_HEAD_SIZE + (int64_t) (num - 1) * (int64_t) sizeof (HYDRO_OUTPUT_T), SEEK_SET);


  if (swap) charts_swap_hof_record (record);


  ret = fwrite (record, sizeof (HYDRO_OUTPUT_T), 1, fp);


  return (ret);
}



/*
    This function computes 95% confidence horizontal and vertical uncertainty values based on
    information from Paul LaRoque at Optech, Toronto (29 March 2005).  More information is
    available in the spreadsheet file, CHARTS_Hydro_Topo_Accuracies_29_Mar_2005.xls

    Author : Jan C. Depner
    Date : June 02, 2008
*/

void hof_get_uncertainty (HYDRO_OUTPUT_T *record, float *h_error, float *v_error, float in_depth, int32_t abdc)
{
  static double horizontal_errors[2] = {2.00, 0.15};
  static float prev_altitude = 400.0;

  float             altitude;
  double            depth;
  int32_t           type_index;

  double            rad_nadir, cos_rad_nadir, cos_rad_nadir_2, cos_rad_yaw, sqrt_2, tan_rad_nadir, rad_nadir_m5,
                    tan_rad_nadir_m5;
  double            height_error, roll_error, pitch_error, heading_error, scan_angle_error, antenna_error, 
                    h_calibration_error, laser_pointing_error, iho_error, propagation_error, beam_steering_error;
  double            total_aircraft_to_surface;
  double            total_platform_to_bottom;
  double            total_horizontal_error;

  double            depth_45, wave_beam_steering_error, propagation_induced_error;
  double            total_random_error;
  double            total_bias_error;


  if (in_depth <= -998.0) 
    {
      *h_error = 0.0;
      *v_error = 0.0;
      return;
    }


  if (record->altitude == 0.0)
    {
      altitude = prev_altitude;
    }
  else
    {
      altitude = record->altitude;
    }


  depth = (double) ((int32_t) -in_depth);
  type_index = (int32_t) record->data_type;


#ifdef CHARTS_DEBUG
  fprintf (stderr,"%s %s %d %d %f %f %f\n", __FILE__, __FUNCTION__, __LINE__, type_index, in_depth, altitude, record->nadir_angle);
#endif


  /*  Horizontal error pre-computed values  */

  rad_nadir = NV_DEG_TO_RAD * record->nadir_angle;
  tan_rad_nadir = tan (rad_nadir);
  rad_nadir_m5 = (record->nadir_angle - 5.0) * NV_DEG_TO_RAD;
  tan_rad_nadir_m5 = tan (rad_nadir_m5);
  cos_rad_nadir = cos (rad_nadir);
  cos_rad_nadir_2 = cos_rad_nadir * cos_rad_nadir;
  cos_rad_yaw = cos (NV_DEG_TO_RAD * E_YAW);
  sqrt_2 = 1.41421356237;


  /*  CHARTS System Errors  */

  height_error = E_HEIGHT * tan_rad_nadir;
  roll_error = altitude * E_ROLL / cos_rad_nadir_2 * NV_DEG_TO_RAD;
  pitch_error = altitude * E_PITCH / cos_rad_nadir_2 * NV_DEG_TO_RAD;
  heading_error = sqrt (2.0 * altitude * altitude * tan_rad_nadir * tan_rad_nadir * (1.0 - cos_rad_yaw));
  scan_angle_error = sqrt_2 * altitude * E_SCAN_ANGLE / cos_rad_nadir_2 * NV_DEG_TO_RAD;
  antenna_error = E_ANTENNA * sqrt_2;
  h_calibration_error = sqrt_2 * altitude * E_H_CALIBRATION / cos_rad_nadir_2 * NV_DEG_TO_RAD;
  laser_pointing_error = altitude * E_LASER_POINTING / cos_rad_nadir_2 * NV_DEG_TO_RAD;

  total_aircraft_to_surface = sqrt (height_error * height_error + roll_error * roll_error + pitch_error * pitch_error +
				    heading_error * heading_error + scan_angle_error * scan_angle_error +
				    antenna_error * antenna_error + h_calibration_error * h_calibration_error +
				    laser_pointing_error * laser_pointing_error);

#ifdef CHARTS_DEBUG
  fprintf (stderr,"%s %s %d %f %f %f %f %f %f %f %f %f\n", __FILE__, __FUNCTION__, __LINE__, height_error, roll_error, pitch_error,
	   heading_error, scan_angle_error, antenna_error, h_calibration_error, laser_pointing_error, total_aircraft_to_surface);
#endif


  /*  Water Parameter Errors  */

  iho_error = sqrt (0.25 + (0.013 * depth) * (0.013 * depth)) / 1.96 * tan_rad_nadir_m5;
  propagation_error = E_PROPAGATION * depth;
  beam_steering_error = (depth * 0.45 / 100.0) * tan_rad_nadir_m5;

  total_platform_to_bottom = sqrt (total_aircraft_to_surface * total_aircraft_to_surface + iho_error * iho_error +
				   propagation_error * propagation_error + beam_steering_error * beam_steering_error);

#ifdef CHARTS_DEBUG
  fprintf (stderr,"%s %s %d %f %f %f %f\n", __FILE__, __FUNCTION__, __LINE__, iho_error, propagation_error, beam_steering_error,
	   total_platform_to_bottom);
#endif


  /*  Total Horizontal Error  */

  total_horizontal_error = sqrt (total_platform_to_bottom * total_platform_to_bottom + 
				 horizontal_errors[type_index] * horizontal_errors[type_index]);


  /*  95% Confidence Horizontal Error  */

  *h_error = (float) (total_horizontal_error * 1.96);


  /*  Land values have no vertical error estimate  */

  if (abdc == 70 || abdc == 13)
    {
      *v_error = 0.0;
    }
  else
    {
      /*  Vertical error pre-computed values  */

      depth_45 = 0.45 * depth;


      /*  Vertical random errors  */

      wave_beam_steering_error = depth * 0.45 / 100.0;

      total_random_error = sqrt (E_ALTIMETER_TIM_2 + E_CFD_2 + E_LOG_AMP_DELAY_2 + E_WAVE_HEIGHT_2 +
				 E_PULSE_LOCATION_2 + (E_ELLIPSOID_TO_LASER_KGPS_2 * (double) type_index) +
				 wave_beam_steering_error * wave_beam_steering_error);

#ifdef CHARTS_DEBUG
      fprintf (stderr,"%s %s %d %f %f\n", __FILE__, __FUNCTION__, __LINE__, wave_beam_steering_error, total_random_error);
#endif


      /*  Vertical Bias Errors  */

      propagation_induced_error = sqrt (36.0 + depth_45 * depth_45) / 100.0;

      total_bias_error = sqrt (E_THERMAL_2 + E_V_CALIBRATION_2 + E_PMT_DELAY_2 + E_SURFACE_ORIGIN_2 +
			       propagation_induced_error * propagation_induced_error);

#ifdef CHARTS_DEBUG
      fprintf (stderr,"%s %s %d %f %f\n", __FILE__, __FUNCTION__, __LINE__, propagation_induced_error, total_bias_error);
#endif


      /*  95% Confidence Vertical Error  */

      *v_error = (float) (fabs (total_bias_error - E_CONSTANT_BIAS) + total_random_error * 1.96);
    }
}



void hof_dump_record (HYDRO_OUTPUT_T *record)
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
  printf ("haps version : %d\n", record->haps_version);
  printf ("position confidence : %d\n", record->position_conf);
  printf ("status : %x\n", record->status);
  printf ("suggested delete/keep/swap : %d\n", record->suggested_dks);
  printf ("suspect status : %x\n", record->suspect_status);
  printf ("tide status : %x\n", record->tide_status & 0x3);
  printf ("latitude : %f\n", record->latitude);
  printf ("longitude : %f\n", record->longitude);
  printf ("secondary latitude : %f\n", record->sec_latitude);
  printf ("secondary longitude : %f\n", record->sec_longitude);
  printf ("correct depth : %f\n", record->correct_depth);
  printf ("correct secondary depth : %f\n", record->correct_sec_depth);
  printf ("abbreviated depth confidence : %d\n", record->abdc);
  printf ("secondary abbreviated depth confidence : %d\n", record->sec_abdc);
  printf ("data type : %d\n", record->data_type);
  printf ("tide corrected depth : %f\n", record->tide_cor_depth);
  printf ("reported depth : %f\n", record->reported_depth);
  printf ("result depth : %f\n", record->result_depth);
  printf ("secondary depth : %f\n", record->sec_depth);
  printf ("wave height : %f\n", record->wave_height);
  printf ("elevation : %f\n", record->elevation);
  printf ("topo : %f\n", record->topo);
  printf ("altitude : %f\n", record->altitude);
  printf ("kgps elevation : %f\n", record->kgps_elevation);
  printf ("kgps result elevation : %f\n", record->kgps_res_elev);
  printf ("kgps secondary elevation : %f\n", record->kgps_sec_elev);
  printf ("kgps topo : %f\n", record->kgps_topo);
  printf ("kgps datum : %f\n", record->kgps_datum);
  printf ("kgps water level : %f\n", record->kgps_water_level);
  printf ("bottom confidence : %f\n", record->bot_conf);
  printf ("secondary bottom confidence : %f\n", record->sec_bot_conf);
  printf ("nadir angle : %f\n", record->nadir_angle);
  printf ("scanner azimuth : %f\n", record->scanner_azimuth);
  printf ("surface figure of merit apd : %f\n", record->sfc_fom_apd);
  printf ("surface figure of merit ir : %f\n", record->sfc_fom_ir);
  printf ("surface figure of merit raman : %f\n", record->sfc_fom_ram);
  printf ("depth confidence : %d\n", record->depth_conf);
  printf ("secondary depth confidence : %d\n", record->sec_depth_conf);
  printf ("warnings : %d\n", record->warnings);
  printf ("warnings2 : %d\n", record->warnings2);
  printf ("warnings3 : %d\n", record->warnings3);
  printf ("calc_bfom_thresh_times10[0] : %d\n", record->calc_bfom_thresh_times10[0]);
  printf ("calc_bfom_thresh_times10[1] : %d\n", record->calc_bfom_thresh_times10[1]);
  printf ("calc_bot_run_required[0] : %d\n", record->calc_bot_run_required[0]);
  printf ("calc_bot_run_required[1] : %d\n", record->calc_bot_run_required[1]);
  printf ("bot_bin_first : %d\n", record->bot_bin_first    );
  printf ("bot_bin_second : %d\n", record->bot_bin_second    );
  printf ("bot_bin_used_pmt : %d\n", record->bot_bin_used_pmt    );
  printf ("sec_bot_bin_used_pmt : %d\n", record->sec_bot_bin_used_pmt    );
  printf ("bot_bin_used_apd : %d\n", record->bot_bin_used_apd    );
  printf ("sec_bot_bin_used_apd : %d\n", record->sec_bot_bin_used_apd);
  printf ("bot_channel : %d\n", record->bot_channel        );
  printf ("sec_bot_chan : %d\n", record->sec_bot_chan    );
  printf ("sfc_bin_apd : %d\n", record->sfc_bin_apd        );
  printf ("sfc_bin_ir : %d\n", record->sfc_bin_ir            );
  printf ("sfc_bin_ram : %d\n", record->sfc_bin_ram        );
  printf ("sfc_channel_used : %d\n", record->sfc_channel_used    );
  printf ("ab_dep_conf : %d\n", record->ab_dep_conf);
  printf ("sec_ab_dep_conf : %d\n", record->sec_ab_dep_conf);
  printf ("kgps_abd_conf : %d\n", record->kgps_abd_conf);
  printf ("kgps_sec_abd_conf : %d\n", record->kgps_sec_abd_conf);

  fflush (stdout);
}
