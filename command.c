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
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "toolbox.h"


const struct earg_command *
command_findbyname(const struct earg_command *cmd, const char *name) {
    if (name == NULL) {
        return NULL;
    }

    if ((cmd == NULL) || (cmd->commands == NULL)) {
        return NULL;
    }

    const struct earg_command **c = cmd->commands;
    const struct earg_command *s;

    while ((s = *c)) {
        if (STREQ(name, s->name)) {
            return (const struct earg_command *)s;
        }

        c++;
    }

    return NULL;
}
