/******************************************************************************\
**
**  This file is part of the Hades GBA Emulator, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2021 - The Hades Authors
**
\******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "hades.h"
#include "debugger.h"
#include "compat.h"
#include "gba.h"

/*
** Dump the registers' content, a disassembly of instructions around the PC
** and the content of the stack.
*/
void
debugger_dump_context(
    struct gba *gba,
    bool force
) {
    if (!force && !hs_isatty(STDIN_FILENO)) {
        return ;
    }

    printf("---------------------------------Registers----------------------------------\n");
    debugger_cmd_registers(
        gba,
        1,
        ((char const * const[]){"registers"})
    );
    printf("------------------------------------Code------------------------------------\n");
    debugger_cmd_disas(
        gba,
        1,
        ((char const * const[]){"disas"})
    );
    printf("-----------------------------------Stack------------------------------------\n");
    debugger_cmd_print_u8(
        gba,
        gba->core.sp,
        3 * 16,
        16
    );
    printf("----------------------------------------------------------------------------\n");
}

void
debugger_dump_context_compact_header(void)
{
    size_t i;

    printf("  %sCycle Counter%s  ", g_light_green, g_reset);

    for (i = 0; i < 16; ++i) {
        printf(
            "   %s%s%s    ",
            g_light_green,
            registers_name[i],
            g_reset
        );
    }

    printf("   %sCPSR%s", g_light_green, g_reset);
    printf("\n");
}

/*
** Print a shorter, more compact version of `debugger_dump_context()` (regs only).
*/
void
debugger_dump_context_compact(
    struct gba *gba
) {
    struct core *core;
    size_t i;
    size_t op_len;

    core = &gba->core;
    op_len = core->cpsr.thumb ? 2 : 4;

    printf(
        "%016" PRIu64 " ",
        gba->scheduler.cycles
    );

    for (i = 0; i < 16; ++i) {
        printf(
            "%08x ",
            core->registers[i]
        );
    }

    printf(
        "%c%c%c%c%c%c%c/%s   ",
        core->cpsr.negative ? 'n' : '-',
        core->cpsr.zero ? 'z' : '-',
        core->cpsr.carry ? 'c' : '-',
        core->cpsr.overflow ? 'v' : '-',
        core->cpsr.irq_disable ? 'i' : '-',
        core->cpsr.fiq_disable ? 'f' : '-',
        core->cpsr.thumb ? 't' : '-',
        arm_modes_name[core->cpsr.mode]
    );

    debugger_cmd_disas_at(gba, core->pc - op_len * 2);
    printf("\n");
}

void
debugger_cmd_context(
    struct gba *gba,
    size_t argc __unused,
    char const * const *argv __unused
) {
    debugger_dump_context(gba, true);
}

void
debugger_cmd_context_compact(
    struct gba *gba,
    size_t argc __unused,
    char const * const *argv __unused
) {
    debugger_dump_context_compact(gba);
    debugger_dump_context_compact_header();
}