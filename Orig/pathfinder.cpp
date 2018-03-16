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

    Vertex(){
        prev = "";
        movieWithPrev = "";
        done = false;
        dist = 1000000;
    }
};

class Compare{
public:
    bool operator() (pair<string, pair<int, Vertex*>> actor1, pair<string, pair<int, Vertex*>> actor2){
        return actor1.second.first<actor2.second.first;
    }
};

vector <string> findPath (string actor1, string actor2, ActorGraph * graph, bool weighted){ 
 
    vector <string> output;
        
    std::priority_queue <pair <string, pair <int, Vertex*>>, vector<pair<string, pair<int, Vertex*>>>, Compare> queue;
    unordered_map<string, pair<int, Vertex*>> vertices; 

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

        if(temp.first == actor2){
            break;
        }

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
        
                        if(a == temp.first){
                            continue;
                        }

                        unordered_map<string, pair<int, Vertex*>>::iterator coStar = 
                            vertices.find(a); 
                   
                        int weight = 1; 

                            if(weighted){
                                weight = stoi(s.substr(s.length()-4));
                                weight = 2018-weight+1;
                            }

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

        vector<string> writeToFile = findPath(actor1, actor2, graph, weighted );

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
    
    //if(!infile.eof())
    //    return 0;

    infile.close();

    delete graph;

    return 0;

}
