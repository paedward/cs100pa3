/* Pathfinder.cpp
 * 
 *
 *
 *
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include "ActorGraph.hpp"
using namespace std;

class Vertex{

public:

    bool done;
    string prev;
    string movieWithPrev;
    int dist;
    string actorName;

    Vertex(){
        prev = "";
        movieWithPrev = "";
        done = false;
        dist = 1000000;
    }
};
/*
class ActorNode{

public:

    bool done;
    ActorNode* prev;
    string movieWithPrev;
    int dist;
    string actorName;

    ActorNode(){
        prev = nullptr;
        movieWithPrev = "";
        done = false;
        dist = 1000000;
    }
};



class Compare2{
public:
    bool operator() (pair<int, ActorNode*> actor1, pair<int, ActorNode*> actor2){

        return actor1.first<actor2.first;
    }
};

*/

class Compare{
public:
    bool operator() (pair<string, pair<int, Vertex*>> actor1, pair<string, pair<int, Vertex*>> actor2){

        return actor1.second.first<actor2.second.first;
    }
};
/*
vector <string> findPathWeighted (string actor1, string actor2, ActorGraph * graph){
    
    std::priority_queue <pair <int, ActorNode*>, vector<pair<int, ActorNode*>>, Compare2> queue;

    unordered_map<string, ActorNode*> vertices;

    ActorNode * start = new ActorNode();
    start->dist=0;
    start->actorName = actor1;
    pair<string, ActorNode*> insertPair = make_pair(actor1, start);
    vertices.insert(insertPair);
    
    queue.push(make_pair(0, start));
    
    while(queue.empty()){

        pair<int, ActorNode*> temp = queue.top();

        queue.pop();
    
        if (temp.second->done == false){
            temp.second->done = true;

                //look for neighbors
                string currActor = temp.second->actorName;
                auto actorIter = graph->actors.find(currActor);
                vector<string> filmList = (*actorIter).second;

                //check every film
                for(string movie: filmList){
                    auto currentFilm = graph->movies.find(movie);
                    
                    vector<string> castList = (*currentFilm).second;
                    
                    //every one they acted with in that movie
                    for(string coStar: castList){
                        
                        auto verticesCheck = vertices.find(coStar);

                        //if not there, make new Vertex*
                        //if(verticesCheck == vertices.end()){  
                        //}
    

                        ActorNode * neighbor = new ActorNode();

                        int weight = stoi(movie.substr(movie.length()-4));
                        weight = 2018-weight+1;
        
                        int c = temp.second->dist+weight;

                        if(c<neighbor->dist){
                            neighbor->prev = temp.second;
                            neighbor->dist = c;
                            neighbor->movieWithPrev = movie;

                            queue.push(make_pair(c, neighbor));
                        }

                    }

                    

                }



        }


    }

    vector<string> output;

    auto it2 = vertices.find(actor2);
    pair<string, ActorNode*> temp = (*it2);

    while(true){
        
        output.push_back(temp.second->actorName);   

        if(temp.second->prev==nullptr){
            break;
        }

        string year = temp.second->movieWithPrev.substr(temp.second->movieWithPrev.length()-4);
        string title=temp.second->movieWithPrev.substr(0, temp.second->movieWithPrev.length()-5);
    
        output.push_back(year);
        output.push_back(title);

        it2 = vertices.find(temp.second->prev->actorName);

        temp= (*it2);
    }
    
    auto vertex = vertices.begin();  
    
    while(vertex!=vertices.end())   
    {
        delete (*vertex).second;
        ++vertex;
    }
        vertices.clear();


    return output;

}
*/

