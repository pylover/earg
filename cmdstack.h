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
#ifndef CMDSTACK_H_
#define CMDSTACK_H_


struct cmdstack {
    const char *names[CONFIG_EARG_CMDSTACK_MAX];
    const struct earg_command *commands[CONFIG_EARG_CMDSTACK_MAX];
    unsigned char len;
};


void
cmdstack_init(struct cmdstack *s);


int
cmdstack_push(struct cmdstack *s, const char *name,
        const struct earg_command *cmd);


const struct earg_command *
cmdstack_last(struct cmdstack *s);


int
cmdstack_print(FILE *file, struct cmdstack *s);


#endif  // CMDSTACK_H_
