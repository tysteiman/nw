#include <ncurses.h>
#include <string.h>

#include "screen.h"
#include "opt.h"

/**
 * Initialize our screen with ncurses and print our file. This
 * will also attach an event listener for handling key presses.
 * To avoid getting to this point and initializing curses, pass
 * the -d flag to our program.
 */
void
initScreen(file_t *file)
{
    /**
     * NCurses config / startup
     */
    initscr();
    getmaxyx(stdscr, screen.height, screen.width);
    raw();
    keypad(stdscr, TRUE);
    noecho();

    line_t *tmp;
    tmp = file->lines;

    /**
     * Print our file's contents to our ncurses window only
     * until we run out of room. I'm not sure if this is the
     * default ncurses behavior but we want to not waste
     * resources trying to print the entire file.
     */
    while (tmp != NULL && tmp->number != screen.height)
        {
            printw("%s", tmp->content);
            tmp = tmp->next;
        }

    /**
     * @TODO this end tag serves as a placeholder for where
     * our status bar can go! that will only need to be one line
     * and can hopefully be swapped out without doing a bunch
     * of horse trash on the side.
     */
    printw("<end>\n");

    /**
     * Place our cursor at the top left of our file ready
     * for navigation.
     */
    move(0, 0);
    refresh();

    attachKeyListener(file);
}

/**
 * Attach keys to our screen functions by parsing keyname(getch()). This
 * essentially starts a REPL that we sit on until the user presses CTRL + Q.
 */
void
attachKeyListener(file_t *file)
{
    while (1)
        {
            char *input;
            char ch = getch();

            /**
             * The actual string value / key name of our input via getch()
             */
            input = keyname(ch);

            /**
             * CTRL + Q : Exit program
             */
            if (!strcmp(input, "^Q"))
                {
                    endwin();
                    break;
                }
            /**
             * CTRL + N : Move down 1 line
             */
            else if (!strcmp(input, "^N"))
                {
                    mvdown(file);
                }
            /**
             * CTRL + P : Move up 1 line
             */
            else if (!strcmp(input, "^P"))
                {
                    mvup(file);
                }
            /**
             * CTRL + F : Move right 1 char or to
             * the beginning of the next line if starting
             * at the end of the line
             */
            else if (!strcmp(input, "^F"))
                {
                    mvright(file);
                }
            /**
             * CTRL + E : Move to end of current line
             */
            else if (!strcmp(input, "^E"))
                {
                    mvendofln(file);
                }
            /**
             * CTRL + B : Move left 1 char or end of
             * previous line if starting at beginning
             * of line.
             */
            else if (!strcmp(input, "^B"))
                {
                    mvleft(file);
                }
            /**
             * CTRL + A : Move to beginning of current line
             */
            else if (!strcmp(input, "^A"))
                {
                    mvbegofline(file);
                }
            /**
             * Handle text input by dumping the char into
             * the buffer.
             * @TODO this should be updated so that only
             * accepted chars are entered to the buffer. For
             * example control characters "^C" etc are being
             * entered into the buffer. Allow anything except
             * for those control chars.
             */
            else
                {
                    printw("%c", ch);
                    refresh();
                }
        }
}

void
mvdown(file_t *file)
{
    if (file->current->number != file->totalLines)
        {
            file->current = file->current->next;
            file->cursor.y++;

            snaptoend(file);
        }

    if (!opts.debug)
        {
            move(file->cursor.y, file->cursor.x);
        }
}

void
mvup(file_t *file)
{
    if (file->current->number != 1)
        {
            file->current = file->current->prev;
            file->cursor.y--;

            snaptoend(file);
        }

    if (!opts.debug)
        {
            move(file->cursor.y, file->cursor.x);
        }
}

/**
 * Snap cursor pos to end of line depending on current position.
 * This is used when navigating up and down lines that are different
 * lengths.
 * @TODO we need to add some kind of a counter or way to keep tabs
 * on the current x value to snap back to. For example, when starting
 * on a line that has 15 characters, navigating down one line that only
 * has 2 lines, then navigating down again to one with 30, it should
 * snap back to x:15 in order to fluidly scroll down the file without
 * crazily snapping down columns and back up.
 */
void
snaptoend(file_t *file)
{
    if ( file->cursor.x > file->current->len ||
         file->cursor.xSnap > file->current->len )
        {
            mvendofln(file);
        }
    else
        {
            file->cursor.x = file->cursor.xSnap;
            move(file->cursor.y, file->cursor.x);
        }
}

void
mvright(file_t *file)
{
    int mv;
    mv = TRUE;

    if (file->cursor.x != file->current->len)
        {
            file->cursor.x++;
            file->cursor.xSnap = file->cursor.x;
        }
    else
        {
            mvdown(file);
            mvbegofline(file);
            mv = FALSE;
        }

    if (!opts.debug && mv)
        {
            move(file->cursor.y, file->cursor.x);
        }
}

void
mvendofln(file_t *file)
{
    if (file->cursor.x != file->current->len)
        {
            file->cursor.x = file->current->len;
        }

    if (!opts.debug)
        {
            move(file->cursor.y, file->current->len);
        }
}

void
mvleft(file_t *file)
{
    int mv;
    mv = TRUE;

    if (file->cursor.x != 0)
        {
            file->cursor.x--;
            file->cursor.xSnap = file->cursor.x;
        }
    else
        {
            mvup(file);
            mvendofln(file);
            mv = FALSE;
        }

    if (!opts.debug && mv)
        {
            move(file->cursor.y, file->cursor.x);
        }
}

void
mvbegofline(file_t *file)
{
    if (file->cursor.x != 0)
        {
            file->cursor.x = 0;
        }

    if (!opts.debug)
        {
            move(file->cursor.y, 0);
        }
}