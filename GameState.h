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
	* Ajusta el n�mero de preguntas que tendr� el juego para que
	* se pueda decidir cual es la �ltima vez que la aplicaci�n
	* est� en el estado "Haciendo Pregunta".
	*/
	void setNumberOfQuestions(int questionsNumber);

	GameState::State currentState;
	/*
	* Pide a la m�quina de estados que decida cual
	* debe ser el pr�ximo estado del juego.
	*/
	GameState::State nextState(void);
	/**
	* Getter para el estado actual
	*/
	GameState::State getState(void);
	/**
	* Getter para el �ndice de la pregunta actual
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

