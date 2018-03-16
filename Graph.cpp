#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include "Graph.hpp"

using namespace std;

Graph::Graph(void){
    actorIndex=0;
};

Vertex* Graph::insertActor(string actorName){
    Vertex * insert = new Vertex (actorName);   
    actorList.insert(make_pair(actorName, insert));
    insert->actorIndex = this->actorIndex++;
    return insert;
}

Edge* Graph::insertMovie(string movie){
    Edge * insert = new Edge (movie);
    movieList.insert(make_pair(movie, insert));
    return insert;
}

void Graph::insertGraph(string actorName, string movieName, int movieYear){
    
    string movieNameAndYear = movieName+'\t'+to_string(movieYear);
    auto actor = this->actorList.find(actorName);
    auto film = this->movieList.find(movieNameAndYear);

    Vertex * currActor;
    Edge * currMovie;

    if(actor==actorList.end()){
        currActor = insertActor(actorName);
    }
    else{
        currActor = (*actor).second;
    }


    if(film==movieList.end()){
        currMovie = insertMovie(movieNameAndYear);
    }

    else{
        currMovie = (*film).second;
    }

    currActor->edges.push_back(currMovie);
    currMovie->actors.push_back(currActor); 
}


vector<int> Graph::findCoStarsMatrix(string actor, unordered_map<string, int> * adjacencyList){
       
    //Movies the actor has starred in
    //unordered_map<string, int> adjacencyCopy (*adjacencyList);
    vector <int> output(this->actorIndex, 0);
    vector<Edge*> filmList = (*actorList.find(actor)).second->edges;
    auto movie = filmList.begin();

    //Loop through every movie to find actors
    while(movie!= filmList.end()){
           
        //Find actor
        auto coStar = (*movie)->actors.begin();
            
        while(coStar!= (*movie)->actors.end() ){
            //Change value in adjacency map to 1
            //auto adjacency = adjacencyCopy.find((*coStar)->getName());
   
            auto adjacency = adjacencyList->find((*coStar)->getName());

            if((*coStar)->getName()!=actor)
                output[(*adjacency).second]=1;
                //(*adjacency).second =1;

            coStar++;
        }  

        movie++;
    }

    /*
    for(auto a: adjacencyCopy){
        output.push_back(a.second);
    }*/
    return output;
}

unordered_map<string, int> Graph::listOfActors (){

    unordered_map<string, int>output;

    for(auto a: this->actorList){
        output.insert(make_pair(a.first, a.second->actorIndex));
    }

    return output;
}

vector<string> Graph::vectorOfActors(){

    vector<string> output(this->actorList.size(), "");

    for(auto a: this->actorList){
        output[a.second->actorIndex] =  a.first;
    }

    return output;
}


bool Graph::loadFromFile(const char* in_filename){

// Initialize the file stream
    ifstream infile(in_filename);

    bool have_header = false;
  
    // keep reading lines until the end of file is reached
    while (infile) {
        string s;
    
        // get the next line
        if (!getline( infile, s )) break;

        if (!have_header) {
            // skip the header
            have_header = true;
            continue;
        }

        istringstream ss( s );
        vector <string> record;

        while (ss) {
            string next;
      
            // get the next string before hitting a tab character and put it in 'next'
            if (!getline( ss, next, '\t' )) break;

            record.push_back( next );
        }
    
        if (record.size() != 3) {
            // we should have exactly 3 columns
            continue;
        }

        string actor_name(record[0]);
        string movie_title(record[1]);
        int movie_year = stoi(record[2]);
    
        // we have an actor/movie relationship, now what?
       
        this->insertGraph(actor_name, movie_title, movie_year);
       
        //build structure //helper in ActorGraph
        

    }

    if (!infile.eof()) {
        cerr << "Failed to read " << in_filename << "!\n";
        return false;
    }

    infile.close();

    return true;
}


void Graph::print(){

    for(auto it=actorList.begin(): actorList){
        cout<<(*it).first<<"\t";
    }
}


