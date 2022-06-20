#pragma once

class SuperCellTemplate {
public:
	int id = -1;
	int type = -1;
	int volume = 0;

	int specialType = 0;

	SuperCellTemplate();
	SuperCellTemplate(int id);

	static int addTemplate(SuperCellTemplate T);
	static SuperCellTemplate &getTemplate(int i);
};