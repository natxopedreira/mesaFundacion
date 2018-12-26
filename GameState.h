#pragma once
#include "ofMain.h"

/*
 * Estado del juego
 */
class GameState
{
private:
	int nQuestions;
	int curQuestion;

public:
	/* estado posibles */
	enum State {
		None,				// No hay estado definido
		Start,				// pantalla de inicio
		ReadyToPlay,		// Pantalla con el mensaje explicativo
		Question,			// Pantalla preguntas
		QuestionAnswered,	// Pantalla de acierto/fallo
		End					// Pantalla Final 
	};
	/**
	* Constructor de la clase.
	*/
	GameState();
	/**
	* Ajusta el número de preguntas que tendrá el juego para que
	* se pueda decidir cual es la última vez que la aplicación
	* está en el estado "Haciendo Pregunta".
	*/
	void setNumberOfQuestions(int questionsNumber);

	GameState::State currentState;
	/*
	* Pide a la máquina de estados que decida cual
	* debe ser el próximo estado del juego.
	*/
	GameState::State nextState(void);
	/**
	* Getter para el estado actual
	*/
	GameState::State getState(void);
	/**
	* Getter para el índice de la pregunta actual
	*/
	int getCurrentQuestion(void);
	/**
	 * Resetea el estado (no se usa)
	 */
	void reset(void);
	/**
	 * Finaliza el juego por un temporizador, etc ..
	 */
	void finishGame(void);
};

