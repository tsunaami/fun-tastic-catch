#include "iGraphics.h"
#include <string>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#define buttonMusic mciSendString("open \"asset\\music\\button sound.wav\" type waveaudio alias buttonSound", NULL, 0, NULL);
#define bgMusic mciSendString("open \"asset\\music\\music_bg.wav\" type mpegvideo alias bgmusic", NULL, 0, NULL);

// Screen size
int screenSizeX = 1000;
int screenSizeY = 800;
int playBg_y = screenSizeY;
int rod_x = 218;
int rod_y = 530;
int string_length = 10;
int hook_radius = 5;
int hook_x = rod_x;
int hook_y = rod_y;
int string_move_speed = 5;

int screen_scroll_speed = 10;

const int MAX_FISH_NUMBER = 30;

// Background images
int menuBg, aboutUsBg, highScoreBg, playBg; // Added playBg

//Fish var
enum FishType { BLUEANGEL, HOLUD, LALTHOT, BEGUNITHOT };

//
int blueAngel_left;
int blueAngel_right;

int begunithot_left;
int begunithot_right;

int holud_left;
int holud_right;

int lalthot_left;
int lalthot_right;



// Button properties
struct Button {
	int x, y;
	int width, height;
	bool isHovered;
} buttons[4];

// Button labels
char* buttonLabels[] = { "Play", "High Score", "About Us", "Exit" };

// Back button properties (for all screens)
struct Button backButton = { 20, screenSizeY - 75, 200, 50, false };

// Current screen state
enum Screen { MENU, PLAY, HIGH_SCORE, ABOUT_US };
Screen currentScreen = MENU;

// Initialize button positions and sizes
void initializeButtons() {
	int buttonWidth = 200;
	int buttonHeight = 50;
	int startX = (screenSizeX - buttonWidth) / 2;
	int startY = 400;

	for (int i = 0; i < 4; i++) {
		buttons[i].x = startX;
		buttons[i].y = startY - i * (buttonHeight + 20);
		buttons[i].width = buttonWidth;
		buttons[i].height = buttonHeight;
		buttons[i].isHovered = false;
	}
}

// Back button 16jan
void drawBackButton() {
	if (currentScreen != MENU) {
		// Draw button background
		if (backButton.isHovered) {
			iSetColor(0, 102, 204);  // Hover color next page
			iFilledRectangle(backButton.x - 10, backButton.y - 5, backButton.width + 20, backButton.height + 10);
		}
		else {
			iSetColor(86, 164, 230);   // Normal color
			iFilledRectangle(backButton.x, backButton.y, backButton.width, backButton.height);
		}

		// Center the text
		iSetColor(255, 255, 255);  // White text

		int textWidth = 8 * strlen("Back"); // Approximate text width
		int textX = backButton.x + (backButton.width - textWidth) / 2;

		int textHeight = 12; // Approximate text height for HELVETICA_18
		int textY = backButton.y + (backButton.height - textHeight) / 2;

		// Draw "Back" text
		if (backButton.isHovered) {
			iText(textX, textY, "Back", GLUT_BITMAP_9_BY_15); // Larger font on hover
		}
		else {
			iText(textX, textY, "Back", GLUT_BITMAP_8_BY_13); // Smaller font when not hovered
		}
	}
}


//FISHFISH
struct Fish{
	int x, y; //position
	int width, height;
	FishType type;

	int speed;
	bool movingRight;
	int imageRight;
	int imageLeft;
	//FishType type;
};
Fish fishArray[50];

Fish createFish() {

	Fish fish;
	int spawnRoll = rand() % 100;


	if (spawnRoll <= 60){

		int randNum = rand() % 2;
		switch (randNum)
		{
			

		case 0:
			fish.type = BLUEANGEL;
			fish.width = 45;
			fish.height = 55;
			fish.speed = 1;
			fish.imageRight = blueAngel_right;
			fish.imageLeft = blueAngel_left;
			printf("type 0");
			break;

		case 1:
			fish.type = LALTHOT;
			fish.width = 45;
			fish.height = 45;
			fish.speed = 5;
			fish.imageRight = lalthot_right;
			fish.imageLeft = lalthot_left;
			printf("type 00");
			break;


		default:
			fish.type =BEGUNITHOT;
			fish.width = 45;
			fish.height = 45;
			fish.speed = 5;
			fish.imageRight = begunithot_right;
			fish.imageLeft = begunithot_left;
			printf("type 000");
			break;

			
		}

	}
	else{
		fish.type = HOLUD;
		fish.width = 45;
		fish.height = 45;
		fish.speed = 5;
		fish.imageRight = holud_right;
		fish.imageLeft = holud_left;
		printf("type 1");
	}
	





	// Random position and direction
	fish.x = rand() % screenSizeX;
	fish.y = rand() % (screenSizeY/2);  //need further modification
	fish.movingRight = rand() % 2;

	return fish;
}

