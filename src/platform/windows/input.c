#ifdef __WIN32
#include "platform/input.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include <windows.h>
#include "platform/terminal.h"
#include "core/shell_print.h"

static const char *event_to_string(DWORD eventType) {
    switch (eventType) {
        case KEY_EVENT:            return "KEY_EVENT";
        case MOUSE_EVENT:          return "MOUSE_EVENT";
        case WINDOW_BUFFER_SIZE_EVENT: return "WINDOW_BUFFER_SIZE_EVENT";
        case MENU_EVENT:           return "MENU_EVENT";
        case FOCUS_EVENT:          return "FOCUS_EVENT";
        default:                   return "UNKNOWN_EVENT";
    }
}

static void dump_key_event(KEY_EVENT_RECORD key) {
    LOG_DEBUG("KEY_EVENT: bKeyDown=%d, wRepeatCount=%d, wVirtualKeyCode=%d, wVirtualScanCode=%d",
              key.bKeyDown, key.wRepeatCount, key.wVirtualKeyCode, key.wVirtualScanCode);

    LOG_DEBUG("AsciiChar=%c (0x%02X), UnicodeChar=%c (0x%04X)",
              key.uChar.AsciiChar >= 32 ? key.uChar.AsciiChar : ' ',
              (unsigned char)key.uChar.AsciiChar,
              key.uChar.UnicodeChar >= 32 ? key.uChar.UnicodeChar : ' ',
              (unsigned int)key.uChar.UnicodeChar);

    LOG_DEBUG("ControlKeyState: 0x%08X (%s%s%s%s%s%s%s%s)",
              key.dwControlKeyState,
              (key.dwControlKeyState & RIGHT_ALT_PRESSED)   ? "RIGHT_ALT "   : "",
              (key.dwControlKeyState & LEFT_ALT_PRESSED)    ? "LEFT_ALT "    : "",
              (key.dwControlKeyState & RIGHT_CTRL_PRESSED)  ? "RIGHT_CTRL "  : "",
              (key.dwControlKeyState & LEFT_CTRL_PRESSED)   ? "LEFT_CTRL "   : "",
              (key.dwControlKeyState & SHIFT_PRESSED)       ? "SHIFT "       : "",
              (key.dwControlKeyState & NUMLOCK_ON)          ? "NUMLOCK "     : "",
              (key.dwControlKeyState & CAPSLOCK_ON)         ? "CAPSLOCK "    : "",
              (key.dwControlKeyState & ENHANCED_KEY)        ? "ENHANCED "    : ""
    );
}

KeyEvent get_key_event() {
    HANDLE hIn = get_input_handle();
    INPUT_RECORD rec = {0};
    DWORD read_bytes;

    while (1) {
        if (ReadConsoleInput(hIn, &rec, 1, &read_bytes) == 0) {
            LOG_ERROR("ReadConsoleInput failed");
            return (KeyEvent) {.type = KEY_UNKNOWN };
        }

        LOG_DEBUG("Received %s event", event_to_string(rec.EventType));

        if (rec.EventType != KEY_EVENT) continue;

        KEY_EVENT_RECORD key = rec.Event.KeyEvent;

        dump_key_event(key);

        if (!key.bKeyDown) continue;

        bool ctrl = key.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);

        switch (key.wVirtualKeyCode) {
            case VK_BACK:
                return (KeyEvent) {.type = KEY_BACKSPACE };

            case VK_RETURN:
                return (KeyEvent) {.type = KEY_ENTER };

            case VK_UP:
                if (ctrl)
                    return (KeyEvent){ .type = KEY_CTRL_ARROW_UP };
                return (KeyEvent){ .type = KEY_ARROW_UP };

            case VK_DOWN:
                if (ctrl)
                    return (KeyEvent){ .type = KEY_CTRL_ARROW_DOWN };
                return (KeyEvent){ .type = KEY_ARROW_DOWN };

            case VK_RIGHT:
                if (ctrl)
                    return (KeyEvent){ .type = KEY_CTRL_ARROW_RIGHT };
                return (KeyEvent){ .type = KEY_ARROW_RIGHT };

            case VK_LEFT:
                if (ctrl)
                    return (KeyEvent){ .type = KEY_CTRL_ARROW_LEFT };
                return (KeyEvent){ .type = KEY_ARROW_LEFT };

            case VK_DELETE:
                if (ctrl)
                    return (KeyEvent){ .type = KEY_CTRL_DELETE };
                return (KeyEvent){ .type = KEY_DELETE };

            default:
                char ch = key.uChar.AsciiChar;
                if (ch == 8 && ctrl)
                    return (KeyEvent){ .type = KEY_CTRL_BACKSPACE }; // Why windows, just why

                if (ch >= 32 && ch <= 126)
                    return (KeyEvent){ .type = KEY_CHAR, .ch = ch };

                if (key.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED) && ch == 3)
                    return (KeyEvent) {.type = KEY_CTRL_C };
                break;
            }

        return (KeyEvent){ .type = KEY_UNKNOWN };
    }
}

#endif
