#include "ofApp.h"
#include "ofxXmlSettings.h"
#include <algorithm>
#include <random>

//--------------------------------------------------------------
void ofApp::setup(){
	ofHideCursor();
	ofSetBackgroundColor(0);
	ofSetVerticalSync(true);
	// eventos
	ofAddListener(keyTouchEvent, this, &ofApp::onKeyTouchEvent);
	//keyTouchEvent
	// variabless de estado
	fadeTotalTime = FADING_TIME * 1000;
	fadeStartTime = 0;
	fadeEndTime = 0;
	isFadingOut = false;
	isFadingIn = false;
	currentAlpha = 255;

	// carga los datos de las pantallas estáticas
	startSheet = new StaticSheetData("START");
	readyToPlaySheet = new StaticSheetData("READY");
	endStaticSheet = new StaticSheetData("FINAL");
	successStaticSheet = new StaticSheetData("SUCCESS",SheetData::Type::Solution);
	failStaticSheet = new StaticSheetData("FAIL", SheetData::Type::Solution);
	printStaticSheet = new StaticSheetData("PRINT");

	// inicializa los warpers con los datos 
	// de la pantalla inicial	
	initGraphics(startSheet);
	currentSheet = startSheet;

#ifdef LOAD_WARPERS_POSITIONS_ON_SETUP
	loadWarpersFromXml();
#endif // LOAD_WARPERS_ON_SETUP

	// inicialización
	// por defecto sonido on
	soundEnabled = true;
	stateChanged = false; 
	skipFadeInOnUpdate = false;

	// fuentes
	fontPoints.load("font.ttf", FONT_POINTS_SIZE);
	fontTimer.load("font.ttf", FONT_TIMER_SIZE);

	// lee el xml de las fichas
	readXml("fichas.xml");
	gameState.setNumberOfQuestions(static_cast<int>(sheetsData.size()));
	fadeOut();

	//init printer
	if (!printer.setup("COM9", 38400))
	{
		ofLogError("ofApp::setup") << "Unable to connect to: " << printer.port();

		//ofExit();
	}
	else
	{
		ofLogNotice("ofApp::setup") << "Connected to: " << printer.port();

		cout << "Connected to: " << printer.port();

		// Set up hardware flow control if needed.
		printer.setDataTerminalReady();
		printer.setRequestToSend();

		// Initialize the printer.
		printer.initialize();
		printer.flush();
	}




	// open PCB
	if (pcb.setup("COM7", 9600)) {
		cout << "Connected to: COM7" << endl;
	}

	// tocket
	bg.load("ticket.jpg");
	container.allocate(bg.getWidth(), bg.getHeight());
	genial.loadFont("Gotham/GothamHTF-Medium.otf", 17);
	info.loadFont("Gotham/GothamHTF-Medium.otf", 12);
	numAciertos.loadFont("Gotham/GothamHTF-Medium.otf", 140);
	aciertos.loadFont("Gotham/GothamHTF-Medium.otf", 33);
}

// Inicializaciones de los objetos de la primera pantalla
// los objetos serán del mismo tamaño en el resto de las
// pantallas.
// Usamos las imagenes y videos de la primera pantalla
// como "plantillas" de los tamaños.
void ofApp::initGraphics(SheetData* data) {
	/*
	 * Hay tambien graficos estáticos que inicializar aqui
	 * - Score
	 * - Timer
	 * - Video derecha
	 * - Video izquierda
	 */
	imgBoxScore.load(PATH_IMG_SCORE);
	imgBoxTimer.load(PATH_IMG_TIMER);

	warperBoxQuestions.setup(data->imgQuestion.getWidth(), data->imgQuestion.getHeight());

	warperBoxScore.setup(imgBoxScore.getWidth(), imgBoxScore.getHeight());
	fboPoints.allocate(imgBoxScore.getWidth(), imgBoxScore.getHeight());
	fboPoints.begin();
	ofClear(255);
	fboPoints.end();

	warperBoxTimer.setup(imgBoxTimer.getWidth(), imgBoxTimer.getHeight());
	fboTimer.allocate(imgBoxTimer.getWidth(), imgBoxTimer.getHeight());
	fboTimer.begin();
	ofClear(255);
	fboTimer.end();

	warperBoxButtonOne.setup(data->imgButtonOne.getWidth(), data->imgButtonOne.getHeight());
	warperBoxButtonTwo.setup(data->imgButtonTwo.getWidth(), data->imgButtonTwo.getHeight());
	warperBoxButtonThree.setup(data->imgButtonThree.getWidth(), data->imgButtonThree.getHeight());
}

