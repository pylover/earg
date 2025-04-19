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
#ifndef BUILTIN_H_
#define BUILTIN_H_

#include "earg.h"


#ifdef CONFIG_EARG_USE_CLOG
extern const struct earg_option opt_verbosity;
extern const struct earg_option opt_verboseflag;
extern const struct earg_option opt_quietflag;
#endif
extern const struct earg_option opt_version;
extern const struct earg_option opt_help;
extern const struct earg_option opt_usage;


#endif  // BUILTIN_H_
