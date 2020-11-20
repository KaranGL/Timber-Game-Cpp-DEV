/*
Author Name - Karan Sharma
Date Created - 31/10/2020
Game - Timber
*/

#include <cstdio>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//Make code easier to type with 'using namespace'
using namespace sf;

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];
//Where is the player/branch? Left or Right
enum class side { LEFT, RIGHT, NONE };
side branchPosition[NUM_BRANCHES];

void updateBranches(int seed) {
	//move all the branches down one place
	for (int i = NUM_BRANCHES - 1; i > 0; i--) {
		branchPosition[i] = branchPosition[i - 1];
	}

	//create a new branch at index 0
	//LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPosition[0] = side::LEFT;
		break;
	case 1:
		branchPosition[0] = side::RIGHT;
		break;
	default:
		branchPosition[0] = side::NONE;
		break;
	}
}

int main() {
	int score = 0;

	//create a video mode object
	VideoMode vm(1920, 1080);

	//Create and open a window for the game
	RenderWindow window(vm, "Timber Game!!!", Style::Fullscreen);

	/*
==============================================================================================
									Graphics of the game
==============================================================================================
*/
//Create a texture to hold a graphic on the GPU
	Texture backtexture;									//
	//Load a graphic into the texture						//Graphics section for the
	backtexture.loadFromFile("graphics/gameback1.png");		//background.
	backtexture.setSmooth(true);							//
	//Create a sprite										
	Sprite backsprite;
	//Attach the texture to the sprite
	backsprite.setTexture(backtexture);
	//set the sprite background to cover the screen
	backsprite.setPosition(0, 0);

	//Graphics section for the tree
	Texture tree;
	tree.loadFromFile("graphics/tree.png");
	tree.setSmooth(true);
	Sprite treesprite;
	treesprite.setTexture(tree);
	treesprite.setPosition(810, 0);

	//Graphics section for bee
	Texture bee;
	bee.loadFromFile("graphics/bee.png");
	bee.setSmooth(true);
	Sprite beesprite;
	beesprite.setTexture(bee);
	beesprite.setPosition(0, 800);
	//bee movement
	bool beeActive = false;
	//bee speed
	float beeSpeed = 0.0f;

	//Graphics section for clouds
	Texture cloudsTexture;
	cloudsTexture.loadFromFile("graphics/cloud.png");
	cloudsTexture.setSmooth(true);

	// Make the clouds with arrays
	const int NUM_CLOUDS = 6;
	Sprite clouds[NUM_CLOUDS];
	int cloudSpeeds[NUM_CLOUDS];
	bool cloudsActive[NUM_CLOUDS];

	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		clouds[i].setTexture(cloudsTexture);
		clouds[i].setPosition(-300, i * 150);
		cloudsActive[i] = false;
		cloudSpeeds[i] = 0;

	}

	//control time
	Clock clock;

	//Time bar
	RectangleShape timeBar, timeBack;
	float timeBarStart = 500;
	float timeBarHeight = 80;

	timeBar.setSize(Vector2f(timeBarStart, timeBarHeight));
	timeBack.setSize(Vector2f(timeBarStart, timeBarHeight));
	
	timeBar.setFillColor(Color::Green);
	timeBack.setFillColor(sf::Color(0, 0, 0, 150));

	timeBar.setPosition((1920 / 2) - timeBarStart / 2, 980);
	timeBack.setPosition((1920 / 2) - timeBarStart / 2, 980);

	Time totalTime;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStart / timeRemaining;

	//Track whether the game is running
	bool paused = true;

	//choose a font
	Font font;

	if (!font.loadFromFile("fonts/orange.ttf")) {
		printf("Font file not loaded!");
	}

	//Draw text
	Text messageText;
	Text scoreText;
	Text fps;
	Text timeText;

	//set the font to message
	messageText.setFont(font);
	scoreText.setFont(font);
	fps.setFont(font);
	timeText.setFont(font);

	//Assign the actual message
	messageText.setString("Press enter to start!!!");
	scoreText.setString("Score = 0");
	fps.setString("FPS = 0");
	timeText.setString("Time Remaining");

	//text size
	messageText.setCharacterSize(80);
	scoreText.setCharacterSize(70);
	fps.setCharacterSize(70);
	timeText.setCharacterSize(50);
	//Choose a color
	messageText.setFillColor(Color::Green);
	scoreText.setFillColor(Color::Black);
	fps.setFillColor(Color::Black);
	timeText.setFillColor(Color::Black);

	//Position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);
	fps.setPosition(1550, 20);
	timeText.setPosition(800, 920);

	//Prepare 6 branches
	Texture treeBranch;
	treeBranch.loadFromFile("graphics/branch.png");
	treeBranch.setSmooth(true);
	//Set texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(treeBranch);
		branches[i].setPosition(-2000, -2000);
		//Set the sprite's origin to dead center 
		//We can then spin it round without changing its position
		branches[i].setOrigin(220, 20);
	}

	//Prepare the player
	Texture player;
	player.loadFromFile("graphics/player.png");
	player.setSmooth(true);
	Sprite playerSprite;
	playerSprite.setTexture(player);
	playerSprite.setPosition(580, 720);

	//Player starts on the left
	side playerSide = side::LEFT;

	//Prepare the gravestone
	Texture dead;
	dead.loadFromFile("graphics/rip.png");
	dead.setSmooth(true);
	Sprite deadSprite;
	deadSprite.setTexture(dead);
	deadSprite.setPosition(600, 860);

	//Prepare the  axe
	Texture axe;
	axe.loadFromFile("graphics/axe.png");
	axe.setSmooth(true);
	Sprite axeSprite;
	axeSprite.setTexture(axe);
	axeSprite.setPosition(700, 830);

	//align the axe with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	//Flying action of the log
	Texture log;
	log.loadFromFile("graphics/log.png");
	log.setSmooth(true);
	Sprite logSprite;
	logSprite.setTexture(log);
	logSprite.setPosition(810, 720);

	//Other log variables
	bool logActive = false;
	float Log_x_Speed = 1000;
	float Log_y_Speed = -1500;

	//Control the player input
	bool acceptInput = false;

	//CHOP sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	//DEATH sound
	SoundBuffer deadBuffer;
	deadBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deadBuffer);

	//Out of time sound
	SoundBuffer outBuffer;
	outBuffer.loadFromFile("sound/out_of_time.wav");
	Sound noTime;
	noTime.setBuffer(outBuffer);

	// control the drawing of the score
	int lastDrawn = 0;

	//====================================================================================================================
	//Game loop
	//====================================================================================================================
	while (window.isOpen()) {

		/*
		===============================================
		HANDLE THE INPUT
		===============================================
		*/

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				//Listen for key presses again
				acceptInput = true;

				//hide the axe
				axeSprite.setPosition(2000, axeSprite.getPosition().y);
			}
		}

		//checking if escape key is pressed to quit
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		//Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;
			timeBar.setFillColor(Color::Green);

			//Reset the time bar and the score
			score = 0;
			timeRemaining = 6;

			//Make all the branches disappear
			for (int i = 1; i < NUM_BRANCHES; i++) {
				branchPosition[i] = side::NONE;
			}
			
			//make sure the gravestone is hidden
			deadSprite.setPosition(675, 2000);

			//move the player into position
			playerSprite.setPosition(580, 720);
			acceptInput = true;
		}

		//Wrap the player controls to make sure we are accepting input
		if (acceptInput) {
			//handle right key input
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				//Make sure the player is on the right side
				playerSide = side::RIGHT;
				score++;
				
				//Add to the amount of time remaining
				timeRemaining += (2 / score) + 0.15;

				if (timeRemaining > 6) {
					timeRemaining = 6;
				}

				axeSprite.setPosition(AXE_POSITION_RIGHT, axeSprite.getPosition().y);

				playerSprite.setPosition(1200, 720);

				//update the branches
				updateBranches(score);

				//set the log flying to the left
				logSprite.setPosition(810, 720);
				Log_x_Speed = -5000;
				logActive = true;

				acceptInput = false;

				//Play the chop sound
				chop.play();
			}
			//handle the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				//Make sure the player is on the left
				playerSide = side::LEFT;
				score++;

				//Add to the amount of time remaining
				timeRemaining += (2 / score) + 0.15;

				if (timeRemaining > 6) {
					timeRemaining = 6;
				}

				axeSprite.setPosition(AXE_POSITION_LEFT, axeSprite.getPosition().y);

				playerSprite.setPosition(580, 720);

				//update the branches
				updateBranches(score);

				//set the log flying to the left
				logSprite.setPosition(810, 720);
				Log_x_Speed = 5000;
				logActive = true;

				acceptInput = false;
				
				//play the chop sound
				chop.play();
			}
		}

		/*
		=================================================
		UPDATE THE SCENE
		=================================================
		*/

		if (!paused) {
			//Measure time
			Time t = clock.restart();
			//Subtract from the amount of time remaining
			timeRemaining -= t.asSeconds();
			//size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

			if (timeRemaining > 0.0f && timeRemaining < 2.0f) {
				timeBar.setFillColor(Color::Red);
			}

			if (timeRemaining <= 0.0f) {
				//pause the game
				paused = true;

				//change the message shown to the player
				messageText.setString("LOL!!! You are out of time!!!\n $ Press Enter to play again $");

				//Reposition the text based its new size
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				
				//Play the out of time sound
				noTime.play();
			}

			/*
			+++++++++++++++++++++++++++++++++++++++++++++++++
			SETTING UP THE BEE
			+++++++++++++++++++++++++++++++++++++++++++++++++
			*/
			//Setup the bee
			if (!beeActive) {
				//Bee's speed
				srand((int)time(0) * 10);
				beeSpeed = (rand() % 200) + 200;

				//Bee's height
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				beesprite.setPosition(2000, height);
				beeActive = true;
			}
			else { //when the bee is active
				beesprite.setPosition(
					beesprite.getPosition().x - (beeSpeed * t.asSeconds()), beesprite.getPosition().y
				);

				//Has the bee reached the right hand edge of the screen?
				if (beesprite.getPosition().x < -100) {
					//Set it up ready to be a whole new cloud next frame
					beeActive = false;
				}

			}

			/*
			+++++++++++++++++++++++++++++++++++++++++++++++++
			SETTING UP THE clouds
			+++++++++++++++++++++++++++++++++++++++++++++++++
			*/
			// Manage the clouds with arrays
			for (int i = 0; i < NUM_CLOUDS; i++)
			{
				if (!cloudsActive[i])
				{
					// How fast is the cloud
					srand((int)time(0) * i);
					cloudSpeeds[i] = (rand() % 200);

					// How high is the cloud
					srand((int)time(0) * i);
					float height = (rand() % 150);
					clouds[i].setPosition(-200, height);
					cloudsActive[i] = true;

				}
				else
				{
					// Set the new position
					clouds[i].setPosition(
						clouds[i].getPosition().x +
						(cloudSpeeds[i] * t.asSeconds()),
						clouds[i].getPosition().y);

					// Has the cloud reached the right hand edge of the screen?
					if (clouds[i].getPosition().x > 1920)
					{
						// Set it up ready to be a whole new cloud next frame
						cloudsActive[i] = false;
					}

				}

			}

			// Update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			if (score > 100) {
				timeRemaining -= t.asSeconds() * 2;
			}
			
			// Draw the frame rate once every 800 frames
			lastDrawn++;
			if (lastDrawn == 800) {
				// Draw the fps
				std::stringstream ss2;
				ss2 << "FPS = " << 1 / t.asSeconds();
				fps.setString(ss2.str());
				lastDrawn = 0;
			}

			//update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPosition[i] == side::LEFT) {
					//Move the sprite to the left side
					branches[i].setPosition(610, height);
					//Flip the sprite by 180 degrees
					branches[i].setRotation(180);
				}
				else if (branchPosition[i] == side::RIGHT) {
					//Move the sprite to the right side
					branches[i].setPosition(1330, height);
					//rotate the sprite to normal
					branches[i].setRotation(0);
				}
				else {
					//Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			//Flying log
			if (logActive) {
				logSprite.setPosition(logSprite.getPosition().x + (Log_x_Speed * t.asSeconds()), logSprite.getPosition().y +
					(Log_y_Speed * t.asSeconds()));

				//has the log reached the right hand edge?
				if (logSprite.getPosition().x < -100 || logSprite.getPosition().x > 2000) {
					//set it up ready to be a whole new log next frame
					logActive = false;
					logSprite.setPosition(810, 720);
				}
			}

			//Has the player been squished by a branch?
			if (branchPosition[5] == playerSide) {
				//death
				paused = true;
				acceptInput = false;

				//Draw the gravestone
				deadSprite.setPosition(525, 760);

				//Hide the player
				playerSprite.setPosition(2000, 600);

				//Create a text on the screen
				messageText.setString("Oh! Oh! You got squished!!!\n $ Press Enter to play again $");

				//Center it on the screen
				FloatRect text = messageText.getLocalBounds();
				messageText.setOrigin(text.left + text.width / 2.0f, text.top + text.height / 2.0f);

				messageText.setPosition(1920/2.0f, 1080/2.0f);
				death.play();
			}
		}//end of if(!paused)

		/*
		=================================================
		DRAW THE SCENE
		=================================================
		*/

		//Clear everything from the last frame
		window.clear();

		//draw the game scene here
		window.draw(backsprite);

		//Draw the clouds
		for (int i = 0; i < NUM_CLOUDS; i++)
		{
			window.draw(clouds[i]);
		}

		//Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		//Draw the tree
		window.draw(treesprite);


		//Draw the bee
		window.draw(beesprite);

		//Draw text
		window.draw(scoreText);

		//time remaining text
		window.draw(timeText);

		//Draw fps text
		window.draw(fps);

		//Draw timebar
		window.draw(timeBar);
		//Time bar background
		window.draw(timeBack);

		//Draw the player
		window.draw(playerSprite);

		//Draw the axe
		window.draw(axeSprite);

		//Draw the flying log
		window.draw(logSprite);

		//Draw the gravestone
		window.draw(deadSprite);

		if (paused) {
			window.draw(messageText);
		}

		//display all the stuff that was drawn earlier
		window.display();
	}
	return 0;
}