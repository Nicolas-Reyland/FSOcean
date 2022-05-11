//
// Created by Nicolas Reyland on 5/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include "states.h"

char * STATE_STRING(STATE state) {
    switch (state) {
        case STATE_GENERAL:
            return "STATE_GENERAL";
        case STATE_FILENAME:
            return "STATE_FILENAME";
        case STATE_AMPERSAND:
            return "STATE_AMPERSAND";
        case STATE_PIPE:
            return "STATE_PIPE";
        case STATE_SINGLE_QUOTES:
            return "STATE_SINGLE_QUOTES";
        case STATE_DOUBLE_QUOTES:
            return "STATE_DOUBLE_QUOTES";
        case STATE_RIGHT_ANGLE_BRACKET:
            return "STATE_RIGHT_ANGLE_BRACKET";
        case STATE_LEFT_ANGLE_BRACKET:
            return "STATE_LEFT_ANGLE_BRACKET";
        case STATE_SINGLE_CHAR:
            return "STATE_SINGLE_CHAR";
        case STATE_EOF:
            return "STATE_EOF";
        case STATE_ESCAPE:
            return "STATE_ESCAPE";
        default:
            fprintf(stderr, "Unknown state: %d\n", state);
            exit(1);
    }
}
