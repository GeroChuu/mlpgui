#include <mlpgui.h>
#include <string.h>

#include <nob.h>

bool style_setted = false;

Color background_normal;
Color foreground_normal;
Color background_hovered;
Color foreground_hovered;
Color background_pressed;
Color foreground_pressed;
Color background_disable;
Color foreground_disable;

float box_roundness;
int box_segments;

int  font_size;
int  font_spacing;
Font current_text_font;

enum gui_text_alignment current_aligment;

bool invalid_mouse_hover = false;
bool current_id_disable  = false;

bool any_id_hovered = false;

int textinput_cursor = 0;
int textinput_length = 0;
char textinput_dummy[1024];

static int clamp(int _x, int _min, int _max) {
    int x = _x > _max ? _max : _x;
    return x < _min ? _min : x;
}

void gui_set_default_style(void) {
    style_setted = true;

    background_normal  = GetColor(0xaaaaaaff);
    foreground_normal  = GetColor(0x181818ff);

    background_hovered = GetColor(0xffaaaaff);
    foreground_hovered = GetColor(0x282828ff);

    background_pressed = GetColor(0xff55ffff);
    foreground_pressed = GetColor(0x282828ff);

    background_disable = GetColor(0xaaaaaaaa);
    foreground_disable = GetColor(0x181818aa);

    box_roundness = .3;
    box_segments  = 10;

    current_text_font = GetFontDefault();
    font_size         = current_text_font.baseSize;
    font_spacing      = 0;
}

void gui_enter_frame(void) {
    current_id_disable = false;
    current_aligment   = ALIGNMENT_CENTER;

    if (!any_id_hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) invalid_mouse_hover = true;
    any_id_hovered = false;

    if (invalid_mouse_hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) invalid_mouse_hover = false;
}

void gui_disable(void) {
    current_id_disable = true;
}

void gui_enable(void) {
    current_id_disable = false;
}

void gui_set_text_alignment(enum gui_text_alignment align) {
    current_aligment = align;
}

enum gui_text_alignment gui_get_text_alignment(void) {
    return current_aligment;
}

void gui_set_font(Font font) {
    current_text_font = font;
    font_size = current_text_font.baseSize;
}

Font gui_get_font(void) {
    return current_text_font;
}

enum gui_state gui_button(Rectangle bound, const char *label) {
    if (!style_setted) gui_set_default_style();
    Vector2 m = GetMousePosition();

    Vector2 text_size = MeasureTextEx(current_text_font, label, font_size, font_spacing);

    Vector2 text_pos;
    text_pos.x = bound.x + bound.width/2  - text_size.x/2;
    text_pos.y = bound.y + bound.height/2 - text_size.y/2;

    enum gui_state state = STATE_NORMAL;

    Color bg = background_normal;
    Color fg = foreground_normal;

    if (current_id_disable) {
        bg = background_disable;
        fg = foreground_disable;

    } else if (!invalid_mouse_hover && CheckCollisionPointRec(m, bound)) {
        any_id_hovered = true;
        state = STATE_HOVERED;

        bg = background_hovered;
        fg = foreground_hovered;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            state = STATE_PRESSED;

            bg = background_pressed;
            fg = foreground_pressed;
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            state = STATE_RELEASED;

            bg = background_pressed;
            fg = foreground_pressed;
        } else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            state = STATE_DOWN;

            bg = background_pressed;
            fg = foreground_pressed;
        }
    }

    DrawRectangleRounded(bound, box_roundness, box_segments, bg);
    DrawTextEx(current_text_font, label, text_pos, font_size, font_spacing, fg);

    return state;
}

enum gui_state gui_textinput(Rectangle bound, char *container, size_t length, bool edit_mode) {
    if (!style_setted) gui_set_default_style();
    Vector2 m = GetMousePosition();

    memcpy(textinput_dummy, container, textinput_cursor);
    textinput_dummy[textinput_cursor] = 0;
    
    Vector2 text_size = MeasureTextEx(current_text_font, container, font_size, font_spacing);
    Vector2 dummy_text_size = MeasureTextEx(current_text_font, textinput_dummy, font_size, font_spacing);
    float padding = text_size.y * 0.20f;
    