// En update se mira también como está el timer
void ofApp::update() {
	// si estamos jugando el contador sigue corriendo
	if (gameBeingPlayed()) {
		if (gameTimer.isRunning()) {
			timerElapsed = gameTimer.tick();
			if (timerElapsed > GAME_MAX_TIME) { // si el tiempo a transcurrido finaliza el juego
#ifdef DEBUG_OUTPUT
				cout << "Timer end : " << endl;
#endif // DEBUG
				gameTimer.stop(); // si no reseteamos el timer aqui entrará en loop ya que timerElapsed siempre será mayor que QUESTION_MAX_TIME, fadeOut() reiniciará el timer y vuelta a empezar
				gameState.finishGame();
				fadeOut();
			}
		}
		// en la pantalla de respuesta esperamos solo un tiempo señalado 
		// y pasamos a la siguiente. Para esto hay un timer separado.
		if (gameState.getState() == GameState::QuestionAnswered ){
			if(questionAnsweredTimer.isRunning() ){ // si el timer está parado, ya se ha cumplido el tiempo
				int questionElapsed = questionAnsweredTimer.tick();
#ifdef DEBUG_OUTPUT
				cout << "Question answered " << ANSWERED_STAY_TIME << " => " << questionElapsed << endl;
#endif
				if (questionElapsed > ANSWERED_STAY_TIME) {
					questionAnsweredTimer.stop(); // si no reseteamos el timer aqui entrará en loop ya que timerElapsed siempre será mayor que QUESTION_MAX_TIME, fadeOut() reiniciará el timer y vuelta a empezar
					fadeOut();
				}
			}
		}
	} else {
		timerElapsed = 0;
	}
	/**
	 * Hay pantallas que tienen textos (puntuacion y tiempo) en las cajas de texto.
	 * Otras no lo tiene.
	 * Se define con la propiedad bool haveText de la pantalla.
	 * En caso de tenerlo, aquí lo pintamos. Si no, se obvia.
	 */
	if( currentSheet->haveText ){
		std::ostringstream pointsStream;
		pointsStream << points;

		fboPoints.begin();
		ofClear(255);
		ofSetColor(0);
		ofDrawRectangle(0,0,fboPoints.getWidth(),fboPoints.getHeight());
		ofSetColor(255);

		fontPoints.drawString(pointsStream.str(), FONT_POINTS_X, FONT_POINTS_Y);
		fboPoints.end();

		fboTimer.begin();
		ofClear(255);
		ofSetColor(0);
		ofDrawRectangle(0, 0, fboTimer.getWidth(), fboTimer.getHeight());
		ofSetColor(255);
		fontTimer.drawString(timerFormatTime(timerElapsed) , FONT_TIMER_X, FONT_TIMER_Y);
		fboTimer.end();
	} else { // esta pantalla no tiene texto
		fboPoints.begin();
		ofClear(255);
		ofSetColor(0);
		ofDrawRectangle(0, 0, fboPoints.getWidth(), fboPoints.getHeight());
		fboPoints.end();

		fboTimer.begin();
		ofClear(255);
		ofSetColor(0);
		ofDrawRectangle(0, 0, fboTimer.getWidth(), fboTimer.getHeight());
		fboTimer.end();
	}

	uint64_t currentTime = ofGetElapsedTimeMillis();
	// aqui manejaremos los estados y transiciones
	if ( isFading() ){ 
#ifdef DEBUG_OUTPUT
		cout << "Is fading : in(" << isFadingIn <<") out(" << isFadingOut << ")" <<  endl;
#endif // DEBUG_OUTPUT
		int64_t remainingTime = fadeEndTime - currentTime;
		int64_t elapsedTime = currentTime - fadeStartTime;
#ifdef DEBUG_OUTPUT
		cout << " * End time " << fadeEndTime << " current time " << currentTime << " elapsed time " << elapsedTime << " remaining time " << remainingTime  << endl;
#endif // DEBUG_OUTPUT
		if (remainingTime <= 0) { // termina el fade
#ifdef DEBUG_OUTPUT
			cout << " * Fade end " << endl;
#endif // DEBUG_OUTPUT
			if (isFadingIn) { // si estamos apareciendo simplemente parar
				isFadingIn = false;
				// hay un caso especial que es la pantalla imprimiendo que es bloqueante.
				// en este caso, paramos al final del fadeIn, imprimimos y hacemos fadeOut
				// durante la impresión bloqueará
				if (gameState.getState() == GameState::State::PrintTicket) {
					printTicket();
					fadeOut();
					return;
				}
			}
			if (isFadingOut) { // si estamos desapareciendo, cambia la pantalla a una nueva, y transición
				isFadingOut = false;
				stateChanged = true;
				gameState.nextState();
				fadeIn();
			}
#ifdef DEBUG_OUTPUT
			cout << " * Fading out  " << isFadingOut  << " fading in " << isFadingIn << endl;
#endif // DEBUG_OUTPUT
		}
		else { // aun está haciendo fade, calcula el valor de alpha
			float f, p;
			if (isFadingOut) {
				f = (float)remainingTime/(float)fadeTotalTime;				
			}
			else {
				f = (float)elapsedTime/(float)fadeTotalTime;
			}
			p = 255 * f;
			currentAlpha = nearbyintf(p);
#ifdef DEBUG_OUTPUT
			cout << " ** Calculate alpha f: " << f << " p: " << p << " alpha : " << currentAlpha << endl;
#endif // DEBUG_OUTPUT
		}
#ifdef DEBUG_OUTPUT
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); // para ralentizar en debug
#endif // DEBUG_OUTPUT
	}

	// read sensors pcb
	readSensors();

}
/**
 * Da formato al tiempo transcurrido
 */
