// cursor.h
#ifndef CURSOR_H
#define CURSOR_H

#include <stdio.h>

// Move cursor up by n lines
void move_cursor_up(int n);

// Move cursor down by n lines
void move_cursor_down(int n);

// Move cursor right by n columns
void move_cursor_right(int n);

// Move cursor left by n columns
void move_cursor_left(int n);

// Move cursor to specific position (row, col)
void move_cursor_to(int row, int col);

// Save the current cursor position
void save_cursor_position();

// Restore the saved cursor position
void restore_cursor_position();

// Clear the screen
void clear_screen();

// Clear from the cursor to the end of the line
void clear_line();

/* Move Cursor to start of current line */
void move_cursor_to_start () ;

#endif // CURSOR_H