    Vector2 text_pos;
    text_pos.x = bound.x + padding;
    text_pos.y = bound.y + bound.height/2 - text_size.y/2;

    enum gui_state state = STATE_NORMAL;

    Color bg = background_normal;
    Color fg = foreground_normal;

    if (current_id_disable) {
        bg = background_disable;
        fg = foreground_disable;
    } else {
        if (!invalid_mouse_hover && CheckCollisionPointRec(m, bound)) {
            any_id_hovered = true;
            state = STATE_HOVERED;

            bg = background_hovered;
            fg = foreground_hovered;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                state = STATE_PRESSED;

                bg = background_pressed;
                fg = foreground_pressed;
            }
        }

        if (edit_mode) {
            state = STATE_ACTIVE;


            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(m, bound)) {
                state = STATE_NORMAL;
            }

            textinput_length = clamp(strlen(container), 0, length);
            textinput_cursor = clamp(textinput_cursor, 0, textinput_length);

            bg = background_hovered;
            fg = foreground_hovered;

            int c = GetCharPressed();
            while (c) {
                if (textinput_cursor < textinput_length) {
                    memmove(container + textinput_cursor+1, container+textinput_cursor, textinput_length-textinput_cursor);
                }

                container[textinput_cursor] = c;
                textinput_cursor += 1;
                c = GetCharPressed();
            }

            bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
                int len = 1;

                if (ctrl) {
                    int cur = textinput_cursor; 
                    while (container[cur] != ' ' && cur >= 0) {
                        cur -= 1;
                        len += 1;
                    }
                    len = clamp(len - 2, 1, textinput_length);
                }

                if (textinput_cursor >= len) { 
                    if (textinput_cursor < textinput_length) {
                        memmove(container+textinput_cursor-len, container+textinput_cursor, textinput_length-textinput_cursor);
                    }
                    textinput_length -= len;
                    textinput_cursor -= len;
                    memset(container+textinput_length, 0, len);
                }
            }

            if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
                textinput_cursor = clamp(textinput_cursor-1, 0, textinput_length);
            }

            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
                textinput_cursor = clamp(textinput_cursor+1, 0, textinput_length);
            }

            if (IsKeyPressed(KEY_ENTER)) {
                state = STATE_SUBMITED;
            }
            DrawRectangleRounded(bound, box_roundness, box_segments, WHITE);
        }
    }

    DrawRectangleRoundedLines(bound, box_roundness, box_segments, bg);

    BeginScissorMode(bound.x, bound.y, bound.width, bound.height);
    if (edit_mode) {
        DrawTextEx(current_text_font, container, text_pos, font_size, font_spacing, fg);
        DrawRectangle(bound.x + padding + dummy_text_size.x, bound.y + 2, 2, bound.height-4, fg);
    } else {
        DrawTextEx(current_text_font, container, text_pos, font_size, font_spacing, bg);
    }

    EndScissorMode();
    return state;
}

enum gui_state gui_label(Rectangle bound, const char *label) {
    if (!style_setted) gui_set_default_style();
    Vector2 m = GetMousePosition();

    Vector2 text_size = MeasureTextEx(current_text_font, label, font_size, font_spacing);

    Vector2 text_pos;
    if (current_aligment == ALIGNMENT_CENTER) {
        text_pos.x = bound.x + bound.width/2  - text_size.x/2;
        text_pos.y = bound.y + bound.height/2 - text_size.y/2;
    } else if (current_aligment == ALIGNMENT_LEFT) {
        text_pos.x = bound.x;
        text_pos.y = bound.y + bound.height/2 - text_size.y/2;
    } else if (current_aligment == ALIGNMENT_RIGHt) {
        text_pos.x = bound.x + bound.width  - text_size.x;
        text_pos.y = bound.y + bound.height/2 - text_size.y/2;
    }

    enum gui_state state = STATE_NORMAL;

    Color fg = background_normal;

    if (current_id_disable) {
        fg = foreground_disable;

    } else if (!invalid_mouse_hover && CheckCollisionPointRec(m, bound)) {
        any_id_hovered = true;
        state = STATE_HOVERED;
    }

