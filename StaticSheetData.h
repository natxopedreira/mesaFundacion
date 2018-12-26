#pragma once
#include "SheetData.h"

/**
 * Objeto con los datos de la pantalla de inicio
 */
class StaticSheetData : public SheetData
{
public:
	StaticSheetData(string folder)
	{
		type = Type::Static;
		setFolder(folder);
		responseNumber = 0;
		is_solution = false;
		load();
	};

	~StaticSheetData() {};
};
