#include "iGraphics.h"
#include <string>
#include <windows.h>
#include <mmsystem.h>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream> // Added for cout support
#pragma comment(lib, "winmm.lib")
#define buttonMusic mciSendString("open \"asset\\music\\button sound.wav\" type waveaudio alias buttonSound", NULL, 0, NULL);
#define octopusMusic mciSendString("open \"asset\\music\\octopus_collision.wav\" type waveaudio alias octopusSound", NULL, 0, NULL); // Added for octopus sound
#define bgMusic mciSendString("open \"asset\\music\\music_bg.wav\" type mpegvideo alias bgmusic", NULL, 0, NULL);
#define sharkSound mciSendString("open \"asset\\music\\shark_sound.wav\" type waveaudio alias sharkSound", NULL, 0, NULL);

using namespace std; // Added to resolve std namespace issues

// Screen size
int screenSizeX = 1000;
int screenSizeY = 800;
int maxDepth = 4000;

int playBg_y = screenSizeY;
int playBg_x = 0;
int rod_x = 237;
int rod_y = 490;
int string_length = 10;
int hook_radius = 5;
int hook_x = rod_x;
int hook_y = rod_y - 150;
int string_move_speed = 5;
int boat_speed = 5;
int screen_horizontal_speed = 7;

int cat_x = 5;
int cat_y = 365;

int screen_scroll_speed = 10;

const int MAX_FISH_NUMBER = 35;

// Background images
int menuBg, aboutUsBg, highScoreBg, playBg, catBg, gameOverBg, pauseOverlay;

// Fish variables
enum FishType { BLUEANGEL, HOLUD, LALTHOT, BEGUNITHOT, FREEZEORB, SHARK, HOLUDDORI, NEMO, OCTOPUS, MAGNETORB };

int blueAngel_left, blueAngel_right;
int begunithot_left, begunithot_right;
int holud_left, holud_right;
int lalthot_left, lalthot_right;
int shark_left, shark_right;
int holud_dori_left, holud_dori_right;
int nemo_left, nemo_right;
int octopus_left, octopus_right;
int freezeOrb;
int magnetOrb;

bool isFrozen = false;
int freezeDuration = 7000;
clock_t freezeStartTime = 0;
int freeze_bg;

int ink_bg;
bool isInk = false;
int inkDuration = 5000;
clock_t inkStartTime = 0;

bool isMagnet = false;
int magnetDuration = 5000;
clock_t magnetStartTime = 0;

int three_hp_pic, two_hp_pic, one_hp_pic;
int blood_image; // Blood image variable
bool showBlood = false; // Blood visibility status
int blood_x = 0, blood_y = 0; // Blood position
clock_t bloodStartTime = 0; // Blood image start time
int bloodDuration = 500; // Blood image duration in milliseconds

int total_money = 0;
int depth = 0;
int hp = 3;

// Money popup variables
int moneyPopupX = 0;
int moneyPopupY = 0;
int moneyPopupValue = 0;
int moneyPopupTimer = 0;

// High Score variables
struct HighScore {
	int score;
	HighScore(int s = 0) : score(s) {} // Default constructor with default value 0
	bool operator<(const HighScore& other) const { return score > other.score; } // For descending order
};
std::vector<HighScore> highScores;
const int MAX_HIGH_SCORES = 5;
const char* HIGH_SCORE_FILE = "highscores.txt";

// Button properties
struct Button {
	int x, y;
	int width, height;
	bool isHovered;
} buttons[4], gameOverButtons[2];

char* buttonLabels[] = { "Play", "High Score", "About Us", "Exit" };
char* gameOverButtonLabels[] = { "Restart", "Main Menu" };

struct Button backButton = { 20, screenSizeY - 75, 200, 50, false };

