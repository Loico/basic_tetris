#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

//Screen dimension constants
#define SCREEN_WIDTH    200
#define SCREEN_HEIGHT   440

#define GRID_WIDTH      10
#define GRID_LENGTH     22

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Event handler
SDL_Event e;

enum pieceMovement { MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT, ROTATE };

int grid[GRID_LENGTH + 4][GRID_WIDTH];
int piece[4][4];
int pieceType;
int posPieceX;
int posPieceY;
int previousTime;
int fallingTime;
bool pause;
bool gameOver;

int typePiece1[4][4] = { 0,1,0,0,
0,1,0,0,
0,1,0,0,
0,1,0,0 };
int typePiece2[4][4] = { 0,0,0,0,
0,2,2,0,
0,2,2,0,
0,0,0,0 };
int typePiece3[4][4] = { 0,0,0,0,
3,3,3,0,
0,3,0,0,
0,0,0,0 };
int typePiece4[4][4] = { 0,0,0,0,
4,4,4,0,
4,0,0,0,
0,0,0,0 };
int typePiece5[4][4] = { 0,0,0,0,
5,5,5,0,
0,0,5,0,
0,0,0,0 };
int typePiece6[4][4] = { 0,0,0,0,
6,6,0,0,
0,6,6,0,
0,0,0,0 };
int typePiece7[4][4] = { 0,0,0,0,
0,7,7,0,
7,7,0,0,
0,0,0,0 };

bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        //Create window
        gWindow = SDL_CreateWindow("TETRIS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0x80);
            }
        }
    }

    return success;
}

void close()
{
    //Destroy window    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void initGrid()
{
    for (int line = GRID_LENGTH - 1; line >= 0; line--)
    {
        for (int column = 0; column<GRID_WIDTH; column++)
        {
            grid[line][column] = 0;
        }
    }
}

void choosePiece()
{
    pieceType = rand() % 7 + 1;
    switch (pieceType)
    {
    case 1:
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                piece[line][column] = typePiece1[line][column];
            }
        }
        break;
    case 2:
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                piece[line][column] = typePiece2[line][column];
            }
        }
        break;
    case 3:
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                piece[line][column] = typePiece3[line][column];
            }
        }
        break;
    case 4:
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                piece[line][column] = typePiece4[line][column];
            }
        }
        break;
    case 5:
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                piece[line][column] = typePiece5[line][column];
            }
        }
        break;
    case 6:
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                piece[line][column] = typePiece6[line][column];
            }
        }
        break;
    case 7:
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                piece[line][column] = typePiece7[line][column];
            }
        }
        break;
    default:
        break;
    }
    posPieceX = (GRID_WIDTH / 2) - 2;
    posPieceY = GRID_LENGTH;
}

void addPiece()
{
    for (int line = 0; line<4; line++)
    {
        for (int column = 0; column<4; column++)
        {
            if (piece[line][column] != 0)
            {
                if (line + posPieceY >= 0 && line + posPieceY < GRID_LENGTH)
                {
                    if (column + posPieceX >= 0 && column + posPieceX < GRID_WIDTH)
                    {
                        grid[line + posPieceY][column + posPieceX] = piece[line][column];
                    }
                }
            }
        }
    }
}

void removePiece()
{
    for (int line = 0; line<4; line++)
    {
        for (int column = 0; column<4; column++)
        {
            if (piece[line][column] != 0)
            {
                if (line + posPieceY >= 0 && line + posPieceY < GRID_LENGTH)
                {
                    if (column + posPieceX >= 0 && column + posPieceX < GRID_WIDTH)
                    {
                        grid[line + posPieceY][column + posPieceX] = 0;
                    }
                }
            }
        }
    }
}

void displayGrid()
{
    //Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0x80);
    SDL_RenderClear(gRenderer);

    SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH / GRID_WIDTH, SCREEN_HEIGHT / GRID_LENGTH };

    for (int line = GRID_LENGTH - 1; line >= 0; line--)
    {
        for (int column = 0; column<GRID_WIDTH; column++)
        {
            fillRect.x = column*(SCREEN_WIDTH / GRID_WIDTH);
            fillRect.y = ((GRID_LENGTH - 1) - line)*(SCREEN_HEIGHT / GRID_LENGTH);
            switch (grid[line][column])
            {
            case 0:
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x80);
                break;
            case 1:
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0x80);
                break;
            case 2:
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0x80);
                break;
            case 3:
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0xFF, 0x80);
                break;
            case 4:
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xA5, 0x00, 0x80);
                break;
            case 5:
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0x80);
                break;
	    case 6:
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0x80);
                break;
            case 7:
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0x80);
                break;
            default:
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x80);
                break;
            }
            SDL_RenderFillRect(gRenderer, &fillRect);
        }
    }

    //Update screen
    SDL_RenderPresent(gRenderer);
}

bool checkColision()
{
    bool res = false;
    for (int line = 0; line<4; line++)
    {
        for (int column = 0; column<4; column++)
        {
            if (piece[line][column] != 0)
            {
                if (line + posPieceY < 0 || column + posPieceX < 0 || column + posPieceX > GRID_WIDTH - 1)
                {
                    res = true;
                    line = 4;
                    column = 4;
                }
                else
                {
                    if (grid[line + posPieceY][column + posPieceX] != 0)
                    {
                        res = true;
                        line = 4;
                        column = 4;
                    }
                    else
                    {
                        res = false;
                    }
                }
            }
        }
    }
    return res;
}

