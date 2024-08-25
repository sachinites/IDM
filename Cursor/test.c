#include <stdio.h>
#include <unistd.h> // For sleep function
#include "cursor.h"

int main() {
    clear_screen();
    sleep(1); // Delay to ensure screen clears

    move_cursor_to(10, 10);
    printf("Hello at (10,10)!");
    fflush(stdout);
    sleep(2); // Pause to see the output

    move_cursor_up(2);
    printf("This is 2 lines up!");
    fflush(stdout);
    sleep(2); // Pause to see the output

    save_cursor_position();
    
    move_cursor_to(20, 20);
    printf("Moved to (20,20)");
    fflush(stdout);
    sleep(2); // Pause to see the output

    restore_cursor_position();
    printf(" Back to saved position!");
    fflush(stdout);
    sleep(2); // Pause to see the output

    return 0;
}
