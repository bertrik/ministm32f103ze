#include "print.h"
#include "cmdproc.h"
#include "editline.h"

#include "Arduino.h"

static char line[200];

void setup()
{
    pinMode(PE0, OUTPUT);
    PrintInit();
    EditInit(line, sizeof(line));
}

static void show_help(const cmd_t *cmds)
{
    for (const cmd_t *cmd = cmds; cmd->cmd != NULL; cmd++) {
        print("%10s: %s\n", cmd->name, cmd->help);
    }
}

static int do_led(int argc, char *argv[])
{
    if (argc < 2) {
        print("Need argument!\n");
        return -1;
    }
    int on = atoi(argv[1]);
    print("Setting LED PE0 to %d\n", on);
    digitalWrite(PE0, on);
    return 0;
}

static int do_help(int argc, char *argv[]);

const cmd_t commands[] = {
    {"led",     do_led,     "[val] Set LED to 'val'"},
    {"help",    do_help,    "Show help"},
    {NULL, NULL, NULL}
};

static int do_help(int argc, char *argv[])
{
    show_help(commands);
    return 0;
}

void loop()
{
    bool haveLine = false;
    if (Serial.available()) {
        char c;
        haveLine = EditLine(Serial.read(), &c);
        Serial.print(c);
    }
    if (haveLine) {
        int result = cmd_process(commands, line);
        switch (result) {
        case CMD_OK:
            print("OK\n");
            break;
        case CMD_NO_CMD:
            break;
        case CMD_UNKNOWN:
            print("Unknown command, available commands:\n");
            show_help(commands);
            break;
        default:
            print("%d\n", result);
            break;
        }
        print(">");
    }
}