string ofApp::timerFormatTime(int seconds) {
	ostringstream stringStream;
	int mins = (GAME_MAX_TIME - seconds) / 60;
	int secs = (GAME_MAX_TIME - seconds) % 60;
	stringStream << mins << ":" << setfill('0') << setw(2) << secs;
	return stringStream.str();
}
/**
 * Devuelve si la pantalla está en una transición 
 */
bool ofApp::isFading() {
	if ( timedOut ) {
		timedOut = false;
		return true;
	}
	return isFadingOut || isFadingIn;
}
//--------------------------------------------------------------
void ofApp::draw(){
	/**
	 * Si se está en una transición se usa currentAlpha calculada en update().
	 * Si no se está en una pantalla con lo que se quita la transición.
	 */
	if (isFading()) {
		ofSetColor(255, 255, 255, currentAlpha);
	}
	else {
		ofSetColor(255, 255, 255, 255);
	}
	// ha cambiado el estado. Definir el nuevo estado.
	if (stateChanged) { 		
#ifdef DEBUG
		cout << "* Estate changed " << gameState.getState() << endl;
#endif // DEBUG
		if (currentSheet->sound.isPlaying() ){
			currentSheet->sound.stop();
		}
		stateChanged = false;
		GameState::State state = gameState.getState();
		switch (state) {
			case GameState::State::Start:
				currentSheet = startSheet;
				break;
			case GameState::State::ReadyToPlay:
				currentSheet = readyToPlaySheet;
				shuffleSheets();
				gameTimer.reset();
				break;
			case GameState::State::Question:
				lastSheet = currentSheet;
				currentSheet = sheetsData[gameState.getCurrentQuestion()];
				break;
			case GameState::State::QuestionAnswered:
				questionAnsweredTimer.start();
				lastSheet = currentSheet;
				if (lastQuestionSuccess == true) {
					currentSheet = successStaticSheet;
				} else {
					currentSheet = failStaticSheet;
				}			
				break;
			case GameState::State::End:
				currentSheet = endStaticSheet;
				break;
			case GameState::State::PrintTicket:
				currentSheet = printStaticSheet;			
				break;
		}
#ifdef DEBUG
		//debugDump(currentSheet);
#endif // DEBUG
		if( ! gameTimer.isRunning() ){
			gameTimer.start();
		}
	}		
	if (!currentSheet->sound.isPlaying() && soundEnabled ) {
		currentSheet->sound.setLoop(currentSheet->isSoundLoop);
		currentSheet->sound.play();
	}
	drawCurrentSheet();
}
/**
 * Baraja las fichas para que no están en el mismo orden siempre
 */
void ofApp::shuffleSheets()
{
	auto rng = std::default_random_engine{};
	std::shuffle(std::begin(sheetsData), std::end(sheetsData), rng);
}
/**
 * Funcion de volcado de datos a pantalla
 */
