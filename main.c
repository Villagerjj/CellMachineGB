#include <gb/gb.h>
#include <stdint.h>
#include <string.h>
#include <gb/drawing.h>
#include <stdio.h>
#include "cells.h"
#include "ID.h"
#include "blankscreen.h"
struct cell CellGrid[360]; // world is 20 wide, and 18 tall
struct cell PlayGrid[360]; // the actual area that the cells will interact in...
struct cell SimBuff[360];  // simbuffer - to prevent a tile by tile update
struct cell EmptyCell[1];
uint8_t curx = 0; // x cord in tiles NOT PIXELS
uint8_t cury = 0;
uint8_t selectedCell = 1; // the ID (not tileid) of the cell
BOOLEAN Buildmode = 1;
typedef struct cell
{
    uint8_t Cell_ID;   // what kind of cell are they?
    BOOLEAN canMove;   // used to check if the block can be pushed
    uint8_t direction; // up down, left or right? 0 = right, 1 = down, 2 = left, 3 = up.
    BOOLEAN ticked;    // if the cell was not updated this frame, go ahead and just dump it into the next, this prevents deleteions.
    uint8_t x;         // initial starting position.
    uint8_t y;
} cell;

void Run_Sim()
{
    for (uint16_t i = 0; i < 360; i++)
    {
        PlayGrid[i].ticked = FALSE;
    }
    for (uint8_t x = 0; x < 20; x++)
    {
        for (uint8_t y = 0; y < 18; y++)
        {
            BOOLEAN AllowPush = FALSE;
            BOOLEAN Test = TRUE;
            uint8_t Cursor = 0;
            switch (PlayGrid[y * 20 + x].Cell_ID)
            {
            case Push:
                if (SimBuff[y * 20 + x].ticked == FALSE)
                    SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];

                break;

            case Immobile:
                SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                break;

            case Trash:
                SimBuff[y * 20 + x + 1] = PlayGrid[y * 20 + x + 1];
                SimBuff[y * 20 + x - 1] = PlayGrid[y * 20 + x - 1];
                SimBuff[(y + 1) * 20 + x] = PlayGrid[(y + 1) * 20 + x];
                SimBuff[(y - 1) * 20 + x] = PlayGrid[(y - 1) * 20 + x];
                SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                break;

            case Mover:
                switch (PlayGrid[y * 20 + x].direction)
                {

                case right:

                    while (Test == TRUE)
                    {
                        if (PlayGrid[y * 20 + x + Cursor].Cell_ID != FreeSpace)
                        {
                            if (PlayGrid[y * 20 + x + Cursor].canMove == TRUE)
                            {
                                Cursor++;
                            }
                            else
                            {
                                Test = FALSE;
                            }
                        }
                        else
                        {
                            AllowPush = TRUE;
                            Test = FALSE;
                        }
                    }

                    if (AllowPush == TRUE)
                    {
                        while ((y * 20 + x + Cursor) != (y * 20 + x))
                        {

                            SimBuff[y * 20 + x + Cursor] = PlayGrid[y * 20 + x + (Cursor - 1)];
                            SimBuff[y * 20 + x + Cursor].x = x + Cursor;
                            SimBuff[y * 20 + x + Cursor].ticked = TRUE;
                            SimBuff[y * 20 + x + (Cursor - 1)].Cell_ID = EmptyCell[0].Cell_ID;

                            Cursor--;
                        }
                    }

                    AllowPush = FALSE;
                    Test = TRUE;
                    Cursor = 0;
                    break;

                case down:

                    while (Test == TRUE)
                    {
                        if (PlayGrid[(y + Cursor) * 20 + x].Cell_ID != FreeSpace)
                        {
                            if (PlayGrid[(y + Cursor) * 20 + x].canMove == TRUE)
                            {
                                Cursor++;
                            }
                            else
                            {
                                Test = FALSE;
                            }
                        }
                        else
                        {
                            AllowPush = TRUE;
                            Test = FALSE;
                        }
                    }

                    if (AllowPush == TRUE)
                    {
                        while ((y + Cursor) * 20 + x != y * 20 + x)
                        {

                            SimBuff[(y + Cursor) * 20 + x] = PlayGrid[(y + (Cursor - 1)) * 20 + x];
                            SimBuff[(y + Cursor) * 20 + x].y = y + Cursor;
                            SimBuff[(y + Cursor) * 20 + x].ticked = TRUE;
                            SimBuff[(y + (Cursor - 1)) * 20 + x].Cell_ID = EmptyCell[0].Cell_ID;

                            Cursor--;
                        }
                    }

                    AllowPush = FALSE;
                    Test = TRUE;
                    Cursor = 0;
                    break;

                case left:
                    while (Test == TRUE)
                    {
                        if (PlayGrid[y * 20 + x - Cursor].Cell_ID != FreeSpace)
                        {
                            if (PlayGrid[y * 20 + x - Cursor].canMove == TRUE)
                            {
                                Cursor++;
                            }
                            else
                            {
                                Test = FALSE;
                            }
                        }
                        else
                        {
                            AllowPush = TRUE;
                            Test = FALSE;
                        }
                    }

                    if (AllowPush == TRUE)
                    {
                        while ((y * 20 + x - Cursor) != (y * 20 + x))
                        {

                            SimBuff[y * 20 + x - Cursor] = PlayGrid[y * 20 + x - (Cursor - 1)];
                            SimBuff[y * 20 + x - Cursor].x = x - Cursor;
                            SimBuff[y * 20 + x - Cursor].ticked = TRUE;
                            SimBuff[y * 20 + x - (Cursor - 1)].Cell_ID = EmptyCell[0].Cell_ID;

                            Cursor--;
                        }
                    }

                    AllowPush = FALSE;
                    Test = TRUE;
                    Cursor = 0;
                    break;

                case up:

                    while (Test == TRUE)
                    {
                        if (PlayGrid[(y - Cursor) * 20 + x].Cell_ID != FreeSpace)
                        {
                            if (PlayGrid[(y - Cursor) * 20 + x].canMove == TRUE)
                            {
                                Cursor++;
                            }
                            else
                            {
                                Test = FALSE;
                            }
                        }
                        else
                        {
                            AllowPush = TRUE;
                            Test = FALSE;
                        }
                    }

                    if (AllowPush == TRUE)
                    {
                        while ((y - Cursor) * 20 + x != y * 20 + x)
                        {

                            SimBuff[(y - Cursor) * 20 + x] = PlayGrid[(y - (Cursor - 1)) * 20 + x];
                            SimBuff[(y - Cursor) * 20 + x].y = y - Cursor;
                            SimBuff[(y - Cursor) * 20 + x].ticked = TRUE;
                            SimBuff[(y - (Cursor - 1)) * 20 + x].Cell_ID = EmptyCell[0].Cell_ID;

                            Cursor--;
                        }
                    }

                    AllowPush = FALSE;
                    Test = TRUE;
                    Cursor = 0;
                    break;

                default:
                    break;
                }
                break;

            case Generator:
                switch (PlayGrid[y * 20 + x].direction)
                {

                case right:
                    if (PlayGrid[y * 20 + x + 1].Cell_ID == FreeSpace)
                    {
                        SimBuff[y * 20 + x + 1] = PlayGrid[y * 20 + x - 1];
                        SimBuff[y * 20 + x + 1].x = x + 1;
                        SimBuff[y * 20 + x + 1].ticked = TRUE;
                        SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                    }
                    else
                    {
                        while (Test == TRUE)
                        {
                            if (PlayGrid[y * 20 + x + Cursor].Cell_ID != FreeSpace)
                            {
                                if (PlayGrid[y * 20 + x + Cursor].canMove == TRUE)
                                {
                                    Cursor++;
                                }
                                else
                                {
                                    Test = FALSE;
                                }
                            }
                            else
                            {
                                AllowPush = TRUE;
                                Test = FALSE;
                            }
                        }

                        if (AllowPush == TRUE)
                        {
                            SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                            while ((y * 20 + x + Cursor) != (y * 20 + x + 1))
                            {

                                SimBuff[y * 20 + x + Cursor] = PlayGrid[y * 20 + x + (Cursor - 1)];
                                SimBuff[y * 20 + x + Cursor].x = x + Cursor;
                                SimBuff[y * 20 + x + Cursor].ticked = TRUE;
                                SimBuff[(y + (Cursor - 1)) * 20 + x].Cell_ID = EmptyCell[0].Cell_ID;
                                Cursor--;
                            }
                        }
                    }

                    AllowPush = FALSE;
                    Test = TRUE;
                    Cursor = 0;
                    break;

                case down:
                    if (PlayGrid[(y + 1) * 20 + x].Cell_ID == FreeSpace)
                    {
                        SimBuff[(y + 1) * 20 + x] = PlayGrid[(y - 1) * 20 + x];
                        SimBuff[(y + 1) * 20 + x].y = y + 1;
                        SimBuff[(y + 1) * 20 + x].ticked = TRUE;
                        // SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                    }
                    else
                    {
                        while (Test == TRUE)
                        {
                            if (PlayGrid[(y + Cursor) * 20 + x].Cell_ID != FreeSpace)
                            {
                                if (PlayGrid[(y + Cursor) * 20 + x].canMove == TRUE)
                                {
                                    Cursor++;
                                }
                                else
                                {
                                    Test = FALSE;
                                }
                            }
                            else
                            {
                                AllowPush = TRUE;
                                Test = FALSE;
                            }
                        }

                        if (AllowPush == TRUE)
                        {
                            SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                            while ((y + Cursor) * 20 + x != (y + 1) * 20 + x)
                            {
                                SimBuff[(y + Cursor) * 20 + x] = PlayGrid[(y - (Cursor - 1)) * 20 + x];
                                SimBuff[(y + Cursor) * 20 + x].y = y + Cursor;
                                SimBuff[(y + Cursor) * 20 + x].ticked = TRUE;
                                SimBuff[(y + (Cursor - 1)) * 20 + x].Cell_ID = EmptyCell[0].Cell_ID;
                                Cursor--;
                            }
                        }
                    }
                    AllowPush = FALSE;
                    Test = TRUE;
                    Cursor = 0;
                    break;

                case left:
                    if (PlayGrid[y * 20 + x - 1].Cell_ID == FreeSpace)
                    {
                        SimBuff[y * 20 + x - 1] = PlayGrid[y * 20 + x + 1];
                        SimBuff[y * 20 + x - 1].x = x - 1;
                        SimBuff[y * 20 + x - 1].ticked = TRUE;
                        SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                    }
                    else
                    {
                        while (Test == TRUE)
                        {
                            if (PlayGrid[y * 20 + x - Cursor].Cell_ID != FreeSpace)
                            {
                                if (PlayGrid[y * 20 + x - Cursor].canMove == TRUE)
                                {
                                    Cursor++;
                                }
                                else
                                {
                                    Test = FALSE;
                                }
                            }
                            else
                            {
                                AllowPush = TRUE;
                                Test = FALSE;
                            }
                        }

                        if (AllowPush == TRUE)
                        {
                            SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                            while ((y * 20 + x - Cursor) != (y * 20 + x - 1))
                            {
                                SimBuff[y * 20 + x - Cursor] = PlayGrid[y * 20 + x - (Cursor - 1)];
                                SimBuff[y * 20 + x - Cursor].x = x - Cursor;
                                SimBuff[y * 20 + x - Cursor].ticked = TRUE;
                                SimBuff[(y + (Cursor - 1)) * 20 + x].Cell_ID = EmptyCell[0].Cell_ID;
                                Cursor--;
                            }
                        }
                    }
                    AllowPush = FALSE;
                    Test = TRUE;
                    Cursor = 0;
                    break;

                case up:
                    if (PlayGrid[(y - 1) * 20 + x].Cell_ID == FreeSpace)
                    {
                        SimBuff[(y - 1) * 20 + x] = PlayGrid[(y + 1) * 20 + x];
                        SimBuff[(y - 1) * 20 + x].y = y - 1;
                        SimBuff[(y - 1) * 20 + x].ticked = TRUE;
                        SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                    }
                    else
                    {
                        while (Test == TRUE)
                        {
                            if (PlayGrid[(y - Cursor) * 20 + x].Cell_ID != FreeSpace)
                            {
                                if (PlayGrid[(y - Cursor) * 20 + x].canMove == TRUE)
                                {
                                    Cursor++;
                                }
                                else
                                {
                                    Test = FALSE;
                                }
                            }
                            else
                            {
                                AllowPush = TRUE;
                                Test = FALSE;
                            }
                        }

                        if (AllowPush == TRUE)
                        {
                            SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                            while ((y - Cursor) * 20 + x != (y - 1) * 20 + x)
                            {
                                SimBuff[(y - Cursor) * 20 + x] = PlayGrid[(y + (Cursor - 1)) * 20 + x];
                                SimBuff[(y - Cursor) * 20 + x].y = y - Cursor;
                                SimBuff[(y - Cursor) * 20 + x].ticked = TRUE;
                                SimBuff[(y - (Cursor - 1)) * 20 + x].Cell_ID = EmptyCell[0].Cell_ID;
                                Cursor--;
                            }
                        }
                    }
                    AllowPush = FALSE;
                    Test = TRUE;
                    Cursor = 0;
                    break;

                default:
                    break;
                }
                break;

            case Rotator:
                switch (PlayGrid[y * 20 + x].direction)
                {

                case right: // clock wise
                if (SimBuff[y * 20 + x + 1].direction < 3)
                    {
                        SimBuff[y * 20 + x + 1].direction++;
                    }
                    else
                    {
                        SimBuff[y * 20 + x + 1].direction = 0;
                    }

                    if (SimBuff[y * 20 + x - 1].direction < 3)
                    {
                        SimBuff[y * 20 + x - 1].direction++;
                    }
                    else
                    {
                        SimBuff[y * 20 + x - 1].direction = 0;
                    }

                    if (SimBuff[(y + 1) * 20 + x].direction < 3)
                    {
                        SimBuff[(y + 1) * 20 + x].direction++;
                    }
                    else
                    {
                        SimBuff[(y + 1) * 20 + x].direction = 0;
                    }

                    if (SimBuff[(y - 1) * 20 + x].direction < 3)
                    {
                        SimBuff[(y - 1) * 20 + x].direction++;
                    }
                    else
                    {
                        SimBuff[(y - 1) * 20 + x].direction = 0;
                    }
                    SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                    break;

                case down:
                    if (SimBuff[y * 20 + x + 1].direction > 0)
                    {
                        SimBuff[y * 20 + x + 1].direction--;
                    }
                    else
                    {
                        SimBuff[y * 20 + x + 1].direction = 3;
                    }

                    if (SimBuff[y * 20 + x - 1].direction > 0)
                    {
                        SimBuff[y * 20 + x - 1].direction--;
                    }
                    else
                    {
                        SimBuff[y * 20 + x - 1].direction = 3;
                    }

                    if (SimBuff[(y + 1) * 20 + x].direction > 0)
                    {
                        SimBuff[(y + 1) * 20 + x].direction--;
                    }
                    else
                    {
                        SimBuff[(y + 1) * 20 + x].direction = 3;
                    }

                    if (SimBuff[(y - 1) * 20 + x].direction > 0)
                    {
                        SimBuff[(y - 1) * 20 + x].direction--;
                    }
                    else
                    {
                        SimBuff[(y - 1) * 20 + x].direction = 3;
                    }
                    SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                    break;

                case left:

                    if (SimBuff[y * 20 + x + 1].direction < 3)
                    {
                        SimBuff[y * 20 + x + 1].direction++;
                    }
                    else
                    {
                        SimBuff[y * 20 + x + 1].direction = 0;
                    }

                    if (SimBuff[y * 20 + x - 1].direction < 3)
                    {
                        SimBuff[y * 20 + x - 1].direction++;
                    }
                    else
                    {
                        SimBuff[y * 20 + x - 1].direction = 0;
                    }

                    if (SimBuff[(y + 1) * 20 + x].direction < 3)
                    {
                        SimBuff[(y + 1) * 20 + x].direction++;
                    }
                    else
                    {
                        SimBuff[(y + 1) * 20 + x].direction = 0;
                    }

                    if (SimBuff[(y - 1) * 20 + x].direction < 3)
                    {
                        SimBuff[(y - 1) * 20 + x].direction++;
                    }
                    else
                    {
                        SimBuff[(y - 1) * 20 + x].direction = 0;
                    }
                    SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                    break;

                case up:
                    if (SimBuff[y * 20 + x + 1].direction > 0)
                    {
                        SimBuff[y * 20 + x + 1].direction--;
                    }
                    else
                    {
                        SimBuff[y * 20 + x + 1].direction = 3;
                    }

                    if (SimBuff[y * 20 + x - 1].direction > 0)
                    {
                        SimBuff[y * 20 + x - 1].direction--;
                    }
                    else
                    {
                        SimBuff[y * 20 + x - 1].direction = 3;
                    }

                    if (SimBuff[(y + 1) * 20 + x].direction > 0)
                    {
                        SimBuff[(y + 1) * 20 + x].direction--;
                    }
                    else
                    {
                        SimBuff[(y + 1) * 20 + x].direction = 3;
                    }

                    if (SimBuff[(y - 1) * 20 + x].direction > 0)
                    {
                        SimBuff[(y - 1) * 20 + x].direction--;
                    }
                    else
                    {
                        SimBuff[(y - 1) * 20 + x].direction = 3;
                    }
                    SimBuff[y * 20 + x] = PlayGrid[y * 20 + x];
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }
    }

    memcpy(PlayGrid, SimBuff, 360 * sizeof *SimBuff);
}

void display()
{
    // world view updater
    if (Buildmode == 0)
    {
        for (uint8_t x = 0; x < 20; x++)
        {
            for (uint8_t y = 0; y < 18; y++)
            {
                switch (PlayGrid[y * 20 + x].Cell_ID)
                {

                case FreeSpace:
                    set_bkg_tile_xy(x, y, 0);
                    break;

                case Mover:
                    switch (PlayGrid[y * 20 + x].direction)
                    {
                    case 0:
                        set_tile_xy(x, y, Mover_R_tile);
                        break;
                    case 1:
                        set_tile_xy(x, y, Mover_D_tile);
                        break;
                    case 2:
                        set_tile_xy(x, y, Mover_L_tile);
                        break;
                    case 3:
                        set_tile_xy(x, y, Mover_U_tile);
                        break;
                    }
                    break;

                case Generator:
                    switch (PlayGrid[y * 20 + x].direction)
                    {
                    case 0:
                        set_tile_xy(x, y, Generator_R_tile);
                        break;
                    case 1:
                        set_tile_xy(x, y, Generator_D_tile);
                        break;
                    case 2:
                        set_tile_xy(x, y, Generator_L_tile);
                        break;
                    case 3:
                        set_tile_xy(x, y, Generator_U_tile);
                        break;
                    }
                    break;

                case Rotator:
                    switch (PlayGrid[y * 20 + x].direction)
                    {
                    case 0:
                        set_tile_xy(x, y, Rotator_R_tile);
                        break;
                    case 1:
                        set_tile_xy(x, y, Rotator_L_tile);
                        break;
                    case 2:
                        set_tile_xy(x, y, Rotator_R_tile);
                        break;
                    case 3:
                        set_tile_xy(x, y, Rotator_L_tile);
                        break;

                    default:
                        break;
                    }
                    break;

                case 4:
                    set_tile_xy(x, y, Push_tile);
                    break;
                case 5:
                    switch (PlayGrid[y * 20 + x].direction)
                    {
                    case 0:
                        set_tile_xy(x, y, Slide_LR_tile);
                        break;
                    case 1:
                        set_tile_xy(x, y, Slide_UD_tile);
                        break;
                    case 2:
                        set_tile_xy(x, y, Slide_LR_tile);
                        break;
                    case 3:
                        set_tile_xy(x, y, Slide_UD_tile);
                        break;

                    default:
                        break;
                    }
                    break;

                default:
                    set_tile_xy(x, y, PlayGrid[y * 20 + x].Cell_ID + 8);
                    break;
                }
            }
        }
    }
    else
    {
        move_sprite(0, curx * 8 + 8, cury * 8 + 16);
    }
}

void main(void)
{
    SHOW_BKG;
    SHOW_SPRITES;
    set_sprite_data(0, 18, cells);
    set_bkg_data(0, 18, cells);
    set_bkg_tiles(0, 0, 20, 18, blankscreen);
    set_sprite_tile(0, 17);
    scroll_sprite(0, 8, 16);
    for (uint16_t i = 0; i < 360; i++)
    {
        PlayGrid[i].Cell_ID = FreeSpace;
    }
    for (uint16_t i = 0; i < 360; i++)
    {
        SimBuff[i].Cell_ID = FreeSpace;
    }

    while (1)
    {
        uint8_t cur = joypad();

        if (cur & J_A)
        {
            CellGrid[cury * 20 + curx].Cell_ID = selectedCell;
            CellGrid[cury * 20 + curx].direction = 0;

            CellGrid[cury * 20 + curx].x = curx;
            CellGrid[cury * 20 + curx].y = cury;

            switch (selectedCell)
            {
            case 1:
                set_tile_xy(curx, cury, Mover_R_tile);
                CellGrid[cury * 20 + curx].canMove = TRUE;
                break;

            case 2:
                set_tile_xy(curx, cury, Generator_R_tile);
                CellGrid[cury * 20 + curx].canMove = TRUE;
                break;

            case 3:
                set_tile_xy(curx, cury, Rotator_R_tile);
                CellGrid[cury * 20 + curx].canMove = TRUE;
                break;

            case 4:
                set_tile_xy(curx, cury, Push_tile);
                CellGrid[cury * 20 + curx].canMove = TRUE;
                break;
            case 5:
                set_tile_xy(curx, cury, Slide_LR_tile);
                CellGrid[cury * 20 + curx].canMove = TRUE;
                break;

            case Immobile:
                set_tile_xy(curx, cury, selectedCell + 8);
                CellGrid[cury * 20 + curx].canMove = FALSE;
                break;

            case Trash:
                set_tile_xy(curx, cury, selectedCell + 8);
                CellGrid[cury * 20 + curx].canMove = TRUE;
                break;

            default:
                set_tile_xy(curx, cury, selectedCell + 8);
                break;
            }
        }

        if (cur & J_B)
        {
            if (CellGrid[cury * 20 + curx].direction < 3)
            {
                CellGrid[cury * 20 + curx].direction++;
            }
            else
            {
                CellGrid[cury * 20 + curx].direction = 0;
            }

            switch (CellGrid[cury * 20 + curx].Cell_ID)
            {
            case Mover:
                switch (CellGrid[cury * 20 + curx].direction)
                {
                case 0:
                    set_tile_xy(curx, cury, Mover_R_tile);
                    break;
                case 1:
                    set_tile_xy(curx, cury, Mover_D_tile);
                    break;
                case 2:
                    set_tile_xy(curx, cury, Mover_L_tile);
                    break;
                case 3:
                    set_tile_xy(curx, cury, Mover_U_tile);
                    break;

                default:
                    break;
                }
                break;

            case Generator:

                switch (CellGrid[cury * 20 + curx].direction)
                {
                case 0:
                    set_tile_xy(curx, cury, Generator_R_tile);
                    break;
                case 1:
                    set_tile_xy(curx, cury, Generator_D_tile);
                    break;
                case 2:
                    set_tile_xy(curx, cury, Generator_L_tile);
                    break;
                case 3:
                    set_tile_xy(curx, cury, Generator_U_tile);
                    break;
                }
                break;

            case Slide:
                switch (CellGrid[cury * 20 + curx].direction)
                {
                case 0:
                    set_tile_xy(curx, cury, Slide_LR_tile);
                    break;
                case 1:
                    set_tile_xy(curx, cury, Slide_UD_tile);
                    break;
                case 2:
                    set_tile_xy(curx, cury, Slide_LR_tile);
                    break;
                case 3:
                    set_tile_xy(curx, cury, Slide_UD_tile);
                    break;

                default:
                    break;
                }
                break;

            case Rotator:
                switch (CellGrid[cury * 20 + curx].direction)
                {
                case 0:
                    set_tile_xy(curx, cury, Rotator_R_tile);
                    break;
                case 1:
                    set_tile_xy(curx, cury, Rotator_L_tile);
                    break;
                case 2:
                    set_tile_xy(curx, cury, Rotator_R_tile);
                    break;
                case 3:
                    set_tile_xy(curx, cury, Rotator_L_tile);
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        if (cur & J_SELECT)
        {
            if (selectedCell < 17)
            {
                selectedCell++;
            }
            else
            {
                selectedCell = 1;
            }
        }

        if (cur & J_START)
        {
            // uint8_t tempmode = Buildmode;

            if (Buildmode == 1)
            {
                Buildmode = 0;

                memcpy(PlayGrid, CellGrid, 360 * sizeof *CellGrid);
            }
        }
        //  STARTSKIP:
        if (cur & J_UP)
        {
            cury -= 1;
        }
        else if (cur & J_DOWN)
        {
            cury += 1;
        }

        if (cur & J_LEFT)
        {
            curx -= 1;
        }
        else if (cur & J_RIGHT)
        {
            curx += 1;
        }

        if (Buildmode == 1)
        {
            delay(100);
        }
        else
        {

            Run_Sim();
            delay(100);
        }
        display();
    }
}
