#pragma once

#include "ofMain.h"
#include "ofxGLWarper.h"
#include "ofxTween.h"
#include "GameState.h"
#include "SheetData.h"
#include "StaticSheetData.h"
#include "TickTimer.h"
#include "ofxESCPOS.h"
//#define DEBUG_OUTPUT
//#define TEST_PRINT_SCREEN 
using namespace ofx;
#define LOAD_WARPERS_POSITIONS_ON_SETUP 1
#define FADING_TIME 1
#define FONT_TIMER_SIZE 150
#define FONT_TIMER_X 15 
#define FONT_TIMER_Y 220
#define FONT_POINTS_SIZE 130
#define FONT_POINTS_X 18 
#define FONT_POINTS_Y 155
#define GAME_MAX_TIME 180 // sg
#define ANSWERED_STAY_TIME 2 // sg

// para los botones de la pantalla ReadyToStart
#define APP_SOUND_ON 1
#define APP_SOUND_OFF 2
#define APP_START 3

// para las imagenes de las cajas de puntuacion
// y los videos que ahora son genéricos
#define PATH_IMG_SCORE "imgScore.jpg"
#define PATH_IMG_TIMER "imgTimer.jpg"
#define PATH_VIDEO_RIGHT "vidRight.mp4"
#define PATH_VIDEO_LEFT "vidLeft.mp4"


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		void keyPressed(int key);		

	private:
		/* evento para teclas. Lo hago con evento por si luego lo necesita la placa de los botones */
		ofEvent<int> keyTouchEvent;
		void onKeyTouchEvent(int & value);
		/* Lee el xml de las fichas en otro hilo*/
		//SheetsXmlReader reader;
		/* las fichas dinámicas leidas por el lector de xml*/
		vector<SheetData*> sheetsData;	
		/* la ficha actual */
		SheetData* currentSheet;
		// para puntuaciones, etc ...
		SheetData* lastSheet;
		/* dibuja la ficha actual */
		void drawCurrentSheet();
		/* para depuración */
		void debugDump(SheetData * data);

		/* callback del evento lanzado al terminar la lectura del xml*/
		//void xmlRead(bool & success); 
		/* imprime el ticket */
		void printTicket(void);

		/* inicializa los warpers, imagenes y videos */
		void initGraphics(SheetData* data);
		
		/* estado del juego */
		GameState gameState;
		bool stateChanged;
		bool lastQuestionSuccess;

		/* puntuación y timer visibles durante el juego */
		int points = 0;
		int timerElapsed = 0;

		/* juego con sonido o sin el */
		bool soundEnabled;
		/* booleano que indica si update() maneja los  fades */
		bool skipFadeInOnUpdate;

		/* imagenes interface */
		ofImage imgBoxQuestion;
		ofImage imgBoxScore;
		ofImage imgBoxTimer;

		/* imagenes botones */
		ofImage imgBoxButtonOne;
		ofImage imgBoxButtonTwo;
		ofImage imgBoxButtonThree;

		/* warpers */
		ofxGLWarper warperBoxQuestions;
		ofxGLWarper warperBoxScore;
		ofxGLWarper warperBoxTimer;
		ofxGLWarper warperBoxButtonOne;
		ofxGLWarper warperBoxButtonTwo;
		ofxGLWarper warperBoxButtonThree;

		/* para manejar los warpers y el posicionamiento */
		void deactivateWarpers();
		void saveWarpersToXml();
		void loadWarpersFromXml();

		/* para las transiciones alpha */
		int fadeTotalTime;
		int fadeStartTime;
		int64_t fadeEndTime;
		bool isFadingOut;
		bool isFadingIn;
		bool timedOut;
		int currentAlpha;
		bool isFading();
		void fadeIn();
		void resetFade();
		void fadeOut();

		/* textos en pantallas */
		ofFbo fboPoints;
		ofFbo fboTimer;
		ofTrueTypeFont fontPoints;
		ofTrueTypeFont fontTimer;

		/* baraja las fichas */
		void shuffleSheets(void);

		/* app exit */
		void exit();

		/* Temporazador */
		TickTimer gameTimer;
		string timerFormatTime(int seconds);
		TickTimer questionAnsweredTimer;

		/* 
			Pantallas estáticas. Estas no varian 
			ni se cargan del XML
		*/
		StaticSheetData* startSheet;
		StaticSheetData* readyToPlaySheet;
		StaticSheetData* successStaticSheet;
		StaticSheetData* failStaticSheet;
		StaticSheetData* endStaticSheet;
		StaticSheetData* printStaticSheet;

		bool gameBeingPlayed();
		void resetGame();

		void readXml(string xmlFileName);

		ESCPOS::DefaultSerialPrinter printer;

		void readSensors();
		ofSerial pcb;

		string message;
		string			messageBuffer;
		unsigned char	bytesReturned[1];

		ofImage bg;
		ofFbo container;

		ofTrueTypeFont genial, info, numAciertos, aciertos;


		/* -------- NO USADAS ------------------------------
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
		------------------------------------------------- */
		~ofApp();
		
};

