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
#include <stdio.h>

#include "toolbox.h"
#include "earg.h"
#include "option.h"


int
option_print(FILE *file, const struct earg_option *opt) {
    int bytes = 0;
    int status;

    if ((opt->key != 0) && ISCHAR(opt->key)) {
        status = fprintf(file, "-%c%s", opt->key, opt->name? "/": "");
        if (status == -1) {
            return -1;
        }

        bytes += status;
    }

    if (opt->name) {
        status = fprintf(file, "--%s", opt->name);
        if (status == -1) {
            return -1;
        }

        bytes += status;
    }

    return bytes;
}