void ofApp::debugDump(SheetData *data)
{
	/*cout << "Dump: " << endl;
	cout << "\t imgCajaOnePath: " << data->imgCajaOnePath << endl;
	cout << "\t imgCajaOne Loaded: " << data->imgQuestion.isAllocated() << endl;

	cout << "\t imgCajaTwoPath: " << data->imgCajaTwoPath << endl;
	cout << "\t imgCajaTwo isAllocated: " << data->imgQuestionOk.isAllocated() << endl;

	cout << "\t imgCajaThreePath: " << data->imgCajaThreePath << endl;
	cout << "\t imgCajaThree isAllocated: " << data->imgCajaThree.isAllocated() << endl;

	cout << "\t videoOnePath: " << data->videoOnePath << endl;
	cout << "\t videoOne isInitialized: " << data->videoOne.isInitialized() << endl;

	cout << "\t videoTwoPath: " << data->videoTwoPath << endl;
	cout << "\t videoTwo isInitialized: " << data->videoTwo.isInitialized() << endl;

	cout << "\t videoThreePath: " << data->videoThreePath << endl;
	cout << "\t videoThree isInitialized: " << data->videoThree.isInitialized() << endl;

	cout << "\t imgCajaOneOnePath: " << data->imgButtonOnePath << endl;
	cout << "\t imgCajaOneOne isAllocated: " << data->imgButtonOne.isAllocated() << endl;

	cout << "\t imgCajaOneTwoPath: " << data->imgButtonTwoPath << endl;
	cout << "\t imgCajaOneTwo isAllocated: " << data->imgButtonTwo.isAllocated() << endl;

	cout << "\t imgCajaOneThreePath: " << data->imgButtonThreePath << endl;
	cout << "\t imgCajaOneThree isAllocated: " << data->imgButtonThree.isAllocated() << endl;

	cout << "\t responseNumber: " << data->soundPath << endl;

	cout << "\t soundPath: " << data->soundPath << endl;
	cout << "\t sound: " << data->sound.isLoaded() << endl;*/
}

/**
 * Estas fichas son estáticas
 *   - Start
 *   - ReadyToPlay
 *   - Success
 *   - Fail
 *   - End 
 */
// Dibuja la pantalla Start
void ofApp::drawCurrentSheet() {
	if (currentSheet->isSolution()) {
		warperBoxQuestions.begin();
		warperBoxQuestions.draw();
		lastSheet->imgQuestionOk.draw(0, 0);
		warperBoxQuestions.end();
	} else {
		warperBoxQuestions.begin();
		warperBoxQuestions.draw();
		currentSheet->imgQuestion.draw(0, 0);
		warperBoxQuestions.end();
	}

	warperBoxScore.begin();
	warperBoxScore.draw();
	imgBoxScore.draw(0, 0);
	fboPoints.draw(0,0);
	warperBoxScore.end();

	warperBoxTimer.begin();
	warperBoxTimer.draw();
	imgBoxTimer.draw(0, 0);
	fboTimer.draw(0,0);
	warperBoxTimer.end();

	warperBoxButtonOne.begin();
	warperBoxButtonOne.draw();
	currentSheet->imgButtonOne.draw(0, 0);
	warperBoxButtonOne.end();

	warperBoxButtonTwo.begin();
	warperBoxButtonTwo.draw();
	currentSheet->imgButtonTwo.draw(0, 0);
	warperBoxButtonTwo.end();

	warperBoxButtonThree.begin();
	warperBoxButtonThree.draw();
	currentSheet->imgButtonThree.draw(0, 0);
	warperBoxButtonThree.end();
}

/**
* Callback del evento de final de lectura del XML.
* Inicia la trtansición a otro estado con fadeOut()
*/
void ofApp::onKeyTouchEvent(int & value)
{
#ifdef DEBUG
	cout << "Touched key " << value << endl;
#endif // DEBUG
	/* Se gestiona un click durante una pregunta. Se comprueba si la respuesta es correcta */
	if( gameState.getState() == GameState::State::Question ){
		int question = gameState.getCurrentQuestion();
		if (currentSheet->responseNumber == value) {
			lastQuestionSuccess = true;		
			points += 1;
		} else {
			lastQuestionSuccess = false;
		}
#ifdef DEBUG
		cout << "Question answered " << question << " expected: " << currentSheet->responseNumber << "value " << value << " result: " << lastQuestionSuccess << " points: " << points << endl;
#endif // DEBUG
 	}
	/* Se gestiona un click al iniciar el juego. Inicializa los puntos a cero */
	if (gameState.getState() == GameState::State::Start) {
		points = 0;
	}

	fadeOut();
}

