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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "config.h"
#include "earg.h"
#include "state.h"
#include "toolbox.h"
#include "builtin.h"
#include "arghint.h"
#include "command.h"
#include "option.h"
#include "help.h"
#include "optiondb.h"
#include "tokenizer.h"


#define TRYHELP(s) \
    PERR("Try `"); \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(" --help' or `"); \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(" --usage' for more information.\n")

#define REJECT_OPTION_MISSINGARGUMENT(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": option requires an argument -- '"); \
    option_print(STDERR_FILENO, o); \
    PERR("'\n")

#define REJECT_OPTION_HASARGUMENT(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": no argument allowed for option -- '"); \
    option_print(STDERR_FILENO, o); \
    PERR("'\n")

#define REJECT_OPTION_UNRECOGNIZED(s, name, len) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": invalid option -- '%s%.*s'\n", \
        len == 1? "-": "", len, name)

#define REJECT_OPTION_NOTEATEN(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": option not eaten -- '"); \
    option_print(STDERR_FILENO, o); \
    PERR("'\n")

#define REJECT_OPTION_REDUNDANT(s, o) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": redundant option -- '"); \
    option_print(STDERR_FILENO, o); \
    PERR("'\n")

#define REJECT_POSITIONAL_NOTEATEN(s, t) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": argument not eaten -- '%s'\n", t)

#define REJECT_POSITIONAL(s, t) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": invalid argument -- '%s'\n", t)

#define REJECT_POSITIONALCOUNT(s) \
    cmdstack_print(STDERR_FILENO, &(s)->cmdstack); \
    PERR(": invalid positional arguments count\n")


static int
_build_optiondb(const struct earg *c, struct optiondb *db) {
    if (optiondb_init(db)) {
        return -1;
    }

    if (c->version && optiondb_insert(db, &opt_version,
                (struct earg_command *)c)) {
        return -1;
    }

    if ((!HASFLAG(c, EARG_NOHELP)) && optiondb_insert(db, &opt_help,
                (struct earg_command *)c)) {
        return -1;
    }

    if ((!HASFLAG(c, EARG_NOUSAGE)) && optiondb_insert(db, &opt_usage,
                (struct earg_command *)c)) {
        return -1;
    }

#ifdef CONFIG_EARG_USE_ELOG
    if (!HASFLAG(c, EARG_NOELOG)) {
        if (optiondb_insert(db, &opt_verbosity, (struct earg_command *)c)) {
            return -1;
        }
        if (optiondb_insert(db, &opt_verboseflag, (struct earg_command *)c)) {
            return -1;
        }
        if (optiondb_insert(db, &opt_quietflag, (struct earg_command *)c)) {
            return -1;
        }
    }
#endif

    return 0;
}


#ifdef CONFIG_EARG_USE_CLOG

#include <clog.h>


void
_clogquieter() {
    if (clog_verbosity > CLOG_SILENT) {
        clog_verbosity--;
    }
}


void
_clogverboser() {
    if (clog_verbosity < CLOG_DEBUG2) {
        clog_verbosity++;
    }
}

void
_clogverbosity(const char *value) {
    int valuelen = value? strlen(value): 0;

    if (valuelen == 0) {
        clog_verbosity = CLOG_INFO;
        return;
    }

    if (valuelen == 1) {
        if (ISDIGIT(value[0])) {
            /* -v0 ... -v5 */
            clog_verbosity = atoi(value);
            if (!BETWEEN(clog_verbosity, CLOG_SILENT, CLOG_DEBUG2)) {
                clog_verbosity = CLOG_INFO;
                return;
            }
            return;
        }
    }

    clog_verbosity = clog_verbosity_from_string(value);
    if (clog_verbosity == CLOG_UNKNOWN) {
        clog_verbosity = CLOG_INFO;
        return;
    }
}
#endif


static enum earg_eatstatus
_eat(const struct earg *c, const struct earg_command *command,
        const struct earg_option *opt, const char *value) {
    /* Try to solve it internaly */
    if (c->version && (opt == &opt_version)) {
        POUT("%s\n", c->version);
        return EARG_EAT_OK_EXIT;
    }

    if ((!HASFLAG(c, EARG_NOHELP)) && (opt == &opt_help)) {
        earg_help_print(c);
        return EARG_EAT_OK_EXIT;
    }

    if ((!HASFLAG(c, EARG_NOUSAGE)) && (opt == &opt_usage)) {
        earg_usage_print(c);
        return EARG_EAT_OK_EXIT;
    }

#ifdef CONFIG_EARG_USE_CLOG
    if (!HASFLAG(c, EARG_NOELOG)) {
        if (opt == &opt_verbosity) {
            _clogverbosity(value);
            return EARG_EAT_OK;
        }

        if (opt == &opt_verboseflag) {
            _clogverboser();
            return EARG_EAT_OK;
        }

        if (opt == &opt_quietflag) {
            _clogquieter();
            return EARG_EAT_OK;
        }
    }
#endif

    if (command->eat) {
        return command->eat(opt, value, command->userptr);
    }

    return EARG_EAT_NOTEATEN;
}


/* Helper macro */
#define NEXT(t, tok) tokenizer_next(t, tok)


