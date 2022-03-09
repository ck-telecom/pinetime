#pragma once

#include "draw_command.h"

/**
 * TYPEDEFS
 */
typedef n_GDrawCommandImage             GDrawCommandImage;
typedef n_GDrawCommand                  GDrawCommand;
typedef n_GDrawCommandFrame             GDrawCommandFrame;

/**
 * ENUMS
 */
typedef n_GDrawCommandType GDrawCommandType;

/**
 * FUNCTIONS
 */
#define gdraw_command_draw		                      n_gdraw_command_draw
#define gdraw_command_get_type                                n_gdraw_command_get_type
#define gdraw_command_set_fill_color                          n_gdraw_command_set_fill_color
#define gdraw_command_get_fill_color                          n_gdraw_command_get_fill_color
#define gdraw_command_set_stroke_color                        n_gdraw_command_set_stroke_color
#define gdraw_command_get_stroke_color                        n_gdraw_command_get_stroke_color
#define gdraw_command_set_stroke_width                        n_gdraw_command_set_stroke_width
#define gdraw_command_get_stroke_width                        n_gdraw_command_get_stroke_width
#define gdraw_command_get_num_points                          n_gdraw_command_get_num_points
#define gdraw_command_set_point                               n_gdraw_command_set_point
#define gdraw_command_get_point                               n_gdraw_command_get_point
#define gdraw_command_set_radius                              n_gdraw_command_set_radius
#define gdraw_command_get_radius                              n_gdraw_command_get_radius
#define gdraw_command_set_path_open                           n_gdraw_command_set_path_open
#define gdraw_command_get_path_open                           n_gdraw_command_get_path_open
#define gdraw_command_set_hidden                              n_gdraw_command_set_hidden
#define gdraw_command_get_hidden                              n_gdraw_command_get_hidden
#define gdraw_command_frame_draw                              n_gdraw_command_frame_draw
#define gdraw_command_frame_set_duration                      n_gdraw_command_frame_set_duration
#define gdraw_command_frame_get_duration                      n_gdraw_command_frame_get_duration
#define gdraw_command_image_create_with_resource              n_gdraw_command_image_create_with_resource
#define gdraw_command_image_clone                             n_gdraw_command_image_clone
#define gdraw_command_image_destroy                           n_gdraw_command_image_destroy
#define gdraw_command_image_draw                              n_gdraw_command_image_draw
#define gdraw_command_image_get_bounds_size                   n_gdraw_command_image_get_bounds_size
#define gdraw_command_image_set_bounds_size                   n_gdraw_command_image_set_bounds_size
#define gdraw_command_image_get_command_list                  n_gdraw_command_image_get_command_list
#define gdraw_command_list_iterate                            n_gdraw_command_list_iterate
#define gdraw_command_list_draw                               n_gdraw_command_list_draw
#define gdraw_command_list_get_command                        n_gdraw_command_list_get_command
#define gdraw_command_list_get_num_commands                   n_gdraw_command_list_get_num_commands
#define gdraw_command_sequence_create_with_resource           n_gdraw_command_sequence_create_with_resource
#define gdraw_command_sequence_clone                          n_gdraw_command_sequence_clone
#define gdraw_command_sequence_destroy                        n_gdraw_command_sequence_destroy
#define gdraw_command_sequence_get_frame_by_elapsed           n_gdraw_command_sequence_get_frame_by_elapsed
#define gdraw_command_sequence_get_frame_by_index             n_gdraw_command_sequence_get_frame_by_index
#define gdraw_command_sequence_get_bounds_size                n_gdraw_command_sequence_get_bounds_size
#define gdraw_command_sequence_set_bounds_size                n_gdraw_command_sequence_set_bounds_size
#define gdraw_command_sequence_get_play_count                 n_gdraw_command_sequence_get_play_count
#define gdraw_command_sequence_set_play_count                 n_gdraw_command_sequence_set_play_count
#define gdraw_command_sequence_get_total_duration             n_gdraw_command_sequence_get_total_duration
#define gdraw_command_sequence_get_num_frames                 n_gdraw_command_sequence_get_num_frames
#define gdraw_command_frame_get_command_list                  n_gdraw_command_frame_get_command_list