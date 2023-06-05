#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>

//additional header files
#include "Food.h"
#include "SnakeComponent.h"
#include "Vector2.h"

bool Initialize();
void RunLoop();
void ShutDown();

void ProcessInput();
void UpdateGame();
void GenerateOutput();
void DisplayText();
void InitSDLText();
void StartingText();
void EndingText();
void ResetGame();

class Game {
    public:
        Game():mWindow(nullptr), mIsRunning(true) {
            mTicksCount = 0;
        }

        ~Game() { }

        bool Initialize();
        void RunLoop();
        void ShutDown();

    private:
        //used to track state of snake
        enum State {
            EActive,
            EPaused,
            EDead
        };

        //used to track direction of snake
        enum Direction {
            Up,
            Down,
            Right,
            Left
        };

        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;

        Uint32 mTicksCount;

        Direction mSnakeDir;

        Vector2 foodPosition = Vector2(96,160);
        Vector2 snakeHeadPos = Vector2(0,0), snakeTailPos = Vector2(0,0);

        float thickness = 16.0f, deltaTime, moveSpeed = 100.0f, counter1, counter2;

        int playerScore = 0, resetClicks = 0;
        
        bool mIsRunning, startGame = false, gameOver = false;

        std::vector<SnakeComponent> snake;

        std::string playerScoreStr;

        Vector2 mSnakePos = Vector2(512,384);

        void ProcessInput();
        void UpdateGame();
        void GenerateOutput();
        void DisplayText();
        void InitSDLText();
        void StartingText();
        void EndingText();
        void ResetGame();
};

void Game::InitSDLText() {
    //initialize, else load error
    if(TTF_Init() == -1) {
        std::cout << "Failed to initialize text.Error: " << TTF_GetError() << std::endl;
    }
}

void Game::ResetGame() {
    //reset variables
    gameOver = false;
    startGame = false;
    resetClicks = 0;
    playerScore = 0;
    mSnakePos = Vector2(512,384);
    snake.clear();
}

void Game::StartingText() {
    SDL_SetRenderDrawColor( //pointer, RGBA
            mRenderer,
            0,
            0,
            0,
            0
        );

        SDL_RenderClear(mRenderer);

        //draw game scene
        SDL_SetRenderDrawColor( //pointer, RGBA
            mRenderer,
            255,
            255,
            255,
            255
        );

        TTF_Font* ourFont;
        SDL_Surface * titleSurfaceText;	
        SDL_Surface * optionSurfaceText;
        SDL_Texture * titleTexture;
        SDL_Texture * optionTexture;

        SDL_Rect titleText = {340,100,300,200};
        SDL_Rect optionText = {250,400,500,100};

        //load font file and set size
        ourFont = TTF_OpenFont("AldotheApache.ttf",1000);

        //confirm font loaded
        if(ourFont == nullptr) {
            std::cout << "Could not load font" << std::endl;
            exit(1);
        }

        SDL_Color textColor = {255,255,255,255};

        //pixels from text
        titleSurfaceText = TTF_RenderText_Solid(ourFont,"SNAKE",textColor);
        optionSurfaceText = TTF_RenderText_Solid(ourFont,"HIT SPACEBAR TO PLAY",textColor);

        //set up texture
        titleTexture = SDL_CreateTextureFromSurface(mRenderer,titleSurfaceText);
        optionTexture = SDL_CreateTextureFromSurface(mRenderer,optionSurfaceText);

        SDL_FreeSurface(titleSurfaceText);
        SDL_FreeSurface(optionSurfaceText);

        SDL_RenderCopy(mRenderer,titleTexture,NULL,&titleText);
        SDL_RenderCopy(mRenderer,optionTexture,NULL,&optionText);

        //destroy texture
        SDL_DestroyTexture(titleTexture);
        SDL_DestroyTexture(optionTexture);

        //close font
        TTF_CloseFont(ourFont);

        SDL_RenderPresent(mRenderer); //swap front and back buffers
}

