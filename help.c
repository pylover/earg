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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "toolbox.h"
#include "builtin.h"
#include "state.h"


#define OPT_MINGAP 4
#define OPT_HELPLEN(o) ((o)->name? \
    (strlen((o)->name) + ((o)->arg? strlen((o)->arg) + 1: 0)): 0)


static int
_calculate_initial_gapsize(const struct earg *c, bool subcommand) {
    int gapsize = 8;

    if ((!subcommand) && (!HASFLAG(c, EARG_NOELOG))) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_verbosity) + OPT_MINGAP);
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_verboseflag) + OPT_MINGAP);
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_quietflag) + OPT_MINGAP);
    }

    if (!HASFLAG(c, EARG_NOHELP)) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_help) + OPT_MINGAP);
    }

    if (!HASFLAG(c, EARG_NOUSAGE)) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_usage) + OPT_MINGAP);
    }

    if (c->version) {
        gapsize = MAX(gapsize, OPT_HELPLEN(&opt_version) + OPT_MINGAP);
    }

    return gapsize;
}


static void
_print_multiline(FILE *file, const char *string, int indent, int linemax) {
    int remain;
    int linesize = linemax - indent;
    int ls;
    bool dash = false;

    if (string == NULL) {
        return;
    }

    remain = strlen(string);
    while (remain) {
        dash = false;
        while (remain && isspace((int)string[0])) {
            string++;
            remain--;
        }

        if (remain <= linesize) {
            fprintf(file, "%s\n", string);
            remain = 0;
            break;
        }

        ls = linesize;
        if (string[ls - 2] == ' ') {
            ls--;
        }
        else if ((string[ls - 1] != ' ') && (string[ls] != ' ') &&
                (!ISSIGN(string[ls - 1])) && (!ISSIGN(string[ls]))) {
            ls--;
            dash = true;
        }

        if (string[ls - 1] == ' ') {
            ls--;
        }

        fprintf(file, "%.*s%s\n", ls, string, dash? "-": "");
        remain -= ls;
        string += ls;
        fprintf(file, "%*s", indent, "");
    }
}


static void
_print_optiongroup(FILE *file, const struct earg_option *opt, int gapsize) {
    int rpad;

    if (opt->name && (!STREQ("-", opt->name))) {
        rpad = (gapsize + 8) - strlen(opt->name);
        fprintf(file, "\n%s%*s", opt->name, rpad, "");
    }

    if (opt->help) {
        _print_multiline(file, opt->help, gapsize + 8, CONFIG_EARG_HELP_LINESIZE);
    }
    else {
        fprintf(file, "\n");
    }
}


static void
_print_subcommands(FILE *file, const struct earg_command *cmd) {
    const struct earg_command **c = cmd->commands;
    const struct earg_command *s;

    if (cmd->commands == NULL) {
        return;
    }

    fprintf(file, "\nCommands:\n");
    while ((s = *c)) {
        fprintf(file, "  %s\n", s->name);
        c++;
    }
}


static void
_print_option(FILE *file, const struct earg_option *opt, int gapsize) {
    int rpad = gapsize - OPT_HELPLEN(opt);

    if (ISCHAR(opt->key)) {
        fprintf(file, "  -%c%c ", opt->key, opt->name? ',': ' ');
    }
    else {
        fprintf(file, "      ");
    }

    if (opt->name) {
        if (opt->arg == NULL) {
            fprintf(file, "--%s%*s", opt->name, rpad, "");
        }
        else {
            fprintf(file, "--%s=%s%*s", opt->name, opt->arg, rpad, "");
        }
    }
    else {
        fprintf(file, "  %*s", rpad, "");
    }

    if (opt->help) {
        _print_multiline(file, opt->help, gapsize + 8, CONFIG_EARG_HELP_LINESIZE);
    }
    else {
        fprintf(file, "\n");
    }
}


static void
_print_options(FILE *file, const struct earg *c, const struct earg_command *cmd) {
    int gapsize;
    int i = 0;
    const struct earg_option *opt;
    bool subcommand = c->state->cmdstack.len > 1;

    /* calculate gap size between options and description */
    gapsize = _calculate_initial_gapsize(c, subcommand);
    while (cmd->options) {
        opt = &(cmd->options[i++]);
        if (opt->name == NULL) {
            break;
        }

        gapsize = MAX(gapsize, OPT_HELPLEN(opt) + OPT_MINGAP);
    }

    fprintf(file, "\nOptions:\n");
    if (!HASFLAG(c, EARG_NOHELP)) {
        _print_option(file, &opt_help, gapsize);
    }

    if (!HASFLAG(c, EARG_NOUSAGE)) {
        _print_option(file, &opt_usage, gapsize);
    }

    if ((!subcommand) && (!HASFLAG(c, EARG_NOELOG))) {
        _print_option(file, &opt_verboseflag, gapsize);
        _print_option(file, &opt_quietflag, gapsize);
        _print_option(file, &opt_verbosity, gapsize);
    }

    if (!subcommand && c->version) {
        _print_option(file, &opt_version, gapsize);
    }

    i = 0;
    while (cmd->options) {
        opt = &(cmd->options[i++]);
        if (opt->name == NULL) {
            break;
        }

        if (opt->key) {
            _print_option(file, opt, gapsize);
        }
        else {
            _print_optiongroup(file, opt, gapsize);
        }
    }
}


void
earg_usage_print(FILE *file, const struct earg *c) {
    char delim[1] = {'\n'};
    char *needle;
    char *saveptr = NULL;
    char *buff = NULL;
    struct earg_state *state = c->state;
    const struct earg_command *cmd = cmdstack_last(&state->cmdstack);

    POUT("Usage: ");
    cmdstack_print(file, &state->cmdstack);
    POUT(" [OPTION...]");

    if (cmd->args == NULL) {
        goto done;
    }

    buff = malloc(strlen(cmd->args) + 1);
    strcpy(buff, cmd->args);

    needle = strtok_r(buff, delim, &saveptr);
    POUT(" %s", needle);
    while (true) {
        needle = strtok_r(NULL, delim, &saveptr);
        if (needle == NULL) {
            break;
        }
        POUT("\n   or: ");
        cmdstack_print(file, &state->cmdstack);
        POUT(" [OPTION...] %s", needle);
    }

done:
    if (buff) {
        free(buff);
    }
    POUT("\n");
}


void
earg_help_print(FILE *file, const struct earg *c) {
    struct earg_state *state = c->state;
    const struct earg_command *cmd = cmdstack_last(&state->cmdstack);

    /* usage */
    earg_usage_print(file, c);

    /* header */
    if (cmd->header) {
        POUT("\n");
        _print_multiline(file, cmd->header, 0, CONFIG_EARG_HELP_LINESIZE);
    }

    /* sub-commands */
    if (cmd->commands) {
        _print_subcommands(file, cmd);
    }

    /* options */
    _print_options(file, c, cmd);

    /* footer */
    if (cmd->footer) {
        POUT("\n");
        _print_multiline(file, cmd->footer, 0, CONFIG_EARG_HELP_LINESIZE);
    }
}
