#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <utility>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

using namespace std;

class Edge;
class Vertex{

public:
    string actorName;
    vector<Edge*> edges;
    int actorIndex;
    int degree;
    bool done;

    Vertex(string a){
        actorName=a;
        degree=0;
        done = false;
    }

    string getName(){
        return this->actorName;
    }

    bool operator==(const Vertex& v2){
        return this->getName() == v2.actorName;
    }
};

class Edge{
public:
    string movieName;
    vector <Vertex*> actors;

    Edge(string m){
        movieName = m;
    }

    string getName(){
        return this->movieName;
    }

};

class Graph{

public:

    unordered_map<string, Vertex*> actorList;
    unordered_map<string, Edge*> movieList;
    int actorIndex;

    Graph(void);

    ~Graph(){

        for(pair<string, Vertex*> v:actorList){
            delete v.second;
        }

        actorList.clear();
        
        for(pair<string, Edge*> e:movieList){
            delete e.second;
        }

        movieList.clear();
    }

    Vertex* insertActor(string actorName);
    Edge* insertMovie(string movie);
    void insertGraph(string actorName, string movieName, int movieYear);
    vector<int> findCoStarsMatrix(string actor, unordered_map<string, int>* adjacencyList);
    unordered_map<string, int> listOfActors();
    vector<string> vectorOfActors();
    bool loadFromFile(const char* in_filename);
    void print();
};

#endif