enum Screen { MENU, PLAY, HIGH_SCORE, ABOUT_US, GAME_OVER };
Screen currentScreen = MENU;
bool isPaused = false;

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

	startY = 300;
	for (int i = 0; i < 2; i++) {
		gameOverButtons[i].x = startX;
		gameOverButtons[i].y = startY - i * (buttonHeight + 20);
		gameOverButtons[i].width = buttonWidth;
		gameOverButtons[i].height = buttonHeight;
		gameOverButtons[i].isHovered = false;
	}
}

void drawBackButton() {
	if (currentScreen != MENU && currentScreen != GAME_OVER) {
		if (backButton.isHovered) {
			iSetColor(0, 102, 204);
			iFilledRectangle(backButton.x - 10, backButton.y - 5, backButton.width + 20, backButton.height + 10);
		}
		else {
			iSetColor(86, 164, 230);
			iFilledRectangle(backButton.x, backButton.y, backButton.width, backButton.height);
		}

		iSetColor(255, 255, 255);
		int textWidth = 8 * strlen("Back");
		int textX = backButton.x + (backButton.width - textWidth) / 2;
		int textHeight = 12;
		int textY = backButton.y + (backButton.height - textHeight) / 2;

		if (backButton.isHovered) {
			iText(textX, textY, "Back", GLUT_BITMAP_9_BY_15);
		}
		else {
			iText(textX, textY, "Back", GLUT_BITMAP_8_BY_13);
		}
	}
}

struct Fish {
	int fish_x, fish_y;
	int width, height;
	FishType type;
	int speed;
	bool movingRight;
	int imageRight;
	int imageLeft;
	bool isAttached = false;
	int money;
};

Fish fishArray[50];

Fish createFish() {
	Fish fish;
	int spawnRoll = rand() % 100;

	if (spawnRoll <= 60) {
		int randNum = rand() % 8;
		switch (randNum) {
		case 0:
			fish.type = BLUEANGEL;
			fish.width = 45;
			fish.height = 55;
			fish.speed = 1;
			fish.imageRight = blueAngel_right;
			fish.imageLeft = blueAngel_left;
			fish.money = 200;
			break;
		case 1:
			fish.type = LALTHOT;
			fish.width = 45;
			fish.height = 45;
			fish.speed = 5;
			fish.imageRight = lalthot_right;
			fish.imageLeft = lalthot_left;
			fish.money = 100;
			break;
		case 2:
			fish.type = SHARK;
			fish.width = 250;
			fish.height = 150;
			fish.speed = 10;
			fish.imageLeft = shark_left;
			fish.imageRight = shark_right;
			fish.money = 0;
			break;
		case 3:
			fish.type = HOLUDDORI;
			fish.width = 50;
			fish.height = 50;
			fish.speed = 10;
			fish.imageLeft = holud_dori_left;
			fish.imageRight = holud_dori_right;
			fish.money = 150;
			break;
		case 4:
			fish.type = FREEZEORB;
			fish.width = 50;
			fish.height = 50;
			fish.speed = 10;
			fish.imageLeft = freezeOrb;
			fish.imageRight = fish.imageLeft;
			fish.money = 50;
			break;
		case 5:
			fish.type = NEMO;
			fish.width = 90;
			fish.height = 50;
			fish.speed = 10;
			fish.imageLeft = nemo_left;
			fish.imageRight = nemo_right;
			fish.money = 300;
			break;
		case 6:
			fish.type = OCTOPUS;
			fish.width = 90;
			fish.height = 90;
			fish.speed = 10;
			fish.imageLeft = octopus_left;
			fish.imageRight = octopus_right;
			fish.money = 250;
			break;
		case 7:
			fish.type = MAGNETORB;
			fish.width = 45;
			fish.height = 45;
			fish.speed = 10;
			fish.imageLeft = magnetOrb;
			fish.imageRight = magnetOrb;
			fish.money = 50;
			break;
		default:
			fish.type = BEGUNITHOT;
			fish.width = 45;
			fish.height = 45;
			fish.speed = 5;
			fish.imageRight = begunithot_right;
			fish.imageLeft = begunithot_left;
			fish.money = 150;
			break;
		}
	}
	else {
		fish.type = HOLUD;
		fish.width = 45;
		fish.height = 45;
		fish.speed = 5;
		fish.imageRight = holud_right;
		fish.imageLeft = holud_left;
		fish.money = 100;
	}

	fish.fish_x = rand() % screenSizeX;
	fish.fish_y = -(rand() % (maxDepth));
	fish.movingRight = rand() % 2;

	return fish;
}

