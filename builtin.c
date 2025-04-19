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
#include <limits.h>
#include <stddef.h>

#include "builtin.h"


/* builtin options */
#define EARG_OPTKEY_VERSION (INT_MIN + 1)


#define EARG_OPTKEY_VERBOSITY (INT_MIN + 2)
const struct earg_option opt_verbosity = {
    .name = "verbosity",
    .key = EARG_OPTKEY_VERBOSITY,
    .arg = "LEVEL",
    .flags = 0,
    .help = "Verbosity level. one of: '0|s|silent', '1|f|fatal', '2|e|error'"
        ", '3|w|warn', '4|i|info' and '5|d|debug'. if this option is not "
        "given, the verbosity level will be '4|i|info'"
};


const struct earg_option opt_verboseflag = {
    .name = NULL,
    .key = 'v',
    .arg = NULL,
    .flags = EARG_OPTION_MULTIPLE,
    .help = "Increase the elog_verbosity on each occurance, e.g. -vvv"
};


const struct earg_option opt_quietflag = {
    .name = NULL,
    .key = 'q',
    .arg = NULL,
    .flags = EARG_OPTION_MULTIPLE,
    .help = "Decrease the elog_verbosity on each occurance, e.g. -qq"
};


const struct earg_option opt_version = {
    .name = "version",
    .key = EARG_OPTKEY_VERSION,
    .arg = NULL,
    .flags = 0,
    .help = "Print program version and exit"
};


const struct earg_option opt_help = {
    .name = "help",
    .key = 'h',
    .arg = NULL,
    .flags = 0,
    .help = "Give this help list and exit"
};


const struct earg_option opt_usage = {
    .name = "usage",
    .key = '?',
    .arg = NULL,
    .flags = 0,
    .help = "Give a short usage message and exit"
};