void Game::EndingText() {
        SDL_SetRenderDrawColor( //pointer, RGBA
            mRenderer,
            0,
            0,
            0,
            0
        );

        SDL_RenderClear(mRenderer);

        //draw game scene
        SDL_SetRenderDrawColor( //pointer, RGBA
            mRenderer,
            255,
            255,
            255,
            255
        );
        TTF_Font* ourFont;
        SDL_Surface * titleSurfaceText;	
        SDL_Surface * optionSurfaceText;
        SDL_Texture * titleTexture;
        SDL_Texture * optionTexture;

        SDL_Rect titleText = {340,100,300,200};
        SDL_Rect optionText = {250,400,500,100};

        //load font file and set size
        ourFont = TTF_OpenFont("AldotheApache.ttf",1000);

        //confirm font loaded
        if(ourFont == nullptr) {
            std::cout << "Could not load font" << std::endl;
            exit(1);
        }

        SDL_Color textColor = {255,255,255,255};

        //pixels from text
        titleSurfaceText = TTF_RenderText_Solid(ourFont,"SNAKE",textColor);
        optionSurfaceText = TTF_RenderText_Solid(ourFont,"EVERYONE WINS! A FOR EFFORT",textColor);

        //set up texture
        titleTexture = SDL_CreateTextureFromSurface(mRenderer,titleSurfaceText);
        optionTexture = SDL_CreateTextureFromSurface(mRenderer,optionSurfaceText);

        SDL_FreeSurface(titleSurfaceText);
        SDL_FreeSurface(optionSurfaceText);

        SDL_RenderCopy(mRenderer,titleTexture,NULL,&titleText);
        SDL_RenderCopy(mRenderer,optionTexture,NULL,&optionText);

        //destroy texture
        SDL_DestroyTexture(titleTexture);
        SDL_DestroyTexture(optionTexture);

        //close font
        TTF_CloseFont(ourFont);

        SDL_RenderPresent(mRenderer); //swap front and back buffers
        
}

bool Game::Initialize() {
    //random number generator seed
    srand (time(NULL));

    //initialize SDL library
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);

    //initialize text
    InitSDLText();

    if(sdlResult != 0) {
        //initialization failed
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    //if initialization successful, create window
    mWindow = SDL_CreateWindow(
        "Snake",
        100,
        100,
        1024,
        768,
        0 // Flags
    );

    if(!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC //initialization flags. use accelerated renderer and enable vsync
    );

    ResetGame();

    //if window and initialization successful, return true
    return true;
}

/* 
* Step 1: Instantiate head + initiate movement
* Step 2: Instantiate head + implement smooth movement
*/

void Game::RunLoop() {
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::DisplayText() {
    //add text
    TTF_Font* ourFont;
    SDL_Surface * playerSurfaceText;	
    SDL_Surface * AISurfaceText;
    SDL_Texture * playerTexture;
    SDL_Texture * AITexture;

    SDL_Rect playerScoreText = {950,25,50,50};

    //load font file and set size
    ourFont = TTF_OpenFont("AldotheApache.ttf",1000);

    //confirm font loaded
    if(ourFont == nullptr) {
        std::cout << "Could not load font" << std::endl;
        exit(1);
    }

    SDL_Color textColor = {255,255,255,255};

    playerScoreStr = std::to_string(playerScore);
    char const *pchar = playerScoreStr.c_str();  //use char const* as target type

    //pixels from text
    playerSurfaceText = TTF_RenderText_Solid(ourFont,pchar,textColor);

    //set up texture
    playerTexture = SDL_CreateTextureFromSurface(mRenderer,playerSurfaceText);

    SDL_FreeSurface(playerSurfaceText);

    SDL_RenderCopy(mRenderer,playerTexture,NULL,&playerScoreText);

    //destroy texture
    SDL_DestroyTexture(playerTexture);

    //close font
    TTF_CloseFont(ourFont);

    SDL_RenderPresent(mRenderer); //swap front and back buffers
}

void Game::ShutDown() {
    //destroy SDL_Window
    SDL_DestroyWindow(mWindow);

    //destroy SDL_Renderer
    SDL_DestroyRenderer(mRenderer);

    //close SDL
    SDL_Quit();
}

void Game::ProcessInput() {
    SDL_Event event;

    //go through all events and respond as desired/appropriate
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                mIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym ) {
                    case SDLK_SPACE:
                        if(!gameOver && !startGame && resetClicks < 1) {
                            resetClicks++;
                        }
                        else if(gameOver && resetClicks == 2) {
                            ResetGame();
                        }
                        break;
                }
                break;
        }
    }

    // Retrieve the state of all of the keys then scan each as desired
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_ESCAPE]){
        mIsRunning = false;
    }

    if(resetClicks < 1) { return; }

    //update snake movement
    if(state[SDL_SCANCODE_LEFT]) {
        mSnakeDir = Left;
        if(!startGame) { startGame = true; }
    } else if(state[SDL_SCANCODE_RIGHT]) {
        mSnakeDir = Right;
        if(!startGame) { startGame = true; }
    } else if(state[SDL_SCANCODE_UP]) {
        mSnakeDir = Up;
        if(!startGame) { startGame = true; }
    } else if(state[SDL_SCANCODE_DOWN]) {
        mSnakeDir = Down;
        if(!startGame) { startGame = true; }
    }
}

