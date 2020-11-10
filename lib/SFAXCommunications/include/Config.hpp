// typedef struct
// {
//     short ccd_number;  // variable JY  toujours z�ro
//     int port;
//     int total_active_x_pixels;
//     int total_active_y_pixels;
//     int num_serial_pxls_before_active;
//     int num_serial_pxls_after_active;
//     int num_parallel_rows_before_active;
//     int num_parallel_rows_after_active;
//     int register_loc_and_direction;        // 0, 90, 180, 270 degrees

//     int min_temperature;
//     int max_temperature;
//     long min_shutter_time;
//     long max_shutter_time;
//     char min_gain;
//     char max_gain;
//     int h_pixel_spacing;
//     int v_pixel_spacing;
//     int total_parallel_pixels;      // includes all possible pixels
//     int total_serial_pixels;        // includes all possible pixels

//     int offset0;
//     int offset4;
//     int tip0;
//     int tip4;
// } ccd_tsr_struct;