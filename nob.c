#define NOB_IMPLEMENTATION
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "./src/nob.h"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = {0};

    const char *program = nob_shift_args(&argc, &argv);
    NOB_UNUSED(program);

    if (argc > 0) {
        const char *sub_cmd = nob_shift_args(&argc, &argv);
        if (strcmp(sub_cmd, "web") == 0) {
            nob_mkdir_if_not_exists("web_build");

            nob_cmd_append(&cmd, "emcc", "-o", "web_build/index.html");
            nob_cmd_append(&cmd, "src/main.c", "src/mlpgui.c");
            nob_cmd_append(&cmd, "-Os", "-Wall", "../raylib/src/bin/web/libraylib.a");
            nob_cmd_append(&cmd, "-I./src/", "-L../raylib/src/bin/web/", "-s", "USE_GLFW=3", "-s", "ASYNCIFY");
            nob_cmd_append(&cmd, "--shell-file", "../raylib/src/minshell.html");
            nob_cmd_append(&cmd, "-DPLATFORM_WEB", "--preload-file", "run_tree/");
            nob_cmd_append(&cmd, "-s", "TOTAL_MEMORY=67108864", "-s", "ALLOW_MEMORY_GROWTH=1");

            if (!nob_cmd_run_sync(cmd)) return 1;

        } else if (strcmp(sub_cmd, "win") == 0) {
            nob_mkdir_if_not_exists("win_build");
            nob_cmd_append(&cmd, "i686-w64-mingw32-cc", "./src/main.c", "./src/mlpgui.c");
            nob_cmd_append(&cmd, "-o", "win_build/mlpgui.exe", "-lraylib", "-lwinmm", "-lgdi32");

            nob_cmd_append(&cmd, "-I./src/", "-L../raylib/src/bin/win32");

            if (!nob_cmd_run_sync(cmd)) return 1;
        }

    } else {
        nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-o", "mlpgui");
        nob_cmd_append(&cmd, "-I./src/", "-L./lib/");

        nob_cmd_append(&cmd, "./src/main.c", "./src/mlpgui.c");
        nob_cmd_append(&cmd, "-lraylib", "-lm");

        if (!nob_cmd_run_sync(cmd)) return 1;
    }
    
    return 0;
}
