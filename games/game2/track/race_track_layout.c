#include "race_track_layout.h"

#include <stddef.h>

static const RaceTrackScreen g_track_screens[] = {
    {
        .tile_type = RACE_TRACK_STRAIGHT_VERTICAL,
        .sector = RACE_SECTOR_1,
        .corner_name = "START STRAIGHT",
        .entry_edge = RACE_EDGE_BOTTOM,
        .exit_edge = RACE_EDGE_TOP,
        .spawn_x = 114.0f,
        .spawn_y = 205.0f,
        .spawn_heading_deg = 0.0f,
        .road_rects = {
            {80, 28, 80, 212},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        .road_rect_count = 1,
        .curb_side = RACE_CURB_NONE,
        .curb_rect = {0, 0, 0, 0},
        .has_start_finish = true
    },

    {
        .tile_type = RACE_TRACK_TURN_UP_RIGHT,
        .sector = RACE_SECTOR_1,
        .corner_name = "T1 RIGHT 90",
        .entry_edge = RACE_EDGE_BOTTOM,
        .exit_edge = RACE_EDGE_RIGHT,
        .spawn_x = 114.0f,
        .spawn_y = 205.0f,
        .spawn_heading_deg = 0.0f,
        .road_rects = {
            {70, 88, 100, 152},
            {70, 68, 170, 100},
            {70, 68, 100, 100}
        },
        .road_rect_count = 3,
        .curb_side = RACE_CURB_RIGHT,
        .curb_rect = {165, 74, 56, 10},
        .has_start_finish = false
    },

    {
        .tile_type = RACE_TRACK_STRAIGHT_HORIZONTAL,
        .sector = RACE_SECTOR_1,
        .corner_name = "EAST STRAIGHT",
        .entry_edge = RACE_EDGE_LEFT,
        .exit_edge = RACE_EDGE_RIGHT,
        .spawn_x = 6.0f,
        .spawn_y = 105.0f,
        .spawn_heading_deg = 90.0f,
        .road_rects = {
            {0, 80, 240, 80},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        .road_rect_count = 1,
        .curb_side = RACE_CURB_NONE,
        .curb_rect = {0, 0, 0, 0},
        .has_start_finish = false
    },

    {
        .tile_type = RACE_TRACK_TURN_LEFT_UP,
        .sector = RACE_SECTOR_1,
        .corner_name = "T2 LEFT 90",
        .entry_edge = RACE_EDGE_LEFT,
        .exit_edge = RACE_EDGE_TOP,
        .spawn_x = 6.0f,
        .spawn_y = 105.0f,
        .spawn_heading_deg = 90.0f,
        .road_rects = {
            {0, 82, 132, 82},
            {92, 28, 84, 136},
            {92, 82, 84, 82}
        },
        .road_rect_count = 3,
        .curb_side = RACE_CURB_LEFT,
        .curb_rect = {92, 64, 12, 58},
        .has_start_finish = false
    },

    {
        .tile_type = RACE_TRACK_STRAIGHT_VERTICAL,
        .sector = RACE_SECTOR_2,
        .corner_name = "NORTH RUN",
        .entry_edge = RACE_EDGE_BOTTOM,
        .exit_edge = RACE_EDGE_TOP,
        .spawn_x = 114.0f,
        .spawn_y = 205.0f,
        .spawn_heading_deg = 0.0f,
        .road_rects = {
            {80, 28, 80, 212},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        .road_rect_count = 1,
        .curb_side = RACE_CURB_NONE,
        .curb_rect = {0, 0, 0, 0},
        .has_start_finish = false
    },

    {
        .tile_type = RACE_TRACK_TURN_UP_LEFT,
        .sector = RACE_SECTOR_2,
        .corner_name = "HOTEL LEFT",
        .entry_edge = RACE_EDGE_BOTTOM,
        .exit_edge = RACE_EDGE_LEFT,
        .spawn_x = 114.0f,
        .spawn_y = 205.0f,
        .spawn_heading_deg = 0.0f,
        .road_rects = {
            {78, 110, 84, 130},
            {0, 72, 122, 84},
            {78, 72, 84, 84}
        },
        .road_rect_count = 3,
        .curb_side = RACE_CURB_LEFT,
        .curb_rect = {24, 78, 60, 12},
        .has_start_finish = false
    },

    {
        .tile_type = RACE_TRACK_STRAIGHT_HORIZONTAL,
        .sector = RACE_SECTOR_2,
        .corner_name = "WEST STRAIGHT",
        .entry_edge = RACE_EDGE_RIGHT,
        .exit_edge = RACE_EDGE_LEFT,
        .spawn_x = 222.0f,
        .spawn_y = 105.0f,
        .spawn_heading_deg = 270.0f,
        .road_rects = {
            {0, 80, 240, 80},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        .road_rect_count = 1,
        .curb_side = RACE_CURB_NONE,
        .curb_rect = {0, 0, 0, 0},
        .has_start_finish = false
    },

    {
        .tile_type = RACE_TRACK_TURN_RIGHT_UP,
        .sector = RACE_SECTOR_2,
        .corner_name = "T4 RIGHT 90",
        .entry_edge = RACE_EDGE_RIGHT,
        .exit_edge = RACE_EDGE_TOP,
        .spawn_x = 222.0f,
        .spawn_y = 105.0f,
        .spawn_heading_deg = 270.0f,
        .road_rects = {
            {108, 82, 132, 82},
            {76, 28, 84, 136},
            {76, 82, 84, 82}
        },
        .road_rect_count = 3,
        .curb_side = RACE_CURB_RIGHT,
        .curb_rect = {148, 64, 12, 58},
        .has_start_finish = false
    }
};

int RaceTrackLayout_GetScreenCount(void)
{
    return (int)(sizeof(g_track_screens) / sizeof(g_track_screens[0]));
}

const RaceTrackScreen *RaceTrackLayout_GetScreenByIndex(int index)
{
    int count = RaceTrackLayout_GetScreenCount();

    if ((index < 0) || (index >= count)) {
        return NULL;
    }

    return &g_track_screens[index];
}