    DrawTextEx(current_text_font, label, text_pos, font_size, font_spacing, fg);

    return state;
}

enum gui_state gui_slider(Rectangle bound, float *value, bool is_vertical, bool slide_mode) {
    if (!style_setted) gui_set_default_style();
    Vector2 m = GetMousePosition();

    enum gui_state state = STATE_NORMAL;

    Color bg = background_normal;

    Rectangle next;
    Rectangle handle;
    if (is_vertical) {
        handle.x = bound.x;
        handle.y = bound.y + bound.height - (*value * bound.height);
        handle.width  = bound.width;
        handle.height = 5;

        if (slide_mode && value) {
            float padding = bound.width * 0.5;
            bound.x      += padding/2;
            bound.width  -= padding;
        }

        next = bound;

        next.y = handle.y;
        next.height = bound.y + bound.height - handle.y;
    } else {
        handle.x = bound.x + (*value * bound.width);
        handle.y = bound.y;
        handle.width  = 5;
        handle.height = bound.height;

        if (slide_mode && value) {
            float padding = bound.height * 0.5;
            bound.y      += padding/2;
            bound.height -= padding;
        }

        next    = bound;
        next.width   = handle.x-bound.x;
    }

    if (current_id_disable) {
        bg = background_disable;

    } else {
        if (!slide_mode) {
            if (!invalid_mouse_hover && CheckCollisionPointRec(m, bound)) {
                any_id_hovered = true;
                state = STATE_HOVERED;

                bg = background_hovered;

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    state = STATE_PRESSED;

                    bg = background_pressed;
                }
            }

            next.x += 5;
            next.y += 5;
            next.width  -= 10;
            next.height -= 10;
        }

        if (slide_mode && value) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                any_id_hovered = true;
                state = STATE_DOWN;

                bg = background_pressed;

                if (is_vertical) {
                    handle.y = clamp(m.y, bound.y, bound.y + bound.height);

                    next.y = handle.y;
                    next.height = bound.y + bound.height - handle.y;

                    *value = (bound.y + bound.height - handle.y) / bound.height;
                    handle.y = handle.y - 2;
                } else {
                    handle.x = clamp(m.x, bound.x, bound.x + bound.width);
                    next.width = handle.x - bound.x;

                    *value = (handle.x - bound.x)/bound.width;
                    handle.x = handle.x - 2;
                }

                DrawRectangleRounded(handle, box_roundness, box_segments,  bg);

            } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                state = STATE_RELEASED;
            }
        }
    }

    DrawRectangleRounded(next, box_roundness, box_segments, bg);
    DrawRectangleRoundedLines(bound, box_roundness, box_segments, bg);

    return state;
}

enum gui_state gui_checkbox(Rectangle bound, bool *value, const char *label) {
    NOB_ASSERT(false);
    NOB_UNUSED(value);

    if (!style_setted) gui_set_default_style();
    Vector2 m = GetMousePosition();

    Vector2 text_size = MeasureTextEx(current_text_font, label, font_size, font_spacing);

    Vector2 text_pos;
    text_pos.x = bound.x + bound.width/2  - text_size.x/2;
    text_pos.y = bound.y + bound.height/2 - text_size.y/2;

    enum gui_state state = STATE_NORMAL;

    Color bg = background_normal;
    Color fg = foreground_normal;

    if (current_id_disable) {
        bg = background_disable;
        fg = foreground_disable;

    } else if (!invalid_mouse_hover && CheckCollisionPointRec(m, bound)) {
        any_id_hovered = true;
        state = STATE_HOVERED;

        bg = background_hovered;
        fg = foreground_hovered;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            state = STATE_PRESSED;

            bg = background_pressed;
            fg = foreground_pressed;
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            state = STATE_RELEASED;

            bg = background_pressed;
            fg = foreground_pressed;
        } else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            state = STATE_DOWN;

            bg = background_pressed;
            fg = foreground_pressed;
        }
    }

    DrawRectangleRounded(bound, box_roundness, box_segments, bg);
    DrawTextEx(current_text_font, label, text_pos, font_size, font_spacing, fg);

    return state;
}