void spawnFish() {
	for (int i = 0; i < MAX_FISH_NUMBER; i++) {
		fishArray[i] = createFish();
	}
}

void drawFishes() {
	for (int i = 0; i < MAX_FISH_NUMBER; i++) {
		Fish &fish = fishArray[i];
		int fishImage = fish.movingRight ? fish.imageRight : fish.imageLeft;
		iShowImage(fish.fish_x, fish.fish_y, fish.width, fish.height, fishImage);
	}
}

void moveFishVertically(int direction) {
	for (int i = 0; i < MAX_FISH_NUMBER; i++) {
		Fish &fish = fishArray[i];
		fish.fish_y = fish.fish_y + (direction)* screen_scroll_speed;
	}
}

bool checkCollision(int hookX, int hookY, int hookRadius, Fish fish) {
	return !(hookX + hookRadius < fish.fish_x || hookX - hookRadius > fish.fish_x + fish.width ||
		hookY + hookRadius < fish.fish_y || hookY - hookRadius > fish.fish_y + fish.height);
}

void loadHighScores() {
	highScores.clear();
	std::ifstream file(HIGH_SCORE_FILE);
	if (file.is_open()) {
		int score;
		while (file >> score) {
			highScores.push_back(HighScore(score));
		}
		file.close();
		std::sort(highScores.begin(), highScores.end()); // Sort in descending order
		if (highScores.size() > MAX_HIGH_SCORES) {
			highScores.resize(MAX_HIGH_SCORES);
		}
	}
	else {
		cout << "Warning: Could not open " << HIGH_SCORE_FILE << ". Starting with empty high scores." << endl;
	}
}

void saveHighScores() {
	std::ofstream file(HIGH_SCORE_FILE);
	if (file.is_open()) {
		for (size_t i = 0; i < highScores.size() && i < MAX_HIGH_SCORES; i++) {
			file << highScores[i].score << std::endl;
		}
		file.close();
	}
	else {
		cout << "Error: Could not save to " << HIGH_SCORE_FILE << endl;
	}
}

void updateHighScores(int newScore) {
	highScores.push_back(HighScore(newScore));
	std::sort(highScores.begin(), highScores.end());
	if (highScores.size() > MAX_HIGH_SCORES) {
		highScores.resize(MAX_HIGH_SCORES);
	}
	saveHighScores();
}

void drawHighScore() {
	iClear();
	iShowImage(0, 0, screenSizeX, screenSizeY, highScoreBg);

	iSetColor(255, 255, 255);
	int titleWidth = 12 * strlen("High Scores");
	int titleX = (screenSizeX - titleWidth) / 2;
	int titleY = screenSizeY - 100;
	//iText(titleX, titleY, "High Scores", GLUT_BITMAP_HELVETICA_18);

	int startY = screenSizeY - 300; // Lowered text position as previously requested
	for (size_t i = 0; i < highScores.size(); i++) {
		char scoreText[50];
		sprintf_s(scoreText, sizeof(scoreText), "%d. Tk.%d", i + 1, highScores[i].score);
		iText(screenSizeX / 2 - 50, startY - i * 40, scoreText, GLUT_BITMAP_HELVETICA_18);
	}

	if (highScores.empty()) {
		iText(screenSizeX / 2 - 70, startY, "No scores yet!", GLUT_BITMAP_HELVETICA_18);
	}

	drawBackButton();
}

