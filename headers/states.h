//
// Created by Nicolas Reyland on 5/10/22.
//

#ifndef FSOCEAN_STATES_H
#define FSOCEAN_STATES_H

typedef enum {
    STATE_GENERAL = 0x100,
    STATE_FILENAME = 0x200,
    STATE_AMPERSAND = 0x300,
    STATE_PIPE = 0x400,
    STATE_SINGLE_QUOTES = 0x500,
    STATE_DOUBLE_QUOTES = 0x600,
    STATE_EOF = 0xf00,
} STATE;

char * STATE_STRING(STATE);

#endif // FSOCEAN_STATES_H