/*
 * Eventos normales e teclado.
 * 
 * Para guardar/cargar warpers (la carga es automática), etc ...
 */
void ofApp::keyPressed(int key){
	if (isFading()) return; // no hacer caso a las teclas en transiciones
	ofXml XML;
	int value;
	switch (key) {
	case 's':
		saveWarpersToXml();
		break;
	case 'l':
		loadWarpersFromXml();
		break;
	case '1':
		deactivateWarpers();
		warperBoxQuestions.activate();
		break;
	case '2':
		deactivateWarpers();
		warperBoxScore.activate();
		break;
	case '3':
		deactivateWarpers();
		warperBoxTimer.activate();
		break;
	case '4':
		deactivateWarpers();
		warperBoxButtonOne.activate();
		break;
	case '5':
		deactivateWarpers();
		warperBoxButtonTwo.activate();
		break;
	case '6':
		deactivateWarpers();
		warperBoxButtonThree.activate();
		break;
	case '0':
		deactivateWarpers();
		break;
	case 'x':
		ofExit();
		break;
	case 'f':
		ofToggleFullscreen();
		break;

	case 'b':
		// emulan los pulsadores
		value = 1;
		ofNotifyEvent(keyTouchEvent, value);
		break;
	case 'n':
		// emulan los pulsadores
		value = 2;
		ofNotifyEvent(keyTouchEvent, value);
		break;
	case 'm':
		// emulan los pulsadores
		value = 3;
		ofNotifyEvent(keyTouchEvent, value);
		break;
	}
}


/**
 * lee la pcb de los sensores
 */
void ofApp::readSensors() {
	// if we've got new bytes
	if (pcb.available() > 0)
	{
		// we will keep reading until nothing is left
		while (pcb.available() > 0)
		{
			// we'll put the incoming bytes into bytesReturned
			pcb.readBytes(bytesReturned, 1);

			// if we find the splitter we put all the buffered messages
			//   in the final message, stop listening for more data and
			//   notify a possible listener
			// else we just keep filling the buffer with incoming bytes.
			if (*bytesReturned == '\n')
			{
				message = messageBuffer;
				messageBuffer = "";
				//ofRemoveListener(ofEvents().update, this, &ofxSimpleSerial::update);
				//ofNotifyEvent(NEW_MESSAGE, message, this);

				int value = ofToInt(message);
				ofNotifyEvent(keyTouchEvent, value);

				break;
			}
			else
			{
				if (*bytesReturned != '\r')
					messageBuffer += *bytesReturned;
			}
			//cout << "  messageBuffer: " << messageBuffer << "\n";
		}

		// clear the message buffer
		memset(bytesReturned, 0, 1);
	}
}


/**
 * Desactiva todos los warpers
 */
void ofApp::deactivateWarpers() {
	warperBoxQuestions.deactivate();
	warperBoxScore.deactivate();
	warperBoxTimer.deactivate();
	warperBoxButtonOne.deactivate();
	warperBoxButtonTwo.deactivate();
	warperBoxButtonThree.deactivate();
}
/*
 * Guarda las posiciones de los warpers al XML
 */
void ofApp::saveWarpersToXml()
{
	ofXml XML;

	warperBoxQuestions.saveToXml(XML, "warperCaja1");
	warperBoxScore.saveToXml(XML, "warperCaja2");
	warperBoxTimer.saveToXml(XML, "warperCaja3");
	warperBoxButtonOne.saveToXml(XML, "warperCaja11");
	warperBoxButtonTwo.saveToXml(XML, "warperCaja12");
	warperBoxButtonThree.saveToXml(XML, "warperCaja13");

	XML.save("warpConfig.xml");
}

/**
 * Carga las posiciones de los warpers del XML
 */
void ofApp::loadWarpersFromXml()
{
	ofXml XML;
	XML.load("warpConfig.xml");

	warperBoxQuestions.loadFromXml(XML, "warperCaja1");
	warperBoxScore.loadFromXml(XML, "warperCaja2");
	warperBoxTimer.loadFromXml(XML, "warperCaja3");
	warperBoxButtonOne.loadFromXml(XML, "warperCaja11");
	warperBoxButtonTwo.loadFromXml(XML, "warperCaja12");
	warperBoxButtonThree.loadFromXml(XML, "warperCaja13");
}