void resetGame() {
	hp = 3;
	total_money = 0;
	depth = 0;
	playBg_y = screenSizeY;
	cat_x = 5;
	cat_y = 365;
	rod_x = 237;
	rod_y = 490;
	hook_x = rod_x;
	hook_y = rod_y - 150;
	isFrozen = false;
	isInk = false;
	isMagnet = false;
	isPaused = false;
	spawnFish();
}

void successfulCatch(int i) {
	Fish &fish = fishArray[i];

	if (fish.type == SHARK) {
		// Check if any fish are attached
		bool hasAttachedFish = false;
		for (int j = 0; j < MAX_FISH_NUMBER; j++) {
			if (fishArray[j].isAttached) {
				hasAttachedFish = true;
				break;
			}
		}

		// If a shark is caught, decrement hp and respawn only the caught shark
		if (hp > 0) {
			hp--;
		}

		// Show blood image near the hook only if there are attached fish
		if (hasAttachedFish) {
			showBlood = true;
			blood_x = hook_x - 120;
			blood_y = hook_y - 120;
			bloodStartTime = clock(); // Start blood image timer
		}

		// Play shark sound only if the game is not over
		if (hp > 0) {
			mciSendString("play sharkSound from 0", NULL, 0, NULL);
		}

		// Iterate through the fish array and reset any fish that are attached to the hook
		for (int j = 0; j < MAX_FISH_NUMBER; j++) {
			if (fishArray[j].isAttached) {
				fishArray[j] = createFish();
			}
		}
		fishArray[i] = createFish(); // Also respawn the caught shark
	}
	else if (fish.type != SHARK && fish.type != OCTOPUS && fish.type != MAGNETORB) {
		total_money += fish.money;
		moneyPopupX = hook_x;
		moneyPopupY = hook_y;
		moneyPopupValue = fish.money;
		moneyPopupTimer = 30;
		mciSendString("play buttonSound from 0", NULL, 0, NULL);
	}

	fishArray[i] = createFish();

	// Check for game over after each catch
	if (hp <= 0) {
		currentScreen = GAME_OVER;
		mciSendString("open \"asset\\music\\game_over.wav\" type waveaudio alias gameOver", NULL, 0, NULL);
		mciSendString("play gameOver from 0", NULL, 0, NULL);
	}
}

void fishAttach(int i) {
	Fish &fish = fishArray[i];

	if (fish.type == FREEZEORB) {
		isFrozen = true;
		freezeStartTime = clock();
		successfulCatch(i);
		return;
	}
	else if (fish.type == SHARK) {
		successfulCatch(i); // Let successfulCatch handle hp decrement
		return;
	}
	else if (fish.type == OCTOPUS) {
		isInk = true;
		inkStartTime = clock();
		mciSendString("play octopusSound from 0", NULL, 0, NULL); // Play octopus sound on collision
		successfulCatch(i);
		return;
	}
	else if (fish.type == MAGNETORB) {
		isMagnet = true;
		magnetStartTime = clock();
		successfulCatch(i);
		return;
	}

	fish.fish_y = hook_y - fish.height / 2;
	fish.fish_x = hook_x - 5;
	fish.isAttached = true;

	if (hook_x == rod_x && hook_y >= rod_y - 150)
		successfulCatch(i);
}

