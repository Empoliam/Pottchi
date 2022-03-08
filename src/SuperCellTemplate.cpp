#include "headers/SuperCellTemplate.h"
#include <map>

static std::map<int, SuperCellTemplate> scTemplates;

SuperCellTemplate::SuperCellTemplate() {}

SuperCellTemplate::SuperCellTemplate(int id) {
	this->id = id;
}

int SuperCellTemplate::addTemplate(SuperCellTemplate T) {

    if(scTemplates.count(T.id) == 1) return -1;

    scTemplates.insert({T.id,T});

	return 0;
}

SuperCellTemplate &SuperCellTemplate::getTemplate(int i) {

    return scTemplates[i];

}