/**
 * Inicia un fadeIn, que marca el inicio de un nuevo estado
 */
void  ofApp::fadeIn() {
	fadeStartTime = ofGetElapsedTimeMillis();
	fadeEndTime = fadeStartTime + fadeTotalTime;
	isFadingOut = false;
	isFadingIn = true;
}
/*
 * Reset timer.
 * Resetea el timer del fade 
 */
void  ofApp::resetFade() {
	fadeStartTime = ofGetElapsedTimeMillis();
}
/**
 * Inicia un fade out que marca el final de un estado.
 */
void  ofApp::fadeOut() {
	fadeStartTime = ofGetElapsedTimeMillis();
	fadeEndTime = fadeStartTime + fadeTotalTime;
	isFadingOut = true;
	isFadingIn = false;
}
/**
 * App exit() - esto podría obviarse
 */
void ofApp::exit() {
	delete startSheet;
	delete readyToPlaySheet;
	delete endStaticSheet;
	delete successStaticSheet;
	delete failStaticSheet;
	delete printStaticSheet;
}
/**
 * Hay que definir aquí el imprimir el ticket
 */
void ofApp::printTicket()
{
#ifdef TEST_PRINT_SCREEN
	std::this_thread::sleep_for(std::chrono::seconds(6));
	return;
# else
	container.begin();
	ofSetColor(255);
	bg.draw(0, 0);
	ofSetColor(100);
	genial.drawString("!HA ESTADO GENIAL!", 320, 50);
	
	if (points < 10) {
		numAciertos.drawString("0"+ofToString(points), 310, 260);
	}else {
		numAciertos.drawString(ofToString(points), 310, 260);
	}

	aciertos.drawString("ACIERTOS", 330, 320);
	//aciertos.drawString(timerFormatTime(timerElapsed), 330, 360);

	info.drawString("POR FAVOR PRESENTA ESTE TICKET", 272, 500);
	info.drawString("EN EL MOSTRADOR DE LA PLANTA BAJA", 231, 520);
	info.drawString("SI NO RECIBES NUESTRO BOLETIN", 288, 540);
	info.drawString("DE ACTIVIDADES, TE DAREMOS DE ALTA", 234, 560);
	info.drawString("Y TE LLEVAS UN BONITO DETALLE", 283, 580);

	genial.drawString("MUCHAS GRACIAS", 358, 620);

	container.end();

	ofPixels pixels;
	container.readToPixels(pixels);
	printer.flush();

	printer.printImage(pixels, OF_ALIGN_HORZ_CENTER);
	printer.flush();

	//.printImage(pixels, OF_ALIGN_HORZ_RIGHT);
	printer.flush();
	printer.flush();
	printer.flush();
	printer.flush();
	printer.flush();

	printer.cut(ESCPOS::BaseCodes::CUT_FULL);
#endif
 }
/**
 * Indica si el juego está en estado inicado (preguntas y respuestas)
 */
bool ofApp::gameBeingPlayed()
{
	GameState::State state = gameState.getState();	
	return ( state == GameState::Question || state == GameState::QuestionAnswered );
}
/**
 * Resetea las variables del juego
 */
void ofApp::resetGame()
{
	gameTimer.reset();
	points = 0;
	timerElapsed = 0;
	gameState.reset();
}
/**
 * Lee el xml en el main thread
 */
void ofApp::readXml(string xmlFileName)
{
	ofxXmlSettings xmlFile;

	xmlFile.load(xmlFileName);
	xmlFile.pushTag("fichas", 0);

	int numFichas = xmlFile.getNumTags("ficha");

	for (int i = 0; i < numFichas; i++) {
		xmlFile.pushTag("ficha", i);

		string carpeta = xmlFile.getValue("carpeta", "");
		int respuesta = xmlFile.getValue("respuesta", -1);

#ifdef DEBUG
		cout << "XML carpeta: " << carpeta << endl;
		cout << "  - respuesta correcta" << respuesta << endl;
#endif // DEBUG

		SheetData *data = new SheetData(carpeta);
		data->responseNumber = respuesta;
		data->haveText = true;
		data->isSoundLoop = true;
		data->load();

		sheetsData.push_back(data);
		xmlFile.popTag();
	}
}
/* destructor por defecto */
ofApp::~ofApp(){}