void moveFish() {
	if (isPaused) return;

	if (isFrozen) {
		if ((clock() - freezeStartTime) * 1000 / CLOCKS_PER_SEC > freezeDuration) {
			isFrozen = false;
		}
		else {
			return;
		}
	}

	if (isMagnet) {
		if ((clock() - magnetStartTime) * 1000 / CLOCKS_PER_SEC > magnetDuration)
			isMagnet = false;
		else {
			for (int i = 0; i < MAX_FISH_NUMBER; i++) {
				// Skip octopuses and sharks
				if (fishArray[i].type == OCTOPUS || fishArray[i].type == SHARK) {
					continue;  // Move to next fish without applying magnet effect
				}

				int dx = hook_x - fishArray[i].fish_x;
				int dy = hook_y - fishArray[i].fish_y;
				float distance = sqrt(dx * dx + dy * dy);
				if (distance <= 200.0) {
					fishArray[i].fish_x += (dx / distance) * 10;
					fishArray[i].fish_y += (dy / distance) * 10;
					if (distance < hook_radius) {
						fishAttach(i);
					}
				}
			}
		}
	}

	for (int i = 0; i < MAX_FISH_NUMBER; i++) {
		Fish &fish = fishArray[i];
		fish.fish_x += fish.movingRight ? fish.speed : -fish.speed;
		if (fish.fish_x > screenSizeX) fish.fish_x = -fish.width;
		if (fish.fish_x + fish.width < 0) fish.fish_x = screenSizeX;
	}
}

void handleCollisions() {
	for (int i = 0; i < MAX_FISH_NUMBER; i++) {
		if (checkCollision(hook_x, hook_y, hook_radius, fishArray[i])) {
			fishAttach(i);
		}
	}
}

void drawMenu() {
	iClear();
	iShowImage(0, 0, screenSizeX, screenSizeY, menuBg);
	for (int i = 0; i < 4; i++) {
		Button btn = buttons[i];
		if (btn.isHovered) {
			iSetColor(0, 102, 204);
			iFilledRectangle(btn.x - 10, btn.y - 5, btn.width + 20, btn.height + 10);
		}
		else {
			iSetColor(86, 164, 230);
			iFilledRectangle(btn.x, btn.y, btn.width, btn.height);
		}

		iSetColor(255, 255, 255);
		int textWidth = 8 * strlen(buttonLabels[i]);
		int textX = btn.x + (btn.width - textWidth) / 2;
		int textHeight = 12;
		int textY = btn.y + (btn.height - textHeight) / 2;

		if (btn.isHovered) {
			iText(textX, textY, buttonLabels[i], GLUT_BITMAP_9_BY_15);
		}
		else {
			iText(textX, textY, buttonLabels[i], GLUT_BITMAP_8_BY_13);
		}
	}
}

void drawAboutUs() {
	iClear();
	iShowImage(0, 0, screenSizeX, screenSizeY, aboutUsBg);
	int titleWidth = 12 * strlen("About Us");
	int titleX = (screenSizeX - titleWidth) / 2;
	int titleY = screenSizeY - 100;
	//iSetColor(255, 255, 255);
	//iText(titleX, titleY, "About Us", GLUT_BITMAP_9_BY_15);
	drawBackButton();
}