vector <string> findPathWeighted (string actor1, string actor2, ActorGraph * graph){ 
 
    vector <string> output;
        
    std::priority_queue <pair <string, pair <int, Vertex*>>, vector<pair<string, pair<int, Vertex*>>>, Compare> queue;
    unordered_map<string, pair<int, Vertex*>> vertices; 


    //Setting up first vertex
    pair<int, Vertex*> first (0, new Vertex());
    first.second->dist = 0;
    pair<string, pair<int, Vertex*>> start = make_pair(actor1, first);
    vertices.insert(start); 

    //Enqueue starting vertex
    queue.push(start);

    //While queue has elements.
    while(!queue.empty()){

        //Dequeue lowest weight.
        pair<string, pair<int, Vertex*>> temp = queue.top();
        queue.pop();

        //If vertex's min path hasn't been discovered yet
        if(temp.second.second->done==false){
            temp.second.second->done=true;


            std::unordered_map<string, vector <string>>::iterator currentActor= 
                graph->actors.find(temp.first);
                
            vector <string> filmList = (*currentActor).second;

           
            //FOR EACH NEIGHBOR
            for(string s: filmList){
                
                std::unordered_map<string, vector <string>>::iterator currentFilm =
                graph->movies.find(s);

                    for(string a: (*currentFilm).second){
       
                        //Dont add if same Actor!!!
                        if(a == temp.first){
                            continue;
                        }

                        unordered_map<string, pair<int, Vertex*>>::iterator coStar = 
                            vertices.find(a); 
                   
                        int weight = stoi(s.substr(s.length()-4));
                        weight = 2018-weight+1;
                            
                        //insert actors into vertices if not present
                        
                        if(coStar==vertices.end()){

                            pair<int, Vertex*> entry (weight, new Vertex());
                            pair<string, pair<int, Vertex*>> addMe = make_pair(a, entry);
                            vertices.insert(addMe); 
                            coStar = vertices.find(a);
                        }

                        int c = temp.second.second->dist + weight;
               
                        if (c < (*coStar).second.second->dist){
                            (*coStar).second.second->prev = temp.first;
                            (*coStar).second.second->movieWithPrev = s;     
                            (*coStar).second.second->dist = c;

                            queue.push((*coStar));
                        }
                    } 
            }         
        }
    }
    
    auto it2 = vertices.find(actor2);
    pair<string, pair<int, Vertex*>> temp = (*it2);

    while(true){
        
        output.push_back(temp.first);   

        if(temp.second.second->prev==""){
            break;
        }

        string year = temp.second.second->movieWithPrev.substr(temp.second.second->movieWithPrev.length()-4);
        string title=temp.second.second->movieWithPrev.substr(0, temp.second.second->movieWithPrev.length()-5);
    
        output.push_back(year);
        output.push_back(title);

        it2 = vertices.find(temp.second.second->prev);

        temp= (*it2);
    }
    
    unordered_map<string, pair<int, Vertex*>>::iterator vertex = vertices.begin();  
    
    while(vertex!=vertices.end())   
    {
        delete (*vertex).second.second;
        ++vertex;
    }
        vertices.clear();


    return output;
}

