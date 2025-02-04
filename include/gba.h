/******************************************************************************\
**
**  This file is part of the Hades GBA Emulator, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2021 - The Hades Authors
**
\******************************************************************************/

#ifndef GBA_H
# define GBA_H

# include "core.h"
# include "memory.h"
# include "ppu.h"
# include "debugger.h"
# include "io.h"
# include "scheduler.h"

struct options
{
    bool debugger;
    uint32_t scale;
    bool headless;
    uint32_t color;     // 0: auto, 1: never, 2: always
};

struct gba
{
    struct core core;
    struct memory memory;
#if ENABLE_DEBUGGER
    struct debugger debugger;
#endif
    struct io io;
    struct scheduler scheduler;

    /*
    ** A mutex that protects the above structure from
    ** concurrency errors, especially when saving/loading savestates.
    */
    pthread_mutex_t emulator_mutex;

    /*
    ** Read-only past initialization.
    ** Can therefore be used by all threads.
    */
    struct options options;

    /*
    ** Read-only past initialization.
    ** Can therefore be used by all threads.
    */
    char game_title[13];
    char const *rom_path;
    char *save_path;

    /*
    ** The result of the video controller, used by the renderer to
    ** print things on screen.
    **
    ** Can be accessed by both the logic and render thread.
    */
    uint32_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
    pthread_mutex_t framebuffer_mutex;

    /*
    ** The state of each input buttons.
    **
    ** Can be accessed by both the logic and render thread.
    */
    pthread_mutex_t input_mutex;
    union {
        struct {
            uint16_t a: 1;
            uint16_t b: 1;
            uint16_t select: 1;
            uint16_t start: 1;
            uint16_t right: 1;
            uint16_t left: 1;
            uint16_t up: 1;
            uint16_t down: 1;
            uint16_t r: 1;
            uint16_t l: 1;
            uint16_t : 6;
        } __packed;
        uint16_t raw;
        uint8_t bytes[2];
    } input;

    /*
    **Amount of frames elapsed since the beginning of the execution.
    */
    atomic_uint frame_counter;
};

/* save.c */
void save_state(struct gba *gba, char const *path);
void load_state(struct gba *gba, char const *path);

#endif /* GBA_H */