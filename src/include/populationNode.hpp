/*
 * populationNode.hpp
 *
 *  Created on: Mar 16, 2012
 *      Author: wjiang2
 */

#ifndef POPULATIONNODE_HPP_
#define POPULATIONNODE_HPP_

#include <map>
#include <string>
#include <vector>
#include "gate.hpp"
using namespace std;

typedef map<string,double> POPSTATS;

class populationNode{
public:
	string thisName;
	gate * thisGate;
	POPINDICES *thisIndice;

	POPSTATS fjStats,fcStats;
public:
	populationNode(){thisGate=NULL;};
//	~populationNode(){delete thisGate;};//since gate is dynamically created,needs to be freed here in destroy method
//	valarray<bool> * getIndice(){return(this->thisIndice);};

	POPSTATS getStats(bool isFlowCore=false){
		return(isFlowCore?this->fcStats:this->fjStats);
		};

	gate * getGate(){return(this->thisGate);};
	string getName(){return(this->thisName);};
	void setName(const char * popName){
		thisName=popName;
	};
	void setGate(gate *gate){thisGate=gate;};
//	void setIndice(vector<bool> indice){thisIndice=indice;};


};


#endif /* POPULATIONNODE_HPP_ */
