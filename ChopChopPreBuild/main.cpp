// Include important C++ libraries here
#include <SFML/Graphics.hpp>
#include <sstream>
#include<SFML/Audio.hpp>

// Make code easier to type with "using namespace"
using namespace sf;

//Function declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 5;
Sprite branches[NUM_BRANCHES];

// Where is the player/branch?
// Left or Right
enum class side {
    LEFT, RIGHT, NONE
};
side branchPositions[NUM_BRANCHES];

int main() {

    // Create a video mode object
    VideoMode vm(1366, 768);

    // Create and open a window for the game RenderWindow
    RenderWindow window(vm, "Chop Chop!!!", Style::Titlebar);

    //Create texture to hold graphic on the GPU
    Texture textureBackground;

    //Load graphic into the texture
    textureBackground.loadFromFile("graphics/background_small.png");

    //create a sprite
    Sprite spriteBackground;

    //Attach texture to the sprite
    spriteBackground.setTexture(textureBackground);

    //set the spriteBackground to cover the screen
    spriteBackground.setPosition(0, 0);

    //Make a tree sprite
    Texture textureTree;
    textureTree.loadFromFile("graphics/tree_small.png");
    Sprite spriteTree;
    spriteTree.setTexture(textureTree);
    spriteTree.setPosition(583, 0); // 810, 0

    // Prepare the bee
    Texture textureBee;
    textureBee.loadFromFile("graphics/bee_small.png");
    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(0, 550); //original (0, 800)

    //Is the Bee currently moving?
    bool beeActive = false;

    //How fast can the Bee fly?
    float beeSpeed = 0.0f;

    //Make 3 clouds sprites from 1 texture
    Texture textureCloud;

    //Load 1 new texture
    textureCloud.loadFromFile("graphics/cloud_small.png");

    //3 new sprites with the same texture
    Sprite spriteCloud1;
    Sprite spriteCloud2;
    Sprite spriteCloud3;
    spriteCloud1.setTexture(textureCloud);
    spriteCloud2.setTexture(textureCloud);
    spriteCloud3.setTexture(textureCloud);

    //positioning the clouds
    spriteCloud1.setPosition(0, 0);
    spriteCloud2.setPosition(0, 125); // 0, 250
    spriteCloud3.setPosition(0, 250); // 0, 500

    //Are clouds currently on screen?
    bool cloud1Active = false;
    bool cloud2Active = false;
    bool cloud3Active = false;

    //How fast is each cloud?
    float cloud1Speed = 0.0f;
    float cloud2Speed = 0.0f;
    float cloud3Speed = 0.0f;

    //Variables to control time itself
    Clock clock;

    //TIME BAR
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 60; // 80
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition((1366 / 2) - timeBarStartWidth / 2, 644); //1920,980

    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    //Track whether the game is running
    bool paused = true;

    //Draw some text
    int score = 0;

    Text messageText;
    Text scoreText;

    //We need to choose the font
    Font fontMessage, fontScore;
    fontMessage.loadFromFile("fonts/8bitlimr.ttf");
    fontScore.loadFromFile("fonts/karma suture.ttf");

    //Set the font to our message
    messageText.setFont(fontMessage);;
    scoreText.setFont(fontScore);

    //Assign the actual message
    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");

    //Make it big
    messageText.setCharacterSize(60);
    scoreText.setCharacterSize(65);

    //Choose a color
    messageText.setColor(Color::White);
    scoreText.setColor(Color::White);

    //Position the text
    FloatRect textRect = messageText.getLocalBounds();

    messageText.setOrigin(textRect.left +
        textRect.width / 2.0f,
        textRect.top +
        textRect.height / 2.0f);

    messageText.setPosition(1366 / 2.0f, 768 / 2.0f);

    scoreText.setPosition(20, 20);

    // Prepare 6 branches
    Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch_small.png");

    // Set the texture for each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++) {
        branches[i].setTexture(textureBranch);
        branches[i].setPosition(-2000, -2000);
        // Set the sprite's origin to dead center
        // We can then spin it round without changing its position
        branches[i].setOrigin(168, 29);
    }

    //PREPARE THE PLAYER
    Texture texturePlayer;
    texturePlayer.loadFromFile("graphics/player_small.png");
    Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);
    spritePlayer.setPosition(455, 500); // 580, 720

    //--THE PLAYER STARTS ON LEFT SIDE--
    side playerSide = side::LEFT;

    //PREPARE THE GRAVESTONE
    Texture textureRIP;
    textureRIP.loadFromFile("graphics/RIP.png");
    Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(400, 1504); // 600,860

    //PREPARE THE AXE
    Texture textureAxe;
    textureAxe.loadFromFile("graphics/axe_small.png");
    Sprite spriteAxe;
    spriteAxe.setTexture(textureAxe);
    spriteAxe.setPosition(515, 540); //700,830

    //--LINE THE AXE UP WITH THE TREE
    const float AXE_POSITION_LEFT = 515; // 700
    const float AXE_POSITION_RIGHT = 755; // 1075

    //PREPARE FLYING LOG
    Texture textureLog;
    textureLog.loadFromFile("graphics/log_small.png");
    Sprite spriteLog;
    spriteLog.setTexture(textureLog);
    spriteLog.setPosition(583, 540); // 810, 720

    //--SOME OTHER USEFUL LOG RELATED VARIABLES--
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    //Control the player input
    bool acceptInput = false;

    // Prepare the sound
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("sound/chop.wav");
    Sound chop;
    chop.setBuffer(chopBuffer);

    //Squashed sound
    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("sound/squash.wav");
    Sound death;
    death.setBuffer(deathBuffer);

    // Out of time
    SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("sound/out_of_time.wav");
    Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);

    while (window.isOpen()) {

        /*
        ****************************************
        Handle the players input
        ****************************************
        */

        Event event;

        while (window.pollEvent(event)) {

            if (event.type == Event::KeyReleased && !paused) {

                //Listen for key pressing again
                acceptInput = true;

                //hide the axe
                spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
            }
        }

        // To Exit game
        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        //To start game
        if (Keyboard::isKeyPressed(Keyboard::Return)) {
            paused = false;

            //Reset the time and score
            score = 0;
            timeRemaining = 6;
            spriteAxe.setPosition(515, 540);

            //Make all the branches disappear
            for (int i = 1; i < NUM_BRANCHES; i++) {

                branchPositions[i] = side::NONE;
            }

            //Make sure the graveStone in hidden
            spriteRIP.setPosition(675, 2000);

            //Move the player into position
            spritePlayer.setPosition(455, 500); // 580, 720

            acceptInput = true;

        }
            // Wrap the player controls to
            // Make sure we are accepting input
            if (acceptInput) {

                // More code here next...
                //FIRST HANDLE PRESSING THE RIGHT CURSOR KEY
                if (Keyboard::isKeyPressed(Keyboard::Right)) {

                    //Make sure the player is on the right side
                    playerSide = side::RIGHT;
                    score++;

                    // Add to the amount of time remaining
                    timeRemaining += (2 / score) + .15;

                    spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);

                    spritePlayer.setPosition(833, 500);

                    //Update the Branches
                    updateBranches(score);

                    //Set the log flying to the left
                    spriteLog.setPosition(583, 540);
                    logSpeedX = -5000;
                    logActive = true;

                    acceptInput = false;

                    //Play a sound
                    chop.play();
                }

                //HANDLE THE LEFT CURSOR KEY

                if (Keyboard::isKeyPressed(Keyboard::Left)) {
                    //Make sure the player is on left side
                    playerSide = side::LEFT;
                    score++;

                    //Add to the amount of remaining time
                    timeRemaining += (2 / score) + .15;

                    spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);

                    spritePlayer.setPosition(455, 500); // 580, 720

                    //Update the branches
                    updateBranches(score);

                    //set log flying
                    spriteLog.setPosition(583, 540);
                    logSpeedX = 5000;
                    logActive = true;

                    acceptInput = false;

                    //Play a sound
                    chop.play();
                }
            }

        /*
        ****************************************
        Update the scene
        ****************************************
        */

        if (!paused) {

            //MEASURE TIME
            Time dt = clock.restart(); //it returns the amount of time that has elapsed since the last time we restarted the clock. (dt = delta time)

            //SUBTRACT FROM THE AMOUNT OF TIME REMAINING
            timeRemaining -= dt.asSeconds();
            //SIZE UP THE TIME BAR
            timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

            if (timeRemaining <= 0.0f) {
                //Pause the game
                paused = true;

                //Change the message shown to the player
                messageText.setString("Out of Time!!");

                //Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();

                messageText.setOrigin(textRect.left +
                    textRect.width / 2.0f,
                    textRect.top +
                    textRect.height / 2.0f);

                messageText.setPosition(1366 / 2.0f, 768 / 2.0f);

                // Play the out of time sound
                outOfTime.play();

            }

            //SETUP THE BEE
            if (!beeActive) {
                //How fast is the bee
                srand((int) time(0));
                beeSpeed = (rand() % 200) + 200;

                //How high is the bee
                srand((int) time(0) * 10); // srand((int)time(0) * 10);
                float height = (rand() % 150) + 400; // original 500 , 500
                spriteBee.setPosition(1400, height);
                beeActive = true;
            } else { //Move the bee
                spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y);

                //Has the bee reached the right hand side of the screen?
                if (spriteBee.getPosition().x < -100) {
                    //Set it up to be a whole new cloud next frame
                    beeActive = false;
                }
            }

            //Manage clouds
            //CLOUD 1
            if (!cloud1Active) {
                //How fast is the cloud
                srand((int) time(0) * 10);
                cloud1Speed = (rand() % 200);

                //How high is the cloud
                srand((int) time(0) * 10);
                float height = (rand() % 120);
                spriteCloud1.setPosition(-200, height);
                cloud1Active = true;
            } else {
                spriteCloud1.setPosition(spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()), spriteCloud1.getPosition().y);

                //Has the cloud reached the right hand side of the screen?
                if (spriteCloud1.getPosition().x > 1366) {
                    // Set i t ready to be a whole new cloud next frame
                    cloud1Active = false;
                }
            }

            //CLOUD 2
            if (!cloud2Active) {
                //How fast is the cloud?
                srand((int) time(0) * 20);
                cloud2Speed = rand() % 200;

                //How high is the cloud
                srand((int) time(0) * 20);
                float height = (rand() % 240) - 120; // original 300, 150
                spriteCloud2.setPosition(-200, height);
                cloud2Active = true;
            } else {
                spriteCloud2.setPosition(spriteCloud2.getPosition().x - (cloud2Speed * dt.asSeconds()), spriteCloud2.getPosition().y);

                // Has the cloud reached the right hand edge of the screen?
                if (spriteCloud2.getPosition().x > 1366) {
                    // Set it up ready to be a whole new cloud next frame
                    cloud2Active = false;
                }

            }
            //CLOUD 3
            if (!cloud3Active) {
                //How fast is cloud
                srand((int) time(0) * 30);
                cloud3Speed = (rand() % 200);

                //How high is the cloud
                srand((int) time(0) * 30);
                float height = (rand() % 360) - 120;
                spriteCloud3.setPosition(-200, height);
                cloud3Active = true;
            } else {
                spriteCloud3.setPosition(spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()), spriteCloud3.getPosition().y);

                // Has the cloud reached the right hand edge of the screen?
                if (spriteCloud3.getPosition().x > 1366) {
                    // Set it up ready to be a whole new cloud next frame
                    cloud3Active = false;
                }
            }

            // Update the score text
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());

        // UPDATE THE BRANCH SPRITES
        for (int i = 0; i < NUM_BRANCHES; i++) {

            float height = i * 150;
            if (branchPositions[i] == side::LEFT) {

                // Move the sprite to the left side
                branches[i].setPosition(415, height); //610

                // Flip the sprite round the other way
                branches[i].setRotation(180);
            } else if (branchPositions[i] == side::RIGHT) {

                // Move the sprite to the right side
                branches[i].setPosition(951, height); //1330

                // Set the sprite rotation to normal
                branches[i].setRotation(0);
            } else {

                // Hide the branch
                branches[i].setPosition(3000, height);
            }
        }

        // Handle a flying log
        if (logActive) {

            spriteLog.setPosition(
            spriteLog.getPosition().x +
            (logSpeedX * dt.asSeconds()),
            spriteLog.getPosition().y +
            (logSpeedY * dt.asSeconds()));

            // Has the log reached the right hand edge?
            if (spriteLog.getPosition().x < -100 ||
            spriteLog.getPosition().x > 2000) {

                // Set it up ready to be a whole new log next frame
                logActive = false;
                spriteLog.setPosition(583, 540);
        }
     }

     //HAS THE PLAYER BEEN SQUASHED BY A BRANCH?
     if(branchPositions[4] == playerSide) {
        //DEATH
        paused = true;
        acceptInput = false;

        //Draw the graveStone
        if(playerSide == side::LEFT) {
            spriteRIP.setPosition(430, 454);
        } else {
            spriteRIP.setPosition(800, 454);
        }
        spriteAxe.setPosition(400, 1500);
        //Change the text of the message
        messageText.setString("SQUASHED!!");

        //Center it to the screen
        FloatRect textRect = messageText.getLocalBounds();

        messageText.setOrigin(textRect.left +
                              textRect.width / 2.0f,
                              textRect.top +
                              textRect.height / 2.0);

        messageText.setPosition(1366 / 2.0f, 768 / 2.0f);

        // Play the death sound
        death.play();
     }

    }
        /*
        ****************************************
        Draw the scene
        ****************************************
        */
        // Clear everything from the last frame
        window.clear();

        // Draw our game scene here
        window.draw(spriteBackground);

        // Draw the clouds
        window.draw(spriteCloud1);
        window.draw(spriteCloud2);
        window.draw(spriteCloud3);

        for (int i = 0; i < NUM_BRANCHES; i++) {

            window.draw(branches[i]);
        }

        // Draw the tree
        window.draw(spriteTree);

        // Draw the player
        window.draw(spritePlayer);

        // Draw the Axe
        window.draw(spriteAxe);

        // Draw the flying log
        window.draw(spriteLog);

        // Draw the gravestone
        window.draw(spriteRIP);

        // Draw the insect
        window.draw(spriteBee);

        //Draw the Score
        window.draw(scoreText);

        //Draw the timber
        window.draw(timeBar);

        if (paused) {

            // Draw our message
            window.draw(messageText);
        }
        // Show everything we just drew
        window.display();
    }

    return 0;
}

// FUNCTION DEFINATION
void updateBranches(int seed) {

    //Move all the branches down 1 place
    for (int j = NUM_BRANCHES - 1; j > 0; j--) {
        branchPositions[j] = branchPositions[j - 1];
    }

    //Spawn a new branch at position 0
    //LEFT, RIGHT or NONE
    srand((int) time(0) + seed); // used seed so that ran. no. always different.
    int r = rand() % 5; //5 to inc. prob. of getting none.(3,4)

    switch (r) {

        case 0:
            branchPositions[0] = side::LEFT;
            break;

        case 1:
            branchPositions[0] = side::RIGHT;
            break;

        default:
            branchPositions[0] = side::NONE;
            break;
    }
}

//NOTES :
/*

    -> HUD (Head-Up Display) =  A method of visually representing information in video games.

*/
