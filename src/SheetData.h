#pragma once
#include "ofMain.h"



class SheetData
{
public:
	enum Type {
		Game,				// Ficha de pregunta/respuesta
		Static,				// Ficha de interface
		Solution			// Ficha de solucion
	};
	
	Type type;

	string dataFolder;
	string soundPath;

	string pathImgQuestion;
	string pathImgQuestionOk;

	string pathImgButtonOne;
	string pathImgButtonTwo;
	string pathImgButtonThree;


	ofImage imgQuestion;
	ofImage imgQuestionOk;

	ofImage imgButtonOne;
	ofImage imgButtonTwo;
	ofImage imgButtonThree;

	ofSoundPlayer sound;

	int responseNumber;
	bool haveText;
	bool isSoundLoop;

public:
	SheetData(string folder, Type sheetType);
	SheetData(string folder);
	SheetData();

	bool isLoaded();
	void load();
	void setIsSolution(void);
	bool isSolution(void);

private:
	std::string formatPath(string folder, string name);

protected:
	void setFolder(string folder);
	bool is_solution;
};