void drawPlay() {
	iClear();
	iShowImage(playBg_x, -playBg_y - 2400, 1600, maxDepth, playBg);
	iShowImage(cat_x, cat_y, 240, 200, catBg);

	if (isFrozen)
		iShowImage(0, 0, screenSizeX, screenSizeY, freeze_bg);

	iSetColor(155, 75, 0);
	iLine(rod_x, rod_y, hook_x, hook_y);
	iSetColor(155, 75, 0);
	iFilledCircle(hook_x, hook_y, hook_radius);

	drawFishes();

	if (isInk) {
		iShowImage(0, 0, screenSizeX, screenSizeY, ink_bg);
		if ((clock() - inkStartTime) * 1000 / CLOCKS_PER_SEC > inkDuration)
			isInk = false;
	}

	if (!isPaused) {
		handleCollisions();
	}

	if (moneyPopupTimer > 0) {
		char popupText[20];
		sprintf_s(popupText, sizeof(popupText), "+Tk.%d", moneyPopupValue);
		iSetColor(0, 255, 0);
		iText(moneyPopupX, moneyPopupY, popupText, GLUT_BITMAP_HELVETICA_12);
		moneyPopupY += 1;
		moneyPopupTimer--;
	}

	char moneyText[50];
	sprintf_s(moneyText, sizeof(moneyText), "Total Money: Tk.%d", total_money);
	iSetColor(255, 255, 0);
	iText(20, screenSizeY - 40, moneyText, GLUT_BITMAP_HELVETICA_18);

	if (hp == 3) {
		iShowImage(screenSizeX - 150, screenSizeY - 40, 150, 33, three_hp_pic);
	}
	else if (hp == 2) {
		iShowImage(screenSizeX - 150, screenSizeY - 40, 150, 33, two_hp_pic);
	}
	else if (hp == 1) {
		iShowImage(screenSizeX - 150, screenSizeY - 40, 150, 33, one_hp_pic);
	}

	char depthText[50];
	sprintf_s(depthText, sizeof(depthText), "Depth: %dm", depth);
	iSetColor(255, 255, 0);
	iText(screenSizeX - 110, screenSizeY - 60, depthText, GLUT_BITMAP_HELVETICA_18);

	if (isPaused) {
		iShowImage(0, 0, screenSizeX, screenSizeY, pauseOverlay);
		//iSetColor(255, 255, 255);
		//iText(screenSizeX / 2 - 40, screenSizeY / 2, "PAUSED", GLUT_BITMAP_HELVETICA_18);
	}
	// Draw blood image if visible and within the duration
	if (showBlood) {
		iShowImage(blood_x, blood_y, 300, 300, blood_image);
		if ((clock() - bloodStartTime) * 1000 / CLOCKS_PER_SEC > bloodDuration) {
			showBlood = false;
		}
	}
}

void drawGameOver() {
	iClear();
	iShowImage(0, 0, screenSizeX, screenSizeY, gameOverBg);

	static bool scoreUpdated = false;
	if (!scoreUpdated) {
		updateHighScores(total_money);
		scoreUpdated = true;
	}

	char finalScore[50];
	sprintf_s(finalScore, sizeof(finalScore), "Final Money: Tk.%d", total_money);
	iSetColor(0, 0, 139);
	iText(screenSizeX / 2 - 70, screenSizeY - 370, finalScore, GLUT_BITMAP_HELVETICA_18);

	for (int i = 0; i < 2; i++) {
		Button btn = gameOverButtons[i];
		if (btn.isHovered) {
			iSetColor(0, 102, 204);
			iFilledRectangle(btn.x - 10, btn.y - 5, btn.width + 20, btn.height + 10);
		}
		else {
			iSetColor(86, 164, 230);
			iFilledRectangle(btn.x, btn.y, btn.width, btn.height);
		}

		iSetColor(255, 255, 255);
		int textWidth = 8 * strlen(gameOverButtonLabels[i]);
		int textX = btn.x + (btn.width - textWidth) / 2;
		int textHeight = 12;
		int textY = btn.y + (btn.height - textHeight) / 2;

		if (btn.isHovered) {
			iText(textX, textY, gameOverButtonLabels[i], GLUT_BITMAP_9_BY_15);
		}
		else {
			iText(textX, textY, gameOverButtonLabels[i], GLUT_BITMAP_8_BY_13);
		}
	}
}

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
	else if (currentScreen == GAME_OVER) {
		drawGameOver();
	}
}

void iPassiveMouseMove(int mx, int my) {
	if (currentScreen == MENU) {
		for (int i = 0; i < 4; i++) {
			if (mx >= buttons[i].x && mx <= buttons[i].x + buttons[i].width &&
				my >= buttons[i].y && my <= buttons[i].y + buttons[i].height) {
				buttons[i].isHovered = true;
			}
			else {
				buttons[i].isHovered = false;
			}
		}
	}
	else if (currentScreen == GAME_OVER) {
		for (int i = 0; i < 2; i++) {
			if (mx >= gameOverButtons[i].x && mx <= gameOverButtons[i].x + gameOverButtons[i].width &&
				my >= gameOverButtons[i].y && my <= gameOverButtons[i].y + gameOverButtons[i].height) {
				gameOverButtons[i].isHovered = true;
			}
			else {
				gameOverButtons[i].isHovered = false;
			}
		}
	}
	else {
		if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
			my >= backButton.y && my <= backButton.y + backButton.height) {
			backButton.isHovered = true;
		}
		else {
			backButton.isHovered = false;
		}
	}
}

