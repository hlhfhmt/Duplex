#include <iostream>
#include "sensitivity.h"

Sensitivity::Sensitivity(int iS, int oS, double threshold){
	relativeSensitivityThreshold = threshold;
	totalQueries = 0;
	inputSize = iS; 
	outputSize = oS;
	sensitivityMatrix = new double*[inputSize];
	for (int i = 0; i < inputSize; i++){
		sensitivityMatrix[i] = new double[outputSize];
	}
	for (int i = 0; i < inputSize; i++){
		for (int j = 0; j < outputSize; j++){
			sensitivityMatrix[i][j] = 0.0;
		}
	}
	cout << "Constructing sensitivity matrix " << inputSize << " " << outputSize << endl;
}

Sensitivity::~Sensitivity(){
	for (int i = 0; i < inputSize; i++){
		delete sensitivityMatrix[i];
	}
	delete sensitivityMatrix;
}

void Sensitivity::setInputOutputMinMax(double* iMin, double* iMax, double* oMin, double* oMax){
	inputMin = iMin;
	inputMax = iMax;
	outputMin = oMin;
	outputMax = oMax;
}

void Sensitivity::pushBackInputChange(int input, double value, double delta){
	inputStack.push_back(input);
	inputDeltaStack.push_back(delta);
	inputValueStack.push_back(value);
}

void Sensitivity::pushBackOutputChange(int input, double value, double delta){

	if (delta/abs(value) > relativeSensitivityThreshold){
		outputStack.push_back(input);
		outputDeltaStack.push_back(delta);
		outputValueStack.push_back(value);
	}
}

void Sensitivity::commit(){
	totalQueries++;
	for (int i = 0; i < inputStack.size(); i++){
		for (int j = 0; j < outputStack.size(); j++){
			sensitivityMatrix[inputStack[i]][outputStack[j]] += abs(outputDeltaStack[j]);
		}
	}
	inputStack.clear();
	inputValueStack.clear();
	inputDeltaStack.clear();
	outputStack.clear();
	outputDeltaStack.clear();
	outputValueStack.clear();
}

void Sensitivity::generateSensitivityMatrix(){
	//Normalize sensitivity matrix
	//find max
	double max = -1;
	for (int i = 0; i < inputSize; i++){
		for (int j = 0; j < outputSize; j++){
			if (sensitivityMatrix[i][j]>max)
				max = sensitivityMatrix[i][j];
		}
	}
	//normalize
	for (int i = 0; i < inputSize; i++){
		for (int j = 0; j < outputSize; j++){
			sensitivityMatrix[i][j] = sensitivityMatrix[i][j]/max;
		}
	}
}

void Sensitivity::save(boost::property_tree::ptree* ptree){
	stringstream ss;
	for (int i = 0; i < inputSize; i++){
		for (int j = 0; j < outputSize; j++){
			ss << sensitivityMatrix[i][j] << " ";
		}
	}
	ptree->add("matrix", ss.str());
}
