#ifndef _Map
#define _Map

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "Road.h"
#include "Intersection.h"
#include "RoadMapInterface.h"
#include "Split.h"

const int inf = 1000;
const int no_Intersections = 100;  //remove and make dynamic

template<class V>
class Map : public RoadMapInterface<V>
{
	std::vector<Intersection<V>*> adjListV;
	int adj_matrix[no_Intersections][no_Intersections];
public:
	//Constructor (populates Map)
	Map(std::string&);
	void calculateSP(const V&);
	int findSmallestUnvisitedIntersection(bool[], int[]);
	void printAdjList() const;
	void printAdjMatrix() const;
	//complete functions from the interface
	//void updateRoad(std::string&,int,int,int) = 0;
    //void updateIntersection(V,double) = 0; //add update vector of adj intersections
};

template<class V>
Map<V>::Map(std::string &f) {
	V s = 0;
	adjListV.push_back(new Intersection<V>(s));
	
	//read <city>.dat file and load road information
	//https://www.new-york-city-map.com/manhattan.htm
	std::ifstream dataFile (f);
	std::string line;
	std::getline (dataFile, line); //ignore header
	if ( dataFile.is_open() ) {
		while ( dataFile ) {
			std::getline (dataFile, line);

			bool IntersectionExists = false;
			std::string name = split(line,',',1,true);
			V src = split(line,',',2);
			V dest = split(line,',',3);
			double congestion = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			std::cout << std::fixed << "\nadding: " << src << "----(" << congestion << ")----" << dest << std::endl;
			
			if (! adjListV.empty()) {
				for (Intersection<V> *Intersection : adjListV) {
					if (src == Intersection->getIntersectionValue()) {
						IntersectionExists = true;
						break;
					}
				}
			}

			std::cout << src << (IntersectionExists ? " exists" : " does not exist") << std::endl;

			if (IntersectionExists) {
				std::cout << "updating...";
				//Intersection at the end of list connected to vector |  Intersection  |->... |  Intersection  |->nullptr
				Intersection<V> *curr = adjListV.at(src);
				while (curr->getNextIntersection() != nullptr)
					curr = curr->getNextIntersection();

				curr->setNextIntersection(new Intersection<V>(dest, congestion));
				std::cout << "--> "<< dest << " added." << std::endl;
			}
			else {
				std::cout << "adding...";
				//add Intersection to vector |  Intersection  |->nullptr
				adjListV.push_back(new Intersection<V>(src));
				
				//add associated destination to the Intersection |  Intersection  |->|  Intersection  |->nullptr
				Intersection<V> *curr = adjListV.at(src);
				curr->setNextIntersection(new Intersection<V>(dest, congestion));
				std::cout << "new Intersection and destination added." << std::endl;
			}

		}

	}
	else{
		std::cout << "\n file not found.";
	}
}

template<class V>
void Map<V>::calculateSP(const V& u) {

	bool visited[no_Intersections];
	int distance[no_Intersections];

	//init adj_matrix, adj = 0, all others at inf
	for (int i = 0; i < no_Intersections; i++)
		for (int j = 0; j < no_Intersections; j++)
			if (i == j) adj_matrix[i][j] = 0;
			else adj_matrix[i][j] = inf;

	//populate adj_matrix from adjListV
	if (!adjListV.empty()) {
		for (Intersection<V> *Intersec : adjListV) {
			Intersection<V> *curr = Intersec;

			while(curr->getNextIntersection() != nullptr) {
				adj_matrix[Intersec->getIntersectionValue()][curr->getNextIntersection()->getIntersectionValue()] = curr->getNextIntersection()->getIntersectionCongestion();
				curr = curr->getNextIntersection();
			}
		}
	}

	printAdjMatrix();

	// calculate distance
	//u's distance to itself = 0
	for (int k = 0; k < no_Intersections; k++) {
		visited[k] = false;
		distance[k] = inf;
	}
	distance[u] = 0;

	for (int count = 0; count < no_Intersections; count++){
		int v = findSmallestUnvisitedIntersection(visited, distance);		//v is to be added next
		visited[v] = true;											//add v to visited Intersections
		for (int i = 0; i < no_Intersections; i++){
			/*Update dist[v] if not in Dset and their is a path from src to v through u that has distance minimum than current value of dist[v]*/
		
			if (!visited[i] && adj_matrix[v][i] != inf && distance[v] != inf)
				if(distance[v] + adj_matrix[v][i] < distance[i])
					distance[i] = distance[v] + adj_matrix[v][i];
		}
	}

	std::cout << "Distances:" << std::endl;
	for (int i = 0; i < no_Intersections; i++) {
		if (distance[i] == inf) std::cout << u << "--->" << i << " with distance " << static_cast<unsigned char>(236) << std::endl;
		else std::cout << u << "--->" << i << " with distance " << distance[i] << std::endl;
	}

}

template<class V>
int Map<V>::findSmallestUnvisitedIntersection(bool visited[], int distance[]) {
		int min = inf, smallest_weight_Intersection;
		for (int i = 0; i < no_Intersections; i++){
			if (!visited[i] && distance[i] <= min){
				min = distance[i];
				smallest_weight_Intersection = i;
			}
		}
		return smallest_weight_Intersection;
}


// print adjacency list representation of Map
template<class V>
void Map<V>::printAdjList() const {
	std::cout << std::endl << "Adjacency list..." << std::endl;
	for (Intersection<V> *Intersec : adjListV) {
		// print all neighboring vertices of given vertex
		Intersection<V> *curr = Intersec;
		std::cout << curr->getIntersectionValue(); //print Map Intersection
		curr = curr->getNextIntersection();
		while (curr != nullptr){
			std::cout << " --> [" << curr->getIntersectionValue() << ",w(" << curr->getIntersectionCongestion() << ")]"; //print adjacent Intersections to Map Intersection
			curr = curr->getNextIntersection(); //move to next adjacent Intersection
		}
		std::cout << std::endl;
	}
}

// print adjacency list representation of Map
template<class V>
void Map<V>::printAdjMatrix() const {
	std::cout << std::endl << "Adjacency matrix..." << std::endl;
	std::cout << "\t";
	for (int i = 0; i < no_Intersections; i++) {
		std::cout << i << "\t";
	}
	std::cout << std::endl;
	for (int i = 0; i < no_Intersections; i++) {
		std::cout << i << "\t";
		for (int j = 0; j < no_Intersections; j++) {
			if (adj_matrix[i][j] == inf) std::cout << static_cast<unsigned char>(236) << "\t";
			else std::cout << adj_matrix[i][j] << "\t";
		}
		std::cout << std::endl;
	}
		
}

#endif