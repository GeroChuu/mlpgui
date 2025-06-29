
#include <raylib.h>
#include <mlpgui.h>
#include <string.h>

char input[1028]  = {0};
bool do_textinput = true;
bool do_slide     = false;
bool do_x_slide   = false;
bool show_hi      = false;

float value   = 0;
float x_value = 0;

int main() {
    InitWindow(800, 600, "MLP_GUI");
    gui_set_default_style();

    Font default_font = LoadFontEx("./run_tree/IosevkaSS01-Extended.ttf", 20, NULL, 0);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x181828ff));

        gui_enter_frame();

        gui_set_font(default_font);
        if (gui_button((Rectangle){20, 20, 100, 28}, "name") == STATE_DOWN) {
            enum gui_text_alignment align = gui_get_text_alignment();
            gui_set_text_alignment(ALIGNMENT_LEFT);

            gui_label((Rectangle){130, 20, 190, 28}, "My name is, my name is, chika-chika slim shady!");
            gui_set_text_alignment(align);
        }

        gui_disable();
        gui_button((Rectangle){20, 64, 100, 28}, "disable");

        gui_enable();
        enum gui_state state = gui_slider((Rectangle){130, 64, 190, 28}, &value, false, do_slide);
        if (state == STATE_PRESSED && !do_slide) {
            do_slide = true;

        } else if (state == STATE_RELEASED) {
            do_slide = false;
        }

        state = gui_textinput((Rectangle){20, 108, 300, 28}, input, sizeof(input), do_textinput);
        if (state == STATE_PRESSED && !do_textinput) {
            do_textinput = true;
            show_hi = false;

        } else if (state == STATE_SUBMITED) {
            do_textinput = false;
            show_hi = true;

        } else if (state == STATE_NORMAL) {
            do_textinput = false;
        }

        if (show_hi) {
            state = gui_slider((Rectangle){20, 164, 300, 28}, &x_value, false, do_x_slide);
            if (state == STATE_PRESSED && !do_x_slide) {
                do_x_slide = true;

            } else if (state == STATE_RELEASED) {
                do_x_slide = false;
            }

            float x_pos = 20 + x_value*300;
            gui_set_text_alignment(ALIGNMENT_LEFT);
            gui_label((Rectangle){x_pos, 220, 50, 28}, "Hi,");
            gui_label((Rectangle){x_pos+60, 220, 200, 28}, input);

            Vector2 ts = MeasureTextEx(default_font, input, 20, 0);
            gui_label((Rectangle){x_pos+60  + ts.x, 220, 200, 28}, ". You are");
            gui_label((Rectangle){x_pos+160 + ts.x, 220, 200, 28}, TextFormat("%3d", (int)(value*100)));
            gui_label((Rectangle){x_pos+190 + ts.x, 220, 200, 28}, "% drunk!");

            if (strcmp(input, "exit") == 0) break;
        }
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