void iMouse(int button, int state, int mx, int my) {
	buttonMusic
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (currentScreen == MENU) {
			for (int i = 0; i < 4; i++) {
				if (mx >= buttons[i].x && mx <= buttons[i].x + buttons[i].width &&
					my >= buttons[i].y && my <= buttons[i].y + buttons[i].height) {
					mciSendString("play buttonSound from 0", NULL, 0, NULL);
					if (i == 0) {
						currentScreen = PLAY;
						resetGame();
					}
					else if (i == 1) currentScreen = HIGH_SCORE;
					else if (i == 2) currentScreen = ABOUT_US;
					else if (i == 3) exit(0);
				}
			}
		}
		else if (currentScreen == GAME_OVER) {
			for (int i = 0; i < 2; i++) {
				if (mx >= gameOverButtons[i].x && mx <= gameOverButtons[i].x + gameOverButtons[i].width &&
					my >= gameOverButtons[i].y && my <= gameOverButtons[i].y + gameOverButtons[i].height) {
					mciSendString("play buttonSound from 0", NULL, 0, NULL);
					if (i == 0) {
						currentScreen = PLAY;
						resetGame();
					}
					else if (i == 1) {
						currentScreen = MENU;
					}
				}
			}
		}
		else if (currentScreen == ABOUT_US || currentScreen == HIGH_SCORE || currentScreen == PLAY) {
			if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
				my >= backButton.y && my <= backButton.y + backButton.height) {
				mciSendString("play buttonSound from 0", NULL, 0, NULL);
				currentScreen = MENU;
			}
		}
	}
}

void iMouseMove(int mx, int my) {}

void iKeyboard(unsigned char key) {
	if (currentScreen != PLAY) return;

	if (key == 'p' || key == 'P') {
		isPaused = !isPaused;
		if (isPaused) {
			mciSendString("pause bgmusic", NULL, 0, NULL);
			cout << "Game Paused!" << endl;
		}
		else {
			mciSendString("resume bgmusic", NULL, 0, NULL);
			cout << "Game Resumed!" << endl;
		}
		return;
	}

	if (!isPaused) {
		if (key == '\r') cout << "Enter key pressed!" << endl;

		if (key == 'w') {
			if (playBg_y < screenSizeY) {
				playBg_y += screen_scroll_speed;
				cat_y -= screen_scroll_speed;
				rod_y -= screen_scroll_speed;
				moveFishVertically(-1);
				depth--;
			}
			if (hook_y < 340) {
				hook_y += string_move_speed;
				depth--;
			}
		}
		if (key == 's') {
			if (playBg_y > -2400) {
				playBg_y -= screen_scroll_speed;
				cat_y += screen_scroll_speed;
				rod_y += screen_scroll_speed;
				moveFishVertically(1);
				depth++;
			}
			else {
				if (hook_y > 0) {
					hook_y -= string_move_speed;
					depth++;
				}
			}
			if (hook_y > 350) hook_y -= string_move_speed;
		}
		else if (key == 'a') {
			if (hook_x > 0) hook_x -= string_move_speed;
		}
		else if (key == 'd') {
			if (hook_x < screenSizeX) hook_x += string_move_speed;
		}
	}
}

void iSpecialKeyboard(unsigned char key) {
	if (currentScreen != PLAY || isPaused) return;

	if (key == GLUT_KEY_RIGHT) {
		if (cat_x < 800) {
			cat_x += boat_speed;
			rod_x += 5;
		}
		if (playBg_x > -600) playBg_x -= screen_horizontal_speed;
	}
	if (key == GLUT_KEY_LEFT) {
		if (cat_x > 0) {
			cat_x -= boat_speed;
			rod_x -= 5;
		}
		if (playBg_x < 0) playBg_x += screen_horizontal_speed;
	}
}

