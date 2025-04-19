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
#ifndef TOKENIZER_H_
#define TOKENIZER_H_


#include "optiondb.h"


struct token {
    const char *text;
    unsigned int len;
    const struct optioninfo *optioninfo;
};


enum tokenizer_status {
    EARG_TOK_UNKNOWN = -2,
    EARG_TOK_ERROR = -1,
    EARG_TOK_END = 0,
    EARG_TOK_OPTION = 1,
    EARG_TOK_POSITIONAL = 2,
};


struct tokenizer *
tokenizer_new(int argc, const char **argv,
        const struct optiondb *optdb);


void
tokenizer_dispose(struct tokenizer *t);


enum tokenizer_status
tokenizer_next(struct tokenizer *t, struct token *token);


#endif  // TOKENIZER_H_
