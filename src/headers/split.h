#pragma once
#include<vector>
#include<string>
#include<sstream>

std::vector<std::string> split(std::string& in, char delim) {
	
	std::stringstream str(in);
	std::string S;
	std::vector<std::string> V;

	while(std::getline(str, S, delim)) {

		V.push_back(S);

	}

	return V;

}