void loadRandomBg() {
	menuBg = iLoadImage("asset\\background\\menu_bg.png");
	aboutUsBg = iLoadImage("asset\\background\\about us.png");
	highScoreBg = iLoadImage("asset\\background\\highscore_bg.png");
	catBg = iLoadImage("asset\\background\\cat.png");
	gameOverBg = iLoadImage("asset\\background\\game_over.png");
	pauseOverlay = iLoadImage("asset\\background\\pause_overlay.png");
	int randomNum = rand() % 4;
	freeze_bg = iLoadImage("asset\\background\\freeze_bg.png");
	ink_bg = iLoadImage("asset\\background\\ink_bg.png");

	switch (randomNum) {
	case 0: playBg = iLoadImage("asset\\background\\green_hill_bg.png"); break;
	case 1: playBg = iLoadImage("asset\\background\\cherry_bg.png"); break;
	case 2: playBg = iLoadImage("asset\\background\\night_bg.png"); break;
	default: playBg = iLoadImage("asset\\background\\morning_bg.png"); break;
	}
}

void loadPower() {
	three_hp_pic = iLoadImage("asset\\power\\three_hp.png");
	two_hp_pic = iLoadImage("asset\\power\\two_hp.png");
	one_hp_pic = iLoadImage("asset\\power\\one_hp.png");
	freezeOrb = iLoadImage("asset\\power\\freeze_orb.png");
	magnetOrb = iLoadImage("asset\\power\\magnet_orb.png");
	blood_image = iLoadImage("asset\\background\\blood.png"); // Load blood image
}

void loadFish() {
	blueAngel_left = iLoadImage("asset\\fish\\blueAngel_left.png");
	blueAngel_right = iLoadImage("asset\\fish\\blueAngel_right.png");
	lalthot_left = iLoadImage("asset\\fish\\lalthot_left.png");
	lalthot_right = iLoadImage("asset\\fish\\lalthot_right.png");
	holud_left = iLoadImage("asset\\fish\\holud_left.png");
	holud_right = iLoadImage("asset\\fish\\holud_right.png");
	begunithot_left = iLoadImage("asset\\fish\\begunithot_left.png");
	begunithot_right = iLoadImage("asset\\fish\\begunithot_right.png");
	shark_left = iLoadImage("asset\\fish\\shark_left.png");
	shark_right = iLoadImage("asset\\fish\\shark_right.png");
	holud_dori_left = iLoadImage("asset\\fish\\holud_dori_left.png");
	holud_dori_right = iLoadImage("asset\\fish\\holud_dori_right.png");
	nemo_left = iLoadImage("asset\\fish\\nemo_left.png");
	nemo_right = iLoadImage("asset\\fish\\nemo_right.png");
	octopus_left = iLoadImage("asset\\fish\\octopus_left.png");
	octopus_right = iLoadImage("asset\\fish\\octopus_right.png");
}

int main() {
	iInitialize(screenSizeX, screenSizeY, "Fun-tastic Catch!");

	srand(time(0));
	loadRandomBg();
	loadFish();
	loadPower();
	spawnFish();
	loadHighScores();

	iSetTimer(20, moveFish);
	iSetTimer(33, []() {
		if (moneyPopupTimer > 0) {
			moneyPopupY += 1;
			moneyPopupTimer--;
		}
	});

	bgMusic
		mciSendString("play bgmusic repeat", NULL, 0, NULL);
	octopusMusic // Load the octopus sound
		sharkSound // Load the shark sound

		initializeButtons();
	iStart();

	mciSendString("stop bgmusic repeat", NULL, 0, NULL);
	mciSendString("close bgmusic repeat", NULL, 0, NULL);
	return 0;
}