vector <string> findPathUnweighted (string actor1, string actor2, ActorGraph * graph){ 
 
    vector <string> output;
        
    std::priority_queue <pair <string, pair <int, Vertex*>>, vector<pair<string, pair<int, Vertex*>>>, Compare> queue;
    unordered_map<string, pair<int, Vertex*>> vertices; 

    //Setting up first vertex
    pair<int, Vertex*> first (0, new Vertex());
    first.second->dist = 0;
    pair<string, pair<int, Vertex*>> start = make_pair(actor1, first);
    vertices.insert(start); 

    //Enqueue starting vertex
    queue.push(start);

    //While queue has elements.
    while(!queue.empty()){

        //Dequeue lowest weight.
        pair<string, pair<int, Vertex*>> temp = queue.top();
        queue.pop();
        
        //If vertex's min path hasn't been discovered yet
        if(temp.second.second->done==false){
            temp.second.second->done=true;

            std::unordered_map<string, vector <string>>::iterator currentActor= 
                graph->actors.find(temp.first);
                
            vector <string> filmList = (*currentActor).second;

           
            //FOR EACH NEIGHBOR
            for(string s: filmList){
                
                std::unordered_map<string, vector <string>>::iterator currentFilm =
                graph->movies.find(s);

                    for(string a: (*currentFilm).second){
       
                        //Dont add if same Actor!!!
                        if(a == temp.first){
                            continue;
                        }

                        unordered_map<string, pair<int, Vertex*>>::iterator coStar = 
                            vertices.find(a); 
                   
                        int weight = 1; 

                        //insert actors into vertices if not present
                        if(coStar==vertices.end()){

                            //Vertex* vert = new Vertex();
                            //vert->dist = weight;
                            pair<int, Vertex*> entry (weight, new Vertex());
                            pair<string, pair<int, Vertex*>> addMe = make_pair(a, entry);
                            vertices.insert(addMe); 
                            coStar = vertices.find(a);
                        }

                        int c = temp.second.second->dist + /*(*coStar).second.first*/ weight;
                
                        if (c < (*coStar).second.second->dist){
                            (*coStar).second.second->prev = temp.first;
                            (*coStar).second.second->movieWithPrev = s;     
                            (*coStar).second.second->dist = c;

                            queue.push((*coStar));
                        }
                    } 
            }         
        }
    }
    
    unordered_map<string, pair<int, Vertex*>>::iterator it = vertices.find(actor2);
    pair<string, pair<int, Vertex*>> temp = (*it);

    while(true){
        
        output.push_back(temp.first);   

        if(temp.second.second->prev==""){
            break;
        }

        string year = temp.second.second->movieWithPrev.substr(temp.second.second->movieWithPrev.length()-4);
        string title=temp.second.second->movieWithPrev.substr(0, temp.second.second->movieWithPrev.length()-5);
    
        output.push_back(year);
        output.push_back(title);

        it = vertices.find(temp.second.second->prev);

        temp= (*it);
    }
    
    unordered_map<string, pair<int, Vertex*>>::iterator vertex = vertices.begin();  
    
    while(vertex!=vertices.end())   
    {
        delete (*vertex).second.second;
        ++vertex;
    }
        vertices.clear();


    return output;
}


int main(int argc, const char **argv){

    if(argc < 4){
        cout<<"./pathfinder called with incorrect arguments."<<endl;
        cout<<"Usage: ./pathfinder movie_cast_tsv_file u/w pairs_tsv_file output_paths_tsv_file\n";
        exit(-1);
    }

    //arguments
    const char* database = argv[1];
    string graphType = argv[2];
    string inputFile = argv[3];
    string outputFile = argv[4];
    
    ActorGraph *  graph = new ActorGraph();

    bool weighted;

    if(graphType == "w"){
        weighted = true;
    }

    else if(graphType == "u"){
        weighted = false;
    }
    
    else{
        cout<<"Incorrect usage; use u/w to create unweighted/weighted graph."<<endl;
        return -1;
    }

    graph->loadFromFile(database, false);
    
    ifstream infile(inputFile);
    bool have_header=false;
    
    ofstream output (outputFile); 
    output<<"(actor)--[movie#@year]-->(actor)--...\n";


    //read in test pairs
    while(infile){
        string s;

        if (!getline(infile, s))
            break;

        if(!have_header){
            have_header = true;
            continue;
        }

        istringstream ss(s);
        vector <string>record;

        while(ss){
            string next;

            if(!getline(ss, next, '\t'))
                break;

            record.push_back(next);
        }
    
        if (record.size() !=2)
            continue;

        string actor1(record[0]);
        string actor2(record[1]);
        
        //search for path
        vector<string> writeToFile;

        if(weighted){
            writeToFile = findPathWeighted(actor1, actor2, graph /*,weighted*/ );
        }
        else{
            writeToFile = findPathUnweighted(actor1, actor2, graph /*,weighted*/ );
        }

        bool yes = true;

        if(writeToFile.size()<1)
            yes = false;
        
        while(yes){

            output<<"("<<writeToFile.back()<<")";
            writeToFile.pop_back();

            if(writeToFile.size()==0)
                break;

            output<<"--["<<writeToFile.back()<<"#@";
            writeToFile.pop_back();

            output<<writeToFile.back()<<"]-->";
            writeToFile.pop_back();
        }
        
        output<<"\n";

    }

    infile.close();

    delete graph;

    return 0;

}
