#define NOB_IMPLEMENTATION
#include "./src/nob.h"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-o", "./run_tree/main");
    nob_cmd_append(&cmd, "-I./src/", "-L./lib/");

    nob_cmd_append(&cmd, "./src/main.c", "./src/mlpgui.c");
    nob_cmd_append(&cmd, "-lraylib", "-lm");
    
    if (!nob_cmd_run_sync(cmd)) return 1;
    return 0;
}
