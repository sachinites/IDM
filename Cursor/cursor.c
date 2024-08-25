// cursor.c
#include "cursor.h"

// Move cursor up by n lines
void move_cursor_up(int n) {
    printf("\033[%dA", n);
}

// Move cursor down by n lines
void move_cursor_down(int n) {
    printf("\033[%dB", n);
}

// Move cursor right by n columns
void move_cursor_right(int n) {
    printf("\033[%dC", n);
}

// Move cursor left by n columns
void move_cursor_left(int n) {
    printf("\033[%dD", n);
}

// Move cursor to specific position (row, col)
void move_cursor_to(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

// Save the current cursor position
void save_cursor_position() {
    printf("\033[s");
}

// Restore the saved cursor position
void restore_cursor_position() {
    printf("\033[u");
}

// Clear the screen
void clear_screen() {
    printf("\033[2J");
}

// Clear from the cursor to the end of the line
void clear_line() {
    printf("\033[K");
}
