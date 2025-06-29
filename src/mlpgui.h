#ifndef MLPGUI_H_
#define MLPGUI_H_

#include <raylib.h>
#include <stddef.h>

enum gui_state {
    STATE_NORMAL,
    STATE_DOWN,
    STATE_PRESSED,
    STATE_RELEASED,
    STATE_HOVERED,
    STATE_SUBMITED,
    STATE_ACTIVE,
};

enum gui_text_alignment {
    ALIGNMENT_CENTER,
    ALIGNMENT_LEFT,
    ALIGNMENT_RIGHt,
};

void gui_set_default_style(void);
void gui_enter_frame(void);
void gui_disable(void);
void gui_enable(void);

void gui_set_text_alignment(enum gui_text_alignment align);
enum gui_text_alignment gui_get_text_alignment(void);

void gui_set_font(Font font);
Font gui_get_font(void);

enum gui_state gui_label(Rectangle bound, const char *label);
enum gui_state gui_button(Rectangle bound, const char *label);
enum gui_state gui_checkbox(Rectangle bound, bool *value, const char *label);
enum gui_state gui_slider(Rectangle bound, float *value, bool is_vertical, bool slide_mode);
enum gui_state gui_textinput(Rectangle bound, char *container, size_t length, bool edit_mode);

#endif // MLPGUI_H_
