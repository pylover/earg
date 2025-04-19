// Copyright 2023 Vahid Mardani
/*
 * This file is part of earg.
 *  earg is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  earg is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with earg. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#ifndef EARG_H_
#define EARG_H_


#include <stdbool.h>
#include <stdio.h>


#ifdef __clang__
#define _Nullable _Nullable
#elif defined(__GNUC__)
#define _Nullable
#else
#define _Nullable 888
#endif


/* earg_parse() result */
enum earg_status {
    EARG_USERERROR = -2,
    EARG_FATAL = -1,
    EARG_OK = 0,
    EARG_OK_EXIT = 1,
};


/* argument eat result */
enum earg_eatstatus {
    EARG_EAT_OK,
    EARG_EAT_OK_EXIT,
    EARG_EAT_UNRECOGNIZED,
    EARG_EAT_INVALID,
    EARG_EAT_NOTEATEN,
};


/* earg flags */
enum earg_flags{
    EARG_NOHELP = 1,
    EARG_NOUSAGE = 2,
    EARG_NOELOG = 4,
};


struct earg;
struct earg_option;
struct earg_command;
typedef enum earg_eatstatus (*earg_eater_t)
    (const struct earg_option *option, const char *value, void *userptr);
typedef int (*earg_entrypoint_t) (const struct earg *c,
        const struct earg_command *cmd);


/* option flags */
enum earg_optionflags {
    EARG_OPTION_NONE = 0,
    EARG_OPTION_MULTIPLE = 1,
};


/* option structure */
struct earg_option {
    const char *name;
    const int key;
    const char *arg;
    enum earg_optionflags flags;
    const char *help;
};


/* Abstract base class! */
struct earg_command {
    const char *name;
    const struct earg_option * _Nullable options;
    const struct earg_command ** _Nullable commands;
    const char *args;
    const char *header;
    const char *footer;
    earg_eater_t eat;
    void *userptr;
    earg_entrypoint_t entrypoint;
};


typedef struct earg_state *earg_state_t;
struct earg {
    struct earg_command;

    const char *version;
    enum earg_flags flags;

    /* Internal earg state */
    earg_state_t state;
};


enum earg_status
earg_parse(struct earg *c, int argc, const char **argv,
        const struct earg_command **command);


int
earg_dispose(struct earg *c);


void
earg_usage_print(FILE *file, const struct earg *c);


void
earg_help_print(FILE *file, const struct earg *c);


int
earg_try_help(const struct earg *c);


int
earg_commandchain_print(FILE *file, const struct earg *c);


#endif  // EARG_H_
