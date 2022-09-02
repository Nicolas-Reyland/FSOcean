//
// Created by Nicolas Reyland on 8/25/22.
//

#ifndef OCEAN_EXECUTABLE_FLAGS_H
#define OCEAN_EXECUTABLE_FLAGS_H

#define EXEC_FLAG_EXIT_CODE_MASK 0xff
#define EXEC_FLAG_LOOP_BREAK 0x100

#define EXEC_CMD_SUCCESS(cmd) (((cmd) & EXEC_FLAG_EXIT_CODE_MASK) == 0)

#endif // OCEAN_EXECUTABLE_FLAGS_H
