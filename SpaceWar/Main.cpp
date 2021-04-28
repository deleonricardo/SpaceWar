#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include<iostream>
#include<string>
#include "enemy.h";
#include <vector>
#include "bullet.h"

using namespace std;

//boolean to maintain program loop
bool quit = false;

//deltaTime for frame rate
float deltaTime = 0.0;
int thisTime = 0;
int lastTime = 0;

//set player speed
float playerSpeed = 500.0f;

//movement up or down
float yDir;

//variables to control integer creep(rounding)
float pos_X, pos_Y;

//create rectangle for player pos
SDL_Rect playerPos;


//enemy list variable
vector<Enemy> enemyList;

//bullet list 
vector<Bullet> bulletList;

//declare sounds
Mix_Chunk* laser;
Mix_Chunk* explosion;
Mix_Music* bgm;


void CreateBullet()
{
	for (int i = 0; i < bulletList.size(); i++)
	{
		if (bulletList[i].active == false)
		{
			//play laser sound
			Mix_PlayChannel(-1, laser, 0);
			bulletList[i].active = true;
			bulletList[i].posRect.y = (pos_Y + (playerPos.w / 2));
			bulletList[i].posRect.y = (bulletList[i].posRect.y - (bulletList[i].posRect.w / 2));
			bulletList[i].posRect.x = playerPos.x;
			bulletList[i].pos_Y = pos_Y;
			bulletList[i].pos_X = playerPos.x;
			break;
		}
	}
}

//variables for score and lives
int score, oldScore, lives, oldLives;

//declare font
TTF_Font* font;

//font color
SDL_Color colorP1 = { 0, 255, 0, 255 };

//create surfaces
SDL_Surface* scoreSurface, * livesSurface;
//create textures
SDL_Texture* scoreTexture, * livesTexture;
SDL_Rect scorePos, livesPos;
string tempScore, tempLives;

void UpdateScore(SDL_Renderer* renderer)
{
	//creating the text
	tempScore = "player score: " + std::to_string(score);
	//create render text
	scoreSurface = TTF_RenderText_Solid(font, tempScore.c_str(), colorP1);
	//creating texture
	scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
	SDL_QueryTexture(scoreTexture, NULL, NULL, &scorePos.w, &scorePos.h);
	SDL_FreeSurface(scoreSurface);
	oldScore = score;
}

void UpdateLives(SDL_Renderer* renderer)
{
	//creating the text
	tempLives = "player lives: " + std::to_string(lives);
	//create renderer text
	livesSurface = TTF_RenderText_Solid(font, tempLives.c_str(), colorP1);
	//creating texture
	livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurface);
	SDL_QueryTexture(livesTexture, NULL, NULL, &livesPos.w, &livesPos.h);
	SDL_FreeSurface(livesSurface);
	oldLives = lives;
}

