/*
 * ActorGraph.hpp
 * Author: <YOUR NAME HERE>
 * Date:   <DATE HERE>
 *
 * This file is meant to exist as a container for starter code that you can use to read the input file format
 * defined in movie_casts.tsv. Feel free to modify any/all aspects as you wish.
 */

#ifndef ACTORGRAPH_HPP
#define ACTORGRAPH_HPP
#include <utility>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

// Maybe include some data structures here

using namespace std;

class ActorGraph{
protected:

// Maybe add class data structure(s) here

public:

    unordered_map<string, vector<string>> actors;
    unordered_map<string, vector<string>> movies;

    ActorGraph(void);

    ~ActorGraph(){

        actors.clear();
        movies.clear();
    }
    
    bool findCoStars (string actor, string movieName, vector<string>* coStars, 
        vector<string>* sharedMovies){

        bool flag = false;

        std::unordered_map<string, vector<string>>::iterator movie = movies.find(movieName);

        if(movie == movies.end())
            return flag;

        for(string s: (*movie).second){

            //Check if entry is already in coStars. If it isn't, add to coStars
            if(std::find(coStars->begin(), coStars->end(), s) == coStars->end()){

                if(s!=actor){
                    flag = true;
                    coStars->push_back(s);
                    sharedMovies->push_back(movieName);

                }
            } 
        }
        return flag;
    }
    
/*
    vector<int> findCoStarsMatrix(string actor, unordered_map<string, int> * adjacencyList){
       
        //Movies the actor has starred in
        unordered_map<string, int> adjacencyCopy (*adjacencyList);
        vector<string> filmList = (*actors.find(actor)).second;
        vector<string>::iterator movie = filmList.begin();

        //Loop through every movie to find actors
        while(movie!= filmList.end()){
           
            //Find actor
            auto film = movies.find(*movie);
            auto coStar = (*film).second.begin();
            
            while(coStar!= (*film).second.end() ){
                //Change value in adjacency map to 1
                auto adjacency = adjacencyCopy.find(*coStar);
    
                if((*adjacency).second==0 && (*coStar!=actor))
                    (*adjacency).second =1;
                    coStar++;
            }  

            movie++;
        }

        vector <int> output;

        for(auto a: adjacencyCopy){
            output.push_back(a.second);
        }
        return output;
    }
*/
    unordered_map<string, int> listOfActors (){
        
        unordered_map<string, int>output;

        for(auto a: this->actors){
            output.insert(make_pair(a.first, 0));
        }

        return output;

    }


    void insertGraph(string actorName, string movieName, int movieYear, bool weighted){

        std::unordered_map<string, vector <string>>::iterator actor = 
            actors.find(actorName);

        string movieNameAndYear = movieName+'\t'+to_string(movieYear);

        std::unordered_map<string, vector <string>>::iterator film = 
            movies.find(movieNameAndYear);
    
           

        //need to insert both into list, and to adjacency lists 
        if(actor==actors.end() && film==movies.end()){

            vector <string> castList;
            castList.push_back(actorName);
            pair <string, vector<string>> addMeToo = make_pair(movieNameAndYear, castList);
            movies.insert(addMeToo);

            vector<string> actorsFilms;
            actorsFilms.push_back(movieNameAndYear);

            //Add actor+movielist to the map
            pair <string, vector<string>>addMe = make_pair(actorName, actorsFilms);
            actors.insert(addMe); 
        }


        //film is already entered, but not the actor
        else if(actor==actors.end() && film!=movies.end()){

            //Insert film to actors list
            vector<string> actorsFilms;
            actorsFilms.push_back(movieNameAndYear);
            pair <string, vector<string>> addMe = make_pair(actorName, actorsFilms);
            //Add actor+movielist to the map
            actors.insert(addMe);

            //add actor to cast list
            (*film).second.push_back(actorName);
        }
    
        //actor is already entered, but not the film
        else if(actor!=actors.end() && film==movies.end()){
        
            (*actor).second.push_back(movieNameAndYear);

            vector<string> castList;
            castList.push_back(actorName);
            pair <string, vector<string>> addMe = make_pair(movieNameAndYear, castList);
            movies.insert(addMe);

        }

        //Already entered, connect the two in their adjacency lists.
        else{
            (*film).second.push_back(actorName);
            (*actor).second.push_back(movieNameAndYear);
        }
    }

    void print(){
        
        for(pair<string, vector<string>> a : actors){
            cout<<a.first<<" ";

            for(string s: a.second){
                cout<<s<<endl;
            }

        }

    }

    // Maybe add some more methods here
  
    /** You can modify this method definition as you wish
     *
     * Load the graph from a tab-delimited file of actor->movie relationships.
     *
     * in_filename - input filename
     * use_weighted_edges - if true, compute edge weights as 1 + (2018 - movie_year), otherwise all edge weights will be 1
     *
     * return true if file was loaded sucessfully, false otherwise
     */
    bool loadFromFile(const char* in_filename, bool use_weighted_edges);
};

#endif // ACTORGRAPH_HPP
