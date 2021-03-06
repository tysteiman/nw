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
 *
 *   dispatch.c
 *
 *   This file is responsible for coordinating all movements and interaction
 *   between file / line and screen. Right now our goal is to only work in
 *   a terminal but if we want to make a GUI runtime just for fun we would need
 *   to have file completely separated from screen. Hopefully only this file
 *   would change in the sense that we could just call the correct front end
 *   function.
 *
 *   @NOTE nothing in this file should be changing file contents or even
 *         manipulate screen items (NW_CURY), etc.
 */

#include "dispatch.h"
#include "screen.h"

void
dispatchRight(file_t *file)
{
    if (NW_CURX != file->current->len)
        {
            NW_MOVE_RIGHT();
            screen.move_right();
        }
}

void
dispatchLeft(file_t *file)
{
    if (NW_CURX != 0)
        {
            NW_MOVE_LEFT();
            screen.move_left();
        }
}

void
dispatchEnd(file_t *file)
{
    NW_MOVE_END();
    screen.move_end();
}

void
dispatchBeg(file_t *file)
{
    NW_MOVE_BEG();
    screen.move_beg();
}

void
dispatchInsert(char c, file_t *file)
{
    /**
     * Even though we check for screen resize in nw.c, the way we're dumping
     * characters into the buffer allows for (int) c -1 to be inserted which
     * is blowing up the screen and creating a sig fault. This is a quick way to
     * block that from happening since none of the cars we should ever use is -1...
     */
    if (!NW_IS_JUNK_KEY())
        {
            NW_INS(c);
            screenInsertChar();
        }
}

void
dispatchDeleteChar(file_t *file)
{
    if (file->totalLines > 1 || CURRENT->len > 0)
        {
            if (CURRENT->len > 0)
                {
                    if (NW_CURX != CURRENT->len)
                        {
                            NW_DEL();
                            screenDeleteChar(CURRENT->content);
                        }
                }
            else
                {
                    joinLine(file);
                    screenDeleteLine();

                    if (CURRENT->number == 1)
                        {
                            clear();
                            NW_PRINT(CURRENT);
                        }

                    NW_MOVE_TO_CUR();
                }
        }
}

void
dispatchDown(file_t *file)
{
    if (CURRENT->number != file->totalLines)
        {
            if (screen.cury != screen.maxy - 1)
                {
                    NW_MOVE_DOWN();
                    screen.move_down();
                }
            else
                {
                    screenScrollDown(file);
                }
        }
}

void
dispatchUp(file_t *file)
{
    /* may be redundant but should be the same */
    if (CURRENT->number != 1)
        {
            NW_MOVE_UP();

            if (NW_CURY == 0)
                {
                    screenScrollUp(CURRENT);
                }
            else
                {
                    screen.move_up();
                }
        }
}

void
dispatchNewLine(file_t *file)
{
    /* move to beg to support new line from anywhere in line */
    NW_MOVE_BEG();
    screen.move_beg();

    /* execute actual new line */
    if (NW_CURY != NW_MAXY)
        {
            newLine(file);
            screenNewLine(CURRENT->content);
        }
    else
        {
            dispatchDown(file);
            dispatchSplitLine(file);
            dispatchUp(file);
        }
}

void
dispatchDeleteLine(file_t *file)
{
    if (file->totalLines > 1)
        {
            /**
             * If line has length delete all chars of line but leave line
             * present in order to keep typing or something like that.
             */
            if (CURRENT->len > 0)
                {
                    int i = 0;
                    int len = CURRENT->len + 1;

                    /**
                     * Move to beginning of line & start deleting characters
                     */
                    NW_MOVE_BEG();
                    screen.move_beg();

                    for (; i <= len; i++)
                        {
                            NW_DEL();
                        }
                    /**
                     * Instead of deleting off the screen one at a time we can use
                     * clrtoeol() to just wipe out the line all together.
                     */
                    clrtoeol();
                }
            dispatchDeleteChar(file);
        }
}

void
dispatchSplitLine(file_t *file)
{
    splitLine(file);
    screenSplitLine(file);
}

void
dispatchJoinLine(file_t *file)
{
    if (NW_CURY != 0)
        {
            joinLine(file);
            screenJoinLine(file);
        }
}

/**
 * For dispatch paging commands i prefer for the cursor to be at
 * natural scroll point after paging, i.e. if we are paging down i want
 * my cursor to be at the top of the screen in order to quickly move. Pressing
 * scroll down again should just shift to the next page. This is the same
 * with scroll up however the cursor scoots to the bottom instead.
 */
void
dispatchPageDown(file_t *file)
{
    int cur = NW_CURY;

    dispatchBeg(file);

    for (; cur < screen.maxy; cur++)
        {
            dispatchDown(file);
        }
}

void
dispatchPageUp(file_t *file)
{
    int cur = NW_CURY;

    dispatchBeg(file);

    for (; cur >= 0; cur--)
        {
            dispatchUp(file);
        }
}