int main(int argc, char* argv[])
{
	//declare pointer
	SDL_Window* window;
	//create renderer variable
	SDL_Renderer* renderer = NULL;
	//Initialize SDL2
	SDL_Init(SDL_INIT_EVERYTHING);

	//create application window
	window = SDL_CreateWindow(
		"Space Wars",					//window title
		SDL_WINDOWPOS_UNDEFINED,		//initial x position
		SDL_WINDOWPOS_UNDEFINED,		//initial y position
		1024,							//width in pixels
		768,							//height in pixesl
		SDL_WINDOW_OPENGL				//flags, see below

	);

	//check window is created succesfully
	if (window == NULL)
	{
		//if window is not made
		printf("Window could not be created: %s\n", SDL_GetError());
		return 1;
	}

	//create renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//create a SDL surface
	SDL_Surface* surface = IMG_Load("./Assets/background.PNG");

	//create bkgd texture
	SDL_Texture* bkgd;

	//place surface ito the texture
	bkgd = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	//create rectangle for background
	SDL_Rect bkgdPos;
	//set bkgdPos;
	bkgdPos.x = 0;
	bkgdPos.y = 0;
	bkgdPos.w = 1024;
	bkgdPos.h = 768;
	//BACKGROUND CREATE END************
	//PLAYER CREATE CREATE********
	//create a SDL surface
	surface = IMG_Load("./Assets/player.PNG");

	//create bkgd texture for player
	SDL_Texture* player;

	//place surface in texture
	player = SDL_CreateTextureFromSurface(renderer, surface);

	//free surface
	SDL_FreeSurface(surface);

	//set bkgdPos x, y, width and height
	playerPos.x = 50;
	playerPos.y = 200;
	playerPos.w = 140;
	playerPos.h = 127;

	//PLAYER CREATE END***********

	//SDL event for input
	SDL_Event event;

	//create bullets and place in list
	for (int i = 0; i < 10; i++)
	{
		Bullet tempBullet(renderer, i + 5, i + 5);
		bulletList.push_back(tempBullet);
	}

	//clear enemy list
	enemyList.clear();

	//fill enemy list
	for (int i = 0; i < 8; i++)
	{
		Enemy tempEnemy(renderer);

		enemyList.push_back(tempEnemy);
	}

	//init audio playback
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	//load laser sound
	laser = Mix_LoadWAV("./Assets/laser.wav");
	//load explosion
	explosion = Mix_LoadWAV("./Assets/explode.wav");
	//play background music
	bgm = Mix_LoadMUS("./Assets/background.wav");
	//if music not playing, play and loop
	if (!Mix_PlayingMusic())
	{
		Mix_PlayMusic(bgm, -1);
	}

	//init player score, lives and font
	oldScore = 0;
	score = 0;
	oldLives = 0;
	lives = 5;

	TTF_Init();
	font = TTF_OpenFont("./Assets/Spac3.ttf", 20);

	//set up rectangles
	scorePos.x = 700;
	scorePos.y = 20;
	livesPos.x = 700;
	livesPos.y = 40;

	UpdateScore(renderer);
	UpdateLives(renderer);

	//set up enum for game states
	enum GameState { GAME, WIN, LOSE };

	//varaiable to track what game state
	GameState gameState = GAME;

	//bool to move to each games state
	bool game, win, lose;

	//game program loop
	while (!quit)
	{


		switch (gameState)
		{
		case GAME:
			game = true;
			std::cout << "The Game State is Game" << endl;
			std::cout << endl;

			//CREATE BACKGROUND
			//create a SDL surface
			surface = IMG_Load("./Assets/background.PNG");

			////place surface into the texture
			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			////free surface
			SDL_FreeSurface(surface);
			//	END CREATE BACKGROUND

			//clear enemy list
			enemyList.clear();

			//fill enemy list
			for (int i = 0; i < 8; i++)
			{
				Enemy tempEnemy(renderer);

				enemyList.push_back(tempEnemy);
			}
			//reset player score and lives
			score = 0;
			lives = 5;

			while (game)
			{
				//create deltaTime
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;

				//check for input
				if (SDL_PollEvent(&event))
				{
					//closes window by windows X button
					if (event.type == SDL_QUIT)
					{
						quit = true;
						game = false;
						break;
					}
					switch (event.type)
						//look for keypress
					{
					case SDL_KEYUP:
						//check the space bar
						switch (event.key.keysym.sym)
						{
						case SDLK_SPACE:
							CreateBullet();
							break;
						default:
							break;
						}

					}
				}

				//player movement
							//get the keyboard state
				const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

				//check which arrows are pressed
				if (currentKeyStates[SDL_SCANCODE_UP])
				{
					yDir = -1;
				}
				else if (currentKeyStates[SDL_SCANCODE_DOWN])
				{
					yDir = 1;
				}
				else
				{
					yDir = 0;
				}

				//UPDATE***********************

				//new position to move player - using direction and deltaTime
				pos_Y += (playerSpeed * yDir) * deltaTime;

				//move player
				playerPos.y = (int)(pos_Y + 0.5f);

				//keep player on screen
				//move down
				if (playerPos.y < 0)
				{
					playerPos.y = 0;
					pos_Y = 0;
				}

				//keep player on screen
				//move up
				if (playerPos.y > 768 - playerPos.h)
				{
					playerPos.y = 768 - playerPos.h;
					pos_Y = 768 - playerPos.h;
				}

				//update bullets
				for (int i = 0; i < bulletList.size(); i++)
				{
					if (bulletList[i].active == true)
					{
						bulletList[i].Update(deltaTime);
					}
				}

				//check collision of bullets with enemies
				//for loop scrolls though all the player's bullets
				for (int i = 0; i < bulletList.size(); i++)
				{
					//check to see if bullet is active
					if (bulletList[i].active == true)
					{
						//check all enemies against active bullet
						for (int j = 0; j < enemyList.size(); j++)
						{
							//see if there is a collision between this bullet and this enemy using SDL
							if (SDL_HasIntersection(&bulletList[i].posRect, &enemyList[j].posRect))
							{
								//play explosion sound
								Mix_PlayChannel(-1, explosion, 0);

								//reset the enemy 
								enemyList[j].Reset();

								//reset the bullet
								bulletList[i].Reset();

								//give player points
								score += 10;

								////check to see if the player wins
								if (score >= 500)
								{
									game = false;
									gameState = WIN;
								}
							}
						}
					}
				}

				//update enemy list
				for (int i = 0; i < 8; i++)
				{
					enemyList[i].Update(deltaTime);
				}

				//check for collision with the player and all enemies
							//check all enemies against active bullet
				for (int i = 0; i < enemyList.size(); i++)
				{
					//see if there is a collision between this bullet and this enemy using SDL
					if (SDL_HasIntersection(&playerPos, &enemyList[i].posRect))
					{
						//play explosion sound
						Mix_PlayChannel(-1, explosion, 0);

						//reset the enemy
						enemyList[i].Reset();

						//take player lives
						lives -= 1;

						////check to see if the player loses
						if (lives <= 0)
						{
							game = false;
							gameState = LOSE;
						}


					}
				}

				//update score and lives on score text
				if (score != oldScore)
				{
					UpdateScore(renderer);
				}

				if (lives != oldLives)
				{
					UpdateLives(renderer);
				}

				//UPDATE END******************


				//DRAW***************
				//clear the old buffer
				SDL_RenderClear(renderer);

				//draw to bkgd
				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);


				//draw bullets
				for (int i = 0; i < bulletList.size(); i++)
				{
					if (bulletList[i].active == true)
					{
						bulletList[i].Draw(renderer);
					}
				}

				//Draw Player
				SDL_RenderCopy(renderer, player, NULL, &playerPos);

				//draw enemy list
				for (int i = 0; i < 8; i++)
				{
					enemyList[i].Draw(renderer);
				}

				//draw score texture
				SDL_RenderCopy(renderer, scoreTexture, NULL, &scorePos);

				//draw lives texture
				SDL_RenderCopy(renderer, livesTexture, NULL, &livesPos);


				//draw new info to screen
				SDL_RenderPresent(renderer);
				//DRAW END**********
			}//end game screen
		break;

		case WIN:
			win = true;

			std::cout << "The Game State is WIN" << endl;
			std::cout << "Press the R Key to Play Again" << endl;
			std::cout << "Press the Q Key to Quit Game" << endl;
			std::cout << endl;

			//load win background
			//create a SDL surface
			surface = IMG_Load("./Assets/win.png");

			//place surface into texture
			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			//free the surface
			SDL_FreeSurface(surface);

			//background image -- CREATE END

			while (win)
			{
				//create deltaTime
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;

				//check for input
				if (SDL_PollEvent(&event))
				{
					//close window by the window X button
					if (event.type == SDL_QUIT)
					{
						quit = true;
						win = false;
						break;
					}

					switch (event.type)
					{
						// Look for a keypress 
					case SDL_KEYUP:
						// Check the SDLKey values 
						switch (event.key.keysym.sym)
						{
						
						case SDLK_r:
							win = false;
							gameState = GAME;
							break;
						case SDLK_q:
							quit = true;
							win = false;
							break;
						default:
							break;
						}

					}
				}

				//DRAW *****************************
				//clear the old buffer
				SDL_RenderClear(renderer);

				//prepare to draw bkgd
				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

				//draw new info to the screen
				SDL_RenderPresent(renderer);

				//END DRAW *************************


			}//ends win screen
			break;

		case LOSE:

			lose = true;

			std::cout << "The Game State is LOSE" << endl;
			std::cout << "Press the R Key to Play Again" << endl;
			std::cout << "Press the M Key to go to the Main Menu" << endl;
			std::cout << "Press the Q Key to Quit Game" << endl;
			std::cout << endl;

			//load win background
			//create a SDL surface
			surface = IMG_Load("./Assets/lose.png");

			//place surface into the texture
			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			//free the surface
			SDL_FreeSurface(surface);

			//background image -- CREATE END

			while (lose)
			{
				//create deltaTime
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;

				//check for input
				if (SDL_PollEvent(&event))
				{
					//close window by the window's X button
					if (event.type == SDL_QUIT)
					{
						quit = true;
						lose = false;
						break;
					}

					switch (event.type)
					{
						/* Look for a keypress */
					case SDL_KEYUP:
						/* Check the SDLKey values and move change the coords */
						switch (event.key.keysym.sym)
						{
						case SDLK_r:
							lose = false;
							gameState = GAME;
							break;
						case SDLK_q:
							quit = true;
							lose = false;
							break;
						default:
							break;
						}

					}
				}

				//DRAW *****************************

				//Draw section
				//clear the old buffer
				SDL_RenderClear(renderer);

				//prepare to draw bkgd1
				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

				//draw new info to the screen
				SDL_RenderPresent(renderer);

				//END DRAW *************************


				break;
			}

	}//end gamestate switch



}//end while loop


//close and destroy the window
SDL_DestroyWindow(window);

//clean up
SDL_Quit();
return 0;
}