Uint32 checkLines()
{
    Uint32 res = 0;
    bool isCompleteLine;
    for (int line = GRID_LENGTH - 1; line >= 0; line--)
    {
        res <<= 1;
        isCompleteLine = true;
        for (int column = 0; column<GRID_WIDTH; column++)
        {
            if (grid[line][column] == 0)
            {
                isCompleteLine = false;
            }
        }
        if (isCompleteLine)
        {
            res |= 0x0001;
        }
    }
    return res;
}

void eraseLines(Uint32 linesStatus)
{
    Uint32 lineStatus;
    for (int line = 0; line<GRID_LENGTH; line++)
    {
        lineStatus = linesStatus & 0x0001;
        if (lineStatus == 1)
        {
            for (int newLine = line; newLine<GRID_LENGTH - 1; newLine++)
            {
                for (int column = 0; column<GRID_WIDTH; column++)
                {
                    //printf("%d %d ",newLine,column);
                    grid[newLine][column] = grid[newLine + 1][column];
                }
            }
            line--;
        }
        lineStatus = 0;
        linesStatus >>= 1;
    }
    displayGrid();
}

void movePiece(enum pieceMovement movement)
{
    switch (movement)
    {
    case MOVE_DOWN:
        posPieceY--;
        if (checkColision())
        {
            posPieceY++;
            addPiece();
            Uint32 lines = 0;
            lines = checkLines();
            if (lines>0)
            {
                eraseLines(lines);
            }
            if (posPieceY >= GRID_LENGTH - 1)
            {
                gameOver = true;
            }
            choosePiece();
        }
        else
        {
            addPiece();
            displayGrid();
            removePiece();
        }
        break;
    case MOVE_RIGHT:
        posPieceX++;
        if (checkColision())
        {
            posPieceX--;
        }
        addPiece();
        displayGrid();
        removePiece();
        break;
    case MOVE_LEFT:
        posPieceX--;
        if (checkColision())
        {
            posPieceX++;
        }
        addPiece();
        displayGrid();
        removePiece();
        break;
    case ROTATE:
    {
        int oldPiece[4][4];
        int newPiece[4][4];
        int oldLine = 0;
        int oldColumn = 3;
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                oldPiece[line][column] = piece[line][column];
            }
        }
        for (int newLine = 0; newLine<4; newLine++)
        {
            oldLine = 0;
            for (int newColumn = 0; newColumn<4; newColumn++)
            {
                newPiece[newLine][newColumn] = piece[oldLine][oldColumn];
                oldLine++;
            }
            oldColumn--;
        }
        for (int line = 0; line<4; line++)
        {
            for (int column = 0; column<4; column++)
            {
                piece[line][column] = newPiece[line][column];
            }
        }
        
        if (checkColision())
        {
            int nbTest;
            if (pieceType == 1)
            {
                nbTest = 3;
            }
            else
            {
                nbTest = 2;
            }
            int test = 0;
            while (checkColision() && test<nbTest)
            {
                posPieceX++;
                test++;
            }
            if (test >= nbTest)
            {
                test = 0;
                posPieceX = posPieceX - nbTest;
                while (checkColision() && test < nbTest)
                {
                    posPieceX--;
                    test++;
                }
                if (test >= nbTest)
                {
                    posPieceX = posPieceX + nbTest;
                    for (int line = 0; line<4; line++)
                    {
                        for (int column = 0; column<4; column++)
                        {
                            piece[line][column] = oldPiece[line][column];
                        }
                    }
                }
                else
                {
                    addPiece();
                    displayGrid();
                    removePiece();
                }
            }
        }
        else
        {
            addPiece();
            displayGrid();
            removePiece();
        }
        break;
    }
    default:
        break;
    }
}

void checkTime()
{
    int currentTime;
    currentTime = SDL_GetTicks();
    if (currentTime > previousTime + fallingTime)
    {
        previousTime = SDL_GetTicks();

        movePiece(MOVE_DOWN);
    }
}

bool checkKeyboardEvents()
{
    bool quit = false;
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    if (currentKeyStates[SDL_SCANCODE_DOWN])
    {
        fallingTime = 50;
    }
    else
    {
        fallingTime = 500;
    }
    while (SDL_PollEvent(&e) != 0)
    {
        //User requests quit
        if (e.type == SDL_QUIT)
        {
            quit = true;
        }
        else
        {
            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_RIGHT:
                    if (!pause)
                    {
                        movePiece(MOVE_RIGHT);
                    }
                    break;
                case SDLK_LEFT:
                    if (!pause)
                    {
                        movePiece(MOVE_LEFT);
                    }
                    break;
                case SDLK_SPACE:
                    if (!pause)
                    {
                        movePiece(ROTATE);
                    }
                    break;
                case SDLK_ESCAPE:
                    pause ^= 1 ;
                    break;
                default:
                    break;
                }
            }
        }
    }
    return quit;
}

int main(int argc, char* args[])
{
    // Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // Main loop flag
        bool quit = false;

        previousTime = SDL_GetTicks();
        srand(time(NULL));
        fallingTime = 500;
        pause = false;

        while (!quit)
        {
            initGrid();
            choosePiece();
            gameOver = false;
            while (!quit && !gameOver)
            {
                if (!pause)
                {
                    checkTime();
                }
                quit = checkKeyboardEvents();
            }
        }
    // Free resources and close SDL
    close();
    }
    return 0;
}