void spawnFish() {

	for (int i = 0; i < 5; i++) {
		fishArray[i] = createFish();
	}

}

void drawFishes() {

	for (int i = 0; i < 5; i++) {

		Fish &fish = fishArray[i];
		char imageRight[100], imageLeft[100];
		int fishImage = fish.movingRight ? fish.imageRight : fish.imageLeft;
		iShowImage(fish.x, fish.y, fish.width, fish.height, fishImage);

	}
}

void moveFish() {
	for (int i = 0; i <5; i++) {
		Fish &fish = fishArray[i];
		fish.x += fish.movingRight ? fish.speed : -fish.speed;

		// Wrap around screen
		if (fish.x > screenSizeX) fish.x = -fish.width;
		if (fish.x + fish.width < 0) fish.x = screenSizeX;
	}





}

// Draw the main menu screen 16 jan
void drawMenu() {
	iClear();
	iShowImage(0, 0, screenSizeX, screenSizeY, menuBg);
	for (int i = 0; i < 4; i++) {
		Button btn = buttons[i];

		// Draw button background
		if (btn.isHovered) {
			iSetColor(0, 102, 204);  // Hover color
			iFilledRectangle(btn.x - 10, btn.y - 5, btn.width + 20, btn.height + 10);
		}
		else {
			iSetColor(86, 164, 230);   // Normal color
			iFilledRectangle(btn.x, btn.y, btn.width, btn.height);
		}

		// Draw button text
		iSetColor(255, 255, 255);  // White text

		// Calculate centered text position
		int textWidth = 8 * strlen(buttonLabels[i]); // Approximate width per character
		int textX = btn.x + (btn.width - textWidth) / 2;

		int textHeight = 12; // Approximate text height for HELVETICA_18
		int textY = btn.y + (btn.height - textHeight) / 2;

		// Add bold effect when hovered
		if (btn.isHovered) {
			iText(textX, textY, buttonLabels[i], GLUT_BITMAP_9_BY_15);
		}
		else {
			iText(textX, textY, buttonLabels[i], GLUT_BITMAP_8_BY_13);
		}
	}
}

// Draw the "About Us" screen 16 jan
void drawAboutUs() {
	iClear();
	iShowImage(0, 0, screenSizeX, screenSizeY, aboutUsBg);
	// Draw the title
	int titleWidth = 12 * strlen("About Us");
	int titleX = (screenSizeX - titleWidth) / 2;
	int titleY = screenSizeY - 100;  // Slightly below the top

	iSetColor(255, 255, 255);
	iText(titleX, titleY, "About Us", GLUT_BITMAP_9_BY_15); //next pg chng

	drawBackButton();
}

// Draw the "High Score" screen
void drawHighScore() {
	iClear();
	iShowImage(0, 0, screenSizeX, screenSizeY, highScoreBg);
	drawBackButton();
}

// Draw the "Play" screen (Game screen, could be just a placeholder for now)
void drawPlay() {
	iClear();
	iShowImage(0, -playBg_y, screenSizeX, screenSizeY * 2, playBg); // Display the play background image
	//iSetColor(155, 75, 0);
	// iFilledRectangle(218,530, 50, 15);
	iSetColor(155, 75, 0);
	iLine(rod_x, rod_y, hook_x, hook_y);
	iSetColor(155, 75, 0);
	iFilledCircle(hook_x, hook_y, hook_radius);
	drawFishes();
}

// Main drawing function
void iDraw() {
	if (currentScreen == MENU) {
		drawMenu();
	}
	else if (currentScreen == ABOUT_US) {
		drawAboutUs();
	}
	else if (currentScreen == HIGH_SCORE) {
		drawHighScore();
	}
	else if (currentScreen == PLAY) {
		drawPlay();
	}
}

// Handle passive mouse movement to check if the cursor is over a button
void iPassiveMouseMove(int mx, int my) {
	for (int i = 0; i < 4; i++) {
		if (mx >= buttons[i].x && mx <= buttons[i].x + buttons[i].width &&
			my >= buttons[i].y && my <= buttons[i].y + buttons[i].height) {
			buttons[i].isHovered = true;
		}
		else {
			buttons[i].isHovered = false;
		}
	}

	// Check if the mouse is hovering over the back button
	if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
		my >= backButton.y && my <= backButton.y + backButton.height) {
		backButton.isHovered = true;
	}
	else {
		backButton.isHovered = false;
	}
}

