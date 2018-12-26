#include "SheetData.h"

/**
 * Constructor por defecto
 */
SheetData::SheetData() {
	type = Type::Game;
}
/**
 * Constructor estandar para fuchas del juego.
 * Usa los nombres por convención de los archivos.
 */
SheetData::SheetData(string folder){
	type = Type::Game;
	setFolder(folder);
}
/**
 * Constructor para fichas
 */
SheetData::SheetData(string folder, Type sheetType) {
	type = sheetType;
	setFolder(folder);
}
/**
 * Setter para ajustar si esta ficha es la de solución (ok, ko)
 * que usa la imagenOk de la ficha anterior
 */
void SheetData::setIsSolution()
{
	type = Type::Solution;
}
/**
* Getter para saber su esta ficha es de solución y debe usar la
* imagenOk de la ficha anterior
*/
bool SheetData::isSolution() {
	return type == Type::Solution;
}

/**
 * Inicializa miembros de la clase a partir del 
 * nombre de carpeta, usando los normbres por convencion
 * de archivo.
 */
void SheetData::setFolder(string folder){
	dataFolder = folder;
	soundPath = formatPath(folder, "sound.mp3");
	pathImgQuestion = formatPath(folder, "image.jpg");

	if (type == Type::Game) {
		pathImgQuestionOk = formatPath(folder, "ok.jpg");
	}

	pathImgButtonOne = formatPath(folder, "b1.jpg");
	pathImgButtonTwo = formatPath(folder, "b2.jpg");
	pathImgButtonThree = formatPath(folder, "b3.jpg");

	isSoundLoop = true;
}

std::string SheetData::formatPath(string folder, string name){
	std::ostringstream stream;
	stream << folder << "/" << name;
	return stream.str();
}

void SheetData::load() {
	if( !isLoaded() ){
		if(type != Type::Solution ){
			imgQuestion.load(pathImgQuestion);
		}
		if (type == Type::Game) {
			imgQuestionOk.load(pathImgQuestionOk);
		}
		imgButtonOne.load(pathImgButtonOne);
		imgButtonTwo.load(pathImgButtonTwo);
		imgButtonThree.load(pathImgButtonThree);

		sound.setLoop(isSoundLoop);
		sound.load(soundPath);
	}
}



//SheetData & SheetData::operator=(const SheetData & other)
//{
//	if (this != &other) { 	
//		type = other.type;
//		dataFolder = other.dataFolder;
//		soundPath = other.soundPath;
//
//		pathImgQuestionOk = other.pathImgQuestionOk;
//		pathImgQuestion = other.pathImgQuestion;
//
//		pathImgButtonOne = other.pathImgButtonOne;
//		pathImgButtonTwo = other.pathImgButtonTwo;
//		pathImgButtonThree = other.pathImgButtonThree;
//
//		haveText = other.haveText;
//		isSoundLoop = other.isSoundLoop;
//		responseNumber = other.responseNumber;
//
//		load();
//	}
//	return *this;
//}


/**
 * Construye el objeto con los datos de la configuración por defecto.
 * 
 */
bool SheetData::isLoaded() {
	if (type == Type::Game) {
		return (
			imgQuestion.isAllocated() &&
			imgQuestionOk.isAllocated() &&
			imgButtonOne.isAllocated() &&
			imgButtonTwo.isAllocated() &&
			imgButtonThree.isAllocated() &&
			sound.isLoaded()
		);
	}

	return (
		imgQuestion.isAllocated() &&
		imgButtonOne.isAllocated() &&
		imgButtonTwo.isAllocated() &&
		imgButtonThree.isAllocated() &&
		sound.isLoaded()
	);
}


