#include "GameState.h"

GameState::GameState()
{
	currentState = State::None;
	nQuestions = 0;
}

void GameState::setNumberOfQuestions(int questionsNumber)
{
	nQuestions = questionsNumber;
}

GameState::State GameState::nextState()
{
	switch (currentState) {
		case State::None:
			currentState = State::Start;
			break;
		case State::Start:
			currentState = State::ReadyToPlay;
			break;
		case State::ReadyToPlay:
			currentState = State::Question;
			break;
		case State::Question:
			++curQuestion;
			currentState = State::QuestionAnswered;
			break;
		case State::QuestionAnswered:
			if (curQuestion == nQuestions) {
				currentState = State::End;
			}
			else {
				currentState = State::Question;
			}
			break;
		case State::End:
			currentState = State::Start;
			curQuestion = 0;
			break;
		default:
			cout << "Error estado desconocido" << endl;
			currentState = State::Start;
	}
	return currentState;
}


GameState::State GameState::getState(void)
{
	return currentState;
}

int GameState::getCurrentQuestion(void)
{
	return curQuestion;
}

void GameState::reset(void)
{
	currentState = State::None;
	curQuestion = 0;
}

void GameState::finishGame(void)
{
	/**
	 * El estado que vamos a generar es "Ultima pregunta contestada".
	 * De este modo nextState() devolverá State::End;
	 */
	currentState = State::QuestionAnswered;
	curQuestion = nQuestions;
}