// Handle mouse clicks for navigation 16 jan
void iMouse(int button, int state, int mx, int my) {
	buttonMusic
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < 4; i++) {
			if (mx >= buttons[i].x && mx <= buttons[i].x + buttons[i].width &&
				my >= buttons[i].y && my <= buttons[i].y + buttons[i].height) {

				// Play sound when a button is clicked
				mciSendString("play buttonSound from 0", NULL, 0, NULL);

				// Handle button clicks to change screens
				if (i == 0) {
					currentScreen = PLAY;  // Start the game
				}
				else if (i == 1) {
					currentScreen = HIGH_SCORE;  // Show high scores
				}
				else if (i == 2) {
					currentScreen = ABOUT_US;  // Show about us information
				}
				else if (i == 3) {
					exit(0);  // Exit the game
				}
			}
		}

		// Back to Menu (for "About Us" and "High Score" screens)
		if (currentScreen == ABOUT_US || currentScreen == HIGH_SCORE || currentScreen == PLAY) {
			if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
				my >= backButton.y && my <= backButton.y + backButton.height) {

				// Play sound when the back button is clicked
				mciSendString("play buttonSound from 0", NULL, 0, NULL);

				currentScreen = MENU;  // Go back to the main menu
			}
		}
	}
}

void iMouseMove(int mx, int my) {
	// Empty function to handle mouse dragging
}

void iKeyboard(unsigned char key) {
	if (key == '\r') {
		printf("Enter key pressed!\n");
	}
	if (key == 'p') {
		printf("P key pressed - Game paused!\n");
	}

	// Adjust background position based on key inputs
	if (key == 'w'){

		if (playBg_y < screenSizeY) {
			playBg_y += screen_scroll_speed; // Move up, but do not go above the top of the image
			
			rod_y -= screen_scroll_speed;
		}
		if (hook_y<530){
			hook_y += string_move_speed;
		}

	}
	if (key == 's'){

		if (playBg_y>0){
			playBg_y -= screen_scroll_speed;
			
			rod_y += screen_scroll_speed;// Move down continuously *********
		}

		if (hook_y>0)
			hook_y -= string_move_speed;
	

		
	}
	

	else if (key == 'a') { // Move the hook left
		if (hook_x > 0) {
			hook_x -= string_move_speed;
		}
	}
	else if (key == 'd') { // Move the hook right
		if (hook_x < screenSizeX) {
			hook_x += string_move_speed;
		}
	}
}



void iSpecialKeyboard(unsigned char key) {
	if (key == GLUT_KEY_RIGHT) {
		printf("Right arrow key pressed!\n");
	}
	if (key == GLUT_KEY_LEFT) {
		printf("Left arrow key pressed!\n");
	}
	if (key == GLUT_KEY_UP) {
		printf("Up arrow key pressed!\n");
	}
	if (key == GLUT_KEY_DOWN) {
		printf("Down arrow key pressed!\n");
	}
	if (key == GLUT_KEY_HOME) {
		printf("Home key pressed!\n");
	}
}

int main() {
	iInitialize(screenSizeX, screenSizeY, "Fun-tastic Catch!");

	// Load background images
	menuBg = iLoadImage("asset\\background\\menu_bg.png");
	aboutUsBg = iLoadImage("asset\\background\\about us.png");
	highScoreBg = iLoadImage("asset\\background\\menu_bg.png");
	playBg = iLoadImage("asset\\background\\pbg.png"); // Load play background image

	//load bunch of fishes
	blueAngel_left = iLoadImage("asset\\fish\\blueAngel_left.png");
	blueAngel_right = iLoadImage("asset\\fish\\blueAngel_left.png");

	lalthot_left = iLoadImage("asset\\fish\\lalthot_left.png");
	lalthot_right = iLoadImage("asset\\fish\\lalthot_left.png");

	holud_left=iLoadImage("asset\\fish\\holud_left.png");
	holud_right = iLoadImage("asset\\fish\\holud_left.png");

	begunithot_left = iLoadImage("asset\\fish\\begunithot_left.png");
	begunithot_left = iLoadImage("asset\\fish\\begunithot_left.png");


	srand(time(0));
	spawnFish();
	iSetTimer(20, moveFish);

	// Play background music in a loop, and ensure it doesn't stop when switching screens
	bgMusic
		mciSendString("play bgmusic repeat", NULL, 0, NULL);

	initializeButtons();
	iStart();
	mciSendString("stop bgmusic repeat", NULL, 0, NULL);
	mciSendString("close bgmusic repeat", NULL, 0, NULL);
	return 0;
}