void Game::UpdateGame() {
    //update frame at fixed intervals (fixedDeltaTime)
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    //get deltaTime
    deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    mTicksCount = SDL_GetTicks();

    //clamp deltaTime
    if(deltaTime > .05f) {
        deltaTime = 0.05f;
    }

    if(gameOver) { return; }

    if(counter1 > 0.1f) {
        counter1 = 0.0f;
        if(mSnakeDir == Left) {
            mSnakePos.x -= 16;
        } else if(mSnakeDir == Right) {
            mSnakePos.x += 16;
        } else if(mSnakeDir == Up) {
            mSnakePos.y -= 16;
        } else if(mSnakeDir == Down) {
            mSnakePos.y += 16;
        }
    }

    if(snake.size() == 0) {
        for(int i = 0; i < 5; i++) {
            SnakeComponent snakeBody(mSnakePos + Vector2(i*16,0));
            snake.push_back(snakeBody);
        }
    }

    if(!startGame) { return; }

    if(counter1 > 0.0f) {
        counter1 += deltaTime;
    } else {
        //update position
        snakeHeadPos = mSnakePos;

        for(int i = snake.size() - 1; i > 0; i--) {
            snake[i].UpdatePosition(snake[i-1].GetPosition());
            if(i != 1) {
                if(snakeHeadPos.x == snake[i].GetPosition().x && snakeHeadPos.y == snake[i].GetPosition().y) {
                    //game over
                    gameOver = true;
                    resetClicks++;
                }
            }
        }

        snake[0].UpdatePosition(mSnakePos);
        if(snake[0].GetPosition().x == snake[1].GetPosition().x && snake[0].GetPosition().y == snake[1].GetPosition().y) {
            if(mSnakeDir == Left) {
                mSnakePos.x -= 16;
            } else if(mSnakeDir == Right) {
                mSnakePos.x += 16;
            } else if(mSnakeDir == Up) {
                mSnakePos.y -= 16;
            } else if(mSnakeDir == Down) {
                mSnakePos.y += 16;
            }
            snake[0].UpdatePosition(mSnakePos);
        }

        //update snake position at boundaries
        if(mSnakePos.x < 0) {
            mSnakePos.x = 1024;
        } else if(mSnakePos.x > 1024) {
            mSnakePos.x = 0;
        } else if(mSnakePos.y < 0) {
            mSnakePos.y = 768;
        } else if(mSnakePos.y > 768) {
            mSnakePos.y = 0;
        }

        snakeTailPos = snake[snake.size() - 1].GetPosition();

        counter1 += deltaTime;

        /* At every movement update, check if food location == snake head location.
        if so, add a segment and eliminate the food.*/
        Vector2 foodPosInt = Vector2(static_cast<int>(foodPosition.x),static_cast<int>(foodPosition.y));

        if(foodPosInt.x == snakeHeadPos.x && foodPosInt.y == snakeHeadPos.y) {
            //std::cout << "FOOD!" << std::endl;
            //find new position for food
            foodPosition.x = (rand() % 62 + 1) * 16;
            foodPosition.y = (rand() % 46 + 1) * 16;

            //increase score
            playerScore++;
             
            //add new segment to snake
            SnakeComponent snakeBody(snakeTailPos);
            snake.push_back(snakeBody);
        }
    }
}

void Game::GenerateOutput() {

    if(!gameOver && !startGame && resetClicks == 0) { //game not started yet 
        StartingText();
        return;
    } else if(gameOver) { //game played till end
        if(resetClicks == 2) {
            EndingText();
            return;
        }
    } 

    SDL_SetRenderDrawColor(mRenderer,0,0,0,0);

    SDL_RenderClear(mRenderer); //clear back buffer to current draw color

    SDL_SetRenderDrawColor(mRenderer,255,255,255,255);

    SDL_Rect topWall = {0,0,1024,thickness};
    SDL_Rect bottomWall = {0,752,1024,thickness};
    SDL_Rect leftWall = {0,0,thickness,768};
    SDL_Rect rightWall = {1008,0,thickness,768};

    SDL_RenderFillRect(mRenderer,&topWall);
    SDL_RenderFillRect(mRenderer,&rightWall);
    SDL_RenderFillRect(mRenderer,&leftWall);
    SDL_RenderFillRect(mRenderer,&bottomWall);

    //UPDATE POSITION OF THE SNAKE COMPONENTS!!!!!!!

    //draw snake
    for(int i = snake.size() - 1; i >= 0; i--) {
        snake[i].Draw(mRenderer);
    }

    //create food
    Food foodInstance(foodPosition);

    //draw food
    foodInstance.Draw(mRenderer);

    DisplayText();
} 
