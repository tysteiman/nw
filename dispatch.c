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
     * @TODO here we need to determine which type of char it is
     *       for example is the first char ^ or some other control
     *       character? if so don't insert it into the buffer!
     */
    NW_INS(c);
    screenInsertChar();
}

void
dispatchDeleteChar(file_t *file)
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
    newLine(file);
    screenNewLine(CURRENT->content);
}

void
dispatchDeleteLine(file_t *file)
{
    /**
     * If line has length delete all chars of line but leave line
     * present in order to keep typing or something like that.
     */
    if (CURRENT->len > 0)
        {
            int i = 0;
            int len = CURRENT->len + 1;

            NW_MOVE_BEG();
            screen.move_beg();

            for (; i <= len; i++)
                {
                    NW_DEL();
                    screenDeleteChar(CURRENT->content);
                }
        }
    /**
     * If line doesn't have any length, delete the char (line) all together
     */
    else
        {
            dispatchDeleteChar(file);
        }
}
