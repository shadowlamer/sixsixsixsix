//
// Created by sl on 01.11.2020.
//

#ifndef YRGB2020_MISSIONS_H
#define YRGB2020_MISSIONS_H

typedef struct {
    char *intro;
    track_element_t *track;
} mission_t;

#define NUM_MISSIONS 3

const static mission_t missions[NUM_MISSIONS] = {
    {
        .intro = msg_mission1,
        .track = track
    },
    {
        .intro = msg_mission2,
        .track = track
    },
    {
        .intro = msg_mission3,
        .track = track
    }
};

//static const missions

#endif //YRGB2020_MISSIONS_H
