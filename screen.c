/**
 *   NW Editor - Super light terminal based editor
 *   Copyright (C) 2017 Tyler Steiman
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ncurses.h>
#include <string.h>

#include "screen.h"

/**
 * Dump our file's contents onto our screen. This should be the only
 * front end function we're not really worried about because we really just
 * want to get the text loaded on the screen, and standing by for user input.
 * After that point we need to dispatch the commands between backend and frontend.
 */
void
initScreen(file_t *file)
{
    screen.move_left = screenMoveLeft;
    screen.move_right = screenMoveRight;
    screen.move_up = screenMoveUp;
    screen.move_down = screenMoveDown;
    screen.move_end = screenMoveEnd;
    screen.move_beg = screenMoveBeg;
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    getmaxyx(stdscr, screen.maxy, screen.maxx);

    line_t *head = file->current;

    if (file->name == NW_EMPTY_FILE)
        {
            char *msg = "THE NW EDITOR";
            screen.init_screen = TRUE;
            NW_PRINT_CENTER(msg);
        }
    else
        {
            NW_PRINT(file->current);
        }

    // NW_PRINT(file->current);

    screen.cury = 0;
    screen.curx = 0;

    NW_MOVE_TO_CUR();

    refresh();
}

void
printLine(char *head)
{
  int i = 0;
  int x = 0;

  if (NW_AT_MAXX && !NW_AT_END)
    {
      x = NW_CURX - 1;
    }

  /**
   * @NOTE It looks like to fix this we need to track both i (incrementer) and
   *       a general offset starting at 0 for working offset (0) < NW_MAXX.
   *       in other words, we need to have an index of which position to actually
   *       print off of and an index to know when to stop printing the particular
   *       line
   * @QUESTION will the system mentioned above actually working when lines are
   *           wrapped multiple times? This is something we want to think about
   *           before building out the entire functionality and only being able
   *           to navigate a single wrap.
   * @TODO we will also have to build the same functionality for moving BACK.
   */
  for (; i < NW_MAXX - 1; ++i)
      {
        printw("%c", head[x]);
        ++x;
      }
  printw("\n");
  refresh();
}

void
printLines(line_t *head, int totalLines)
{
    int scrcury = screen.cury;
    while(head != NULL && NW_CURY != screen.maxy)
        {
            printLine(head->content);
            head = head->next;
            ++screen.cury;
        }
    screen.cury = scrcury;
}

/**
 * @TODO Also here only print up until NW_MAXX. We will probably have to check
 *       where the user is in terms of which 'section' of the line we are printing
 *       if that makes sense. This needs a rather complex setup in order to work.
 */
void
reprintLine(char *cur)
{
    move(screen.cury, 0);
    clrtoeol();
    printLine(cur);
    move(screen.cury, screen.curx);
}

void
resizeScreen(file_t *file)
{
    clear();
    getmaxyx(stdscr, screen.maxy, screen.maxx);
    NW_PRINT(file->current);

    screen.cury = 0;
    screen.curx = 0;

    NW_CUR_SYNC();
}

void
screenDeleteChar(char *cur)
{
    reprintLine(cur);
}

void
screenInsertChar()
{
    reprintLine(file.current->content);
    ++screen.curx;
    NW_MOVE_TO_CUR();
}

void
screenMoveRight()
{
    if (NW_AT_MAXX && !NW_AT_END)
        {
            reprintLine(file.current->content);
        }

    ++screen.curx;

    NW_MOVE_TO_CUR();
}

void
screenMoveLeft()
{
    --screen.curx;

    NW_MOVE_TO_CUR();
}

void
moveToCursor()
{
    move(screen.cury, screen.curx);
    refresh();
}

void
screenNewLine(char *cur)
{
    printLines(file.current->prev, file.totalLines);
    ++screen.cury;
    NW_MOVE_TO_CUR();
}

/**
 * @TODO for any line movement commands we have to check to see if the line length
 *       is longer than maxx aka is the line wrapped? if it is we want to display
 *       enough chars to be visible.
 * @TODO do we want word wrap to be slow scroll one more char at a time or all the
 *       way at the next 'iteration' of the line?
 */
void
screenMoveEnd()
{
    screen.curx = file.current->len;
    NW_MOVE_TO_CUR();
}

void
screenMoveBeg()
{
    screen.curx = 0;
    NW_MOVE_TO_CUR();
}

void
screenMoveDown()
{
    ++NW_CURY;
    NW_CURX = file.cursor.x;
    NW_MOVE_TO_CUR();
}

void
screenMoveUp()
{
    if (NW_CURY != 0)
        {
            --NW_CURY;
            NW_CURX = file.cursor.x;
            NW_MOVE_TO_CUR();
        }
}

void
screenDeleteLine()
{
    if (NW_CURY != 0)
        {
            clrtobot();
            printLines(file.current->next, file.totalLines);
            --NW_CURY;
            NW_CURX = file.current->len;
            NW_MOVE_TO_CUR();
        }
}

void
screenScrollDown(file_t *file)
{
    clear();
    NW_MOVE_DOWN();
    NW_CURY = 0;
    NW_PRINT(CURRENT);
    NW_MOVE_TO_CUR();
}

void
screenScrollUp(line_t *head)
{
    int curx = NW_CURX;
    clear();

    NW_CURY = NW_MAXY;

    while (NW_CURY != 0)
        {
            head = head->prev;
            --NW_CURY;
        }

    printLines(head, file.totalLines);

    NW_CURY = NW_MAXY;
    NW_MOVE_TO_CUR();

    refresh();
}

void
screenSplitLine(file_t *file)
{
    move(NW_CURY, 0);
    NW_PRINT(CURRENT->prev);

    ++NW_CURY;
    NW_CURX = 0;

    NW_MOVE_TO_CUR();
}

void
screenJoinLine(file_t *file)
{
    move(--NW_CURY, 0);

    clrtobot();

    NW_PRINT(CURRENT);

    NW_MOVE_TO_CUR();
}
