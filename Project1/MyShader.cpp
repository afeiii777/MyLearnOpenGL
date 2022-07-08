#include "MyShader.h"

#include<iostream>
#include<fstream>
#include<sstream>

using namespace std;

MyShader::MyShader(const char* vertexPath, const char* fragmentPath) {

	ifstream vertexFile;
	ifstream fragmentFile;
	stringstream vertextSStream;
	stringstream fragmentSSteram;

	vertexFile.open(vertexPath);
	fragmentFile.open(fragmentPath);

	try
	{
		if (!vertexFile.is_open() || ! fragmentFile.is_open()) {
			throw exception("open file failed");
		}

		vertextSStream << vertexFile.rdbuf();
		fragmentSSteram << fragmentFile.rdbuf();

		vertexString = vertextSStream.str();
		fragmentString = fragmentSSteram.str();

		vertexSource = vertexString.c_str();
		fragmentSource = fragmentString.c_str();

		printf(vertexSource);
		printf(fragmentSource);
	}
	catch (const exception& ex)
	{
		printf(ex.what());
	}

}