static enum earg_status
_command_parse(struct earg *c, struct tokenizer *t) {
    enum earg_status status = EARG_OK;
    enum tokenizer_status tokstatus;
    enum earg_eatstatus eatstatus;
    struct token tok;
    struct token nexttok;
    struct earg_state *state = c->state;
    const struct earg_command *cmd = cmdstack_last(&state->cmdstack);
    const struct earg_command *subcmd = NULL;
    int arghint = arghint_parse(cmd->args);

    if (optiondb_insertvector(&state->optiondb, cmd->options, cmd) == -1) {
        status = EARG_FATAL;
        goto terminate;
    }

    do {
        /* fetch the next token */
        if ((tokstatus = NEXT(t, &tok)) <= EARG_TOK_END) {
            if (tokstatus == EARG_TOK_UNKNOWN) {
                REJECT_OPTION_UNRECOGNIZED(state, tok.text, tok.len);
                status = EARG_USERERROR;
            }
            goto terminate;
        }

        /* is this a positional? */
        if (tok.optioninfo == NULL) {
            /* is this a sub-command? */
            subcmd = command_findbyname(cmd, tok.text);
            if (subcmd) {
                if (cmdstack_push(&state->cmdstack, tok.text, subcmd) == -1) {
                    status = EARG_FATAL;
                }
                else {
                    status = _command_parse(c, t);
                }
                goto terminate;
            }

            /* it's positional */
            state->positionals++;
            eatstatus = _eat(c, cmd, NULL, tok.text);
            goto dessert;
        }

        /* ensure option occureances */
        if ((!HASFLAG(tok.optioninfo->option, EARG_OPTION_MULTIPLE)) &&
                (tok.optioninfo->occurances > 1)) {
            REJECT_OPTION_REDUNDANT(state, tok.optioninfo->option);
            status = EARG_USERERROR;
            goto terminate;
        }


        /* ensure option's value */
        if (EARG_OPTION_ARGNEEDED(tok.optioninfo->option)) {
            if (tok.text == NULL) {
                /* try the next token as value */
                if ((tokstatus = NEXT(t, &nexttok))
                        != EARG_TOK_POSITIONAL) {
                    REJECT_OPTION_MISSINGARGUMENT(state,
                            tok.optioninfo->option);
                    status = EARG_USERERROR;
                    goto terminate;
                }

                tok.text = nexttok.text;
                tok.len = nexttok.len;
            }
            eatstatus = _eat(c, tok.optioninfo->command,
                    tok.optioninfo->option, tok.text);
        }
        else {
            if (tok.text) {
                REJECT_OPTION_HASARGUMENT(state, tok.optioninfo->option);
                status = EARG_USERERROR;
                goto terminate;
            }
            eatstatus = _eat(c, tok.optioninfo->command,
                    tok.optioninfo->option, NULL);
        }

dessert:
        switch (eatstatus) {
            case EARG_EAT_OK:
                continue;
            case EARG_EAT_OK_EXIT:
                status = EARG_OK_EXIT;
                goto terminate;
            case EARG_EAT_UNRECOGNIZED:
                REJECT_POSITIONAL(state, tok.text);
                status = EARG_USERERROR;
                goto terminate;
            case EARG_EAT_NOTEATEN:
                if (tok.optioninfo) {
                    REJECT_OPTION_NOTEATEN(state, tok.optioninfo->option);
                }
                else {
                    REJECT_POSITIONAL_NOTEATEN(state, tok.text);
                }
            default:
                status = EARG_FATAL;
                goto terminate;
        }
    } while (tokstatus > EARG_TOK_END);

terminate:
    if ((status == EARG_OK) && (subcmd == NULL) &&
            arghint_validate(state->positionals, arghint)) {
        REJECT_POSITIONALCOUNT(state);
        status = EARG_USERERROR;
    }

    return status;
}


enum earg_status
earg_parse(struct earg *c, int argc, const char **argv,
        const struct earg_command **command) {
    struct earg_state *state;
    enum earg_status status = EARG_OK;
    enum tokenizer_status tokstatus;
    struct token tok;
    struct tokenizer *t;

    if (argc < 1) {
        return EARG_FATAL;
    }

    state = malloc(sizeof(struct earg_state));
    if (state == NULL) {
        return EARG_FATAL;
    }
    memset(state, 0, sizeof(struct earg_state));
    state->positionals = 0;
    c->state = state;

    if (_build_optiondb(c, &state->optiondb)) {
        return EARG_FATAL;
    }

    t = tokenizer_new(argc, argv, &state->optiondb);
    if (t == NULL) {
        optiondb_dispose(&state->optiondb);
        return EARG_FATAL;
    }

    /* initialize command stack */
    cmdstack_init(&state->cmdstack);

    /* excecutable name */
    if ((tokstatus = NEXT(t, &tok)) != EARG_TOK_POSITIONAL) {
        goto terminate;
    }

    if (cmdstack_push(&state->cmdstack, tok.text,
                (struct earg_command *)c) == -1) {
        goto terminate;
    }
    c->name = tok.text;

    status = _command_parse(c, t);
    if (status < EARG_OK) {
        goto terminate;
    }

    /* commands */
    if (command) {
        *command = cmdstack_last(&state->cmdstack);
    }

terminate:
    tokenizer_dispose(t);
    optiondb_dispose(&state->optiondb);
    if (status == EARG_USERERROR) {
        TRYHELP(state);
    }
    return status;
}


int
earg_dispose(struct earg *c) {
    if (c == NULL) {
        return -1;
    }

    if (c->state == NULL) {
        return -1;
    }

    free(c->state);
    c->state = NULL;
    return 0;
}


int
earg_try_help(const struct earg* c) {
    if (c == NULL) {
        return -1;
    }

    if (c->state == NULL) {
        return -1;
    }

    TRYHELP(c->state);
    return 0;
}


int
earg_commandchain_print(int fd, const struct earg *c) {
    if (fd < 0) {
        return -1;
    }

    if (c == NULL) {
        return -1;
    }

    return cmdstack_print(fd, &c->state->cmdstack);
}
