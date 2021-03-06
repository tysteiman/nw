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

#include <stdio.h>

#include "../lib.h"
#include "../opt.h"

#include "test.h"

void
deleteCharTest(file_t *file)
{
    /**
     * By hard coding a new file load we can anticipate the exact file that's being tested
     * instead of guess scenarios that we have to code around. This is probably the best
     * way to do test multiple functional elements at the same time and actual assert
     * real file content.
     */
    loadFile(file, NW_TEST_FILE);

    NW_MOVE_DOWN(); NW_MOVE_DOWN(); NW_MOVE_DOWN(); NW_MOVE_DOWN();
    NW_MOVE_DOWN(); NW_MOVE_DOWN(); NW_MOVE_DOWN(); NW_MOVE_DOWN();

    NW_DEL();

    NW_MOVE_RIGHT(); NW_MOVE_RIGHT();

    NW_MOVE_BEG();

    NW_INS('z');

    NW_ASSERT_STR(file->current->content, "zdefine NW_LIB", "String should match input");
    
    NW_MOVE_DOWN(); NW_MOVE_DOWN();
    
    NW_MOVE_END();
    
    NW_MOVE_LEFT(); NW_MOVE_LEFT();
    
    NW_INS('T'); NW_INS('E'); NW_INS('S'); NW_INS('T'); NW_INS(' ');
    
    NW_ASSERT_STR(file->current->content, "/* CONSTANTS TEST */", "String should match input");
    
    NW_MOVE_UP();
    
    NW_INS('/'); NW_INS('/'); NW_INS(' '); NW_INS('q'); NW_INS('!');
    
    NW_ASSERT_STR(file->current->content, "// q!", "New line's content should equal // q!");
}

void
moveAndInsertCharDebug(file_t *file)
{
    NW_MOVE_DOWN();
    NW_MOVE_RIGHT();
    NW_MOVE_RIGHT();

    lineInsertChar('z', &file->current->content[0], &CURSOR.x, &file->current->len);

    NW_MOVE_DOWN();
    
    lineInsertChar('t', &file->current->content[0], &CURSOR.x, &file->current->len);
    lineInsertChar('y', &file->current->content[0], &CURSOR.x, &file->current->len);
    lineInsertChar('l', &file->current->content[0], &CURSOR.x, &file->current->len);
    lineInsertChar('e', &file->current->content[0], &CURSOR.x, &file->current->len);
    lineInsertChar('r', &file->current->content[0], &CURSOR.x, &file->current->len);
}

void
substrTest(file_t *file)
{
    char * str = "Scuzbucket";

    char * res = substr(0, 3, str);
    NW_ASSERT_STR(res, "Scuz", "Substring 0-3 of Scuzbucket is Scuz");

    char * end = substr(4, 9, str);
    NW_ASSERT_STR(end, "bucket", "Substring 4-9 of Scuzbucket is bucket");
}

void
moveRightTest(file_t *file)
{
    /* test with cursor at 0 */
    int x = CURSOR.x;
    lineMoveRight(file->current->len, &CURSOR);
    if (file->current->len > 0)
        {
            NW_ASSERT(CURSOR.x, ++x, "Moving right moves CURSOR.x by 1");
        }
}

void
moveLeftTest(file_t *file)
{
    /* test at beginning of line (0) */
    int x = CURSOR.x;
    // moveLeft(&CURSOR);
    NW_MOVE_LEFT();
    NW_ASSERT(CURSOR.x, x, "Moving left at pos 0 shouldn't move cursor.");

    /* test after moving */
    NW_MOVE_RIGHT();
    NW_MOVE_RIGHT();
    NW_MOVE_RIGHT();
    NW_MOVE_LEFT();
    if (file->current->len > 0)
        {
            // NW_ASSERT(CURSOR.x, 2, "Moving right 3 times then left should make x = 2");
        }
}

void
moveToLineEndTest(file_t *file)
{
    int expect = file->current->len == 0 ? 0 : file->current->len;
    NW_MOVE_END();
    NW_ASSERT(CURSOR.x, expect, "Moving to end of line places x at end");
}

void
moveToLineBegTest(file_t *file)
{
    /* test before moving */
    NW_MOVE_BEG();
    NW_ASSERT(CURSOR.x, 0, "Moving to beginning of line while at 0 doesn't move cursor");
    
    /* test after moving */
    NW_MOVE_RIGHT();
    NW_MOVE_RIGHT();
    NW_MOVE_RIGHT();
    NW_MOVE_BEG();
    NW_ASSERT(CURSOR.x, 0, "Moving to beginning of the line places x at 0");
}

void
clearLineTest(file_t * file)
{
    loadFile(file, NW_TEST_FILE);

    NW_MOVE_RIGHT(); NW_MOVE_RIGHT();

    NW_CLEAR_LINE();

    NW_ASSERT_STR(file->current->content, "", "Cleared line is empty");
    NW_ASSERT(file->current->len, 0, "Current line len is now 0");
    NW_ASSERT(CURSOR.x, 0, "Cursor x is at 0");
    NW_ASSERT(CURSOR.xSnap, 0, "xSnap is at 0");
}
