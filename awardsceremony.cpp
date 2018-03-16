#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>
#include "Graph.hpp"

class Compare{
public:

    bool operator() (pair<Vertex*, vector<Vertex*>> actor1, pair<Vertex*, vector<Vertex*>> actor2){
        return actor1.first->degree > actor2.first->degree;
    }
};

vector<Vertex*> findNeighbors (Vertex* actor /*, Graph* graph*/){
    
    vector<Vertex*> output;
    vector<Edge*> movies = actor->edges;

    for(Edge * e: movies){
        vector<Vertex*> castList = e->actors;
        
        for(Vertex* v: castList){
            auto it= find(output.begin(), output.end(), v);
            if(v!=actor && it==output.end()){
                output.push_back(v);
            }
        }
    }

    actor->degree=output.size();

    return output;
}



vector<string> graphDecomp (Graph * graph, int k){
    
    unordered_map<Vertex*, vector<Vertex*>> kGraphMap;
    vector<string> kGraph;
    std::priority_queue <pair <Vertex*, vector<Vertex*>>, vector<pair<Vertex*,vector <Vertex*>>>, Compare> queue;

    auto it = graph->actorList.begin();

    for(it; it!=graph->actorList.end(); ++it){
        Vertex * inputActor = (*it).second;
        vector<Vertex*> neighbors = findNeighbors(inputActor);
        
        kGraphMap.insert(make_pair(inputActor, neighbors));
    }

    for(auto it2= kGraphMap.begin(); it2!=kGraphMap.end(); ++it2){
        queue.push(*it2);
    }

    while(!queue.empty()){
       pair<Vertex*, vector<Vertex*>> temp = queue.top(); 

        //remove from queue, change degree of neighbors
        if(temp.first->degree<k){ 

            queue.pop();

            //cout<<"Actor: "<<temp.first->actorName<<" has degree: "<<temp.first->degree<<endl;
            if(temp.first->done==false){

                temp.first->done=true;
            
            for(Vertex* v: temp.second){
                
                //cout<<"decrement neighbor: "<<v->actorName<<endl;
                --(v->degree);
                
                if(v->degree<k && v->done==false){

                    auto it = kGraphMap.find(v);
                    queue.push(*it); 
                }
            }
            }
        }

        else{
            break;
        }
    }

    vector<Vertex*> stuff;
    
    //Dump remaining actors into "invite" list
    while(!queue.empty()){
        
        pair<Vertex*, vector<Vertex*>> temp = queue.top();

        if(temp.first->degree>=k){
            kGraph.push_back(temp.first->actorName);
            stuff.push_back(temp.first);    
        }
        queue.pop();

    }

    /*
    for(Vertex* v :stuff){
        if(v->degree<150)
            cout<<v->actorName<<" with degree "<<v->degree<<endl;
    }*/


    sort(kGraph.begin(), kGraph.end());

    return kGraph;
}


int main(int argc, const char **argv){

    if(argc !=4){
        exit(-1);

    }

    const char* database = argv[1];
    const char* kValue = argv[2];
    string outputFile = argv[3];

    int k = atoi(kValue);

    Graph * graph = new Graph();
    graph->loadFromFile(database);
    
    ofstream output (outputFile);
    bool have_header=false;

    //call operations on database
   
    /*
    auto it = graph->actorList.begin();

    
    for(it; it!=graph->actorList.end(); ++it){
        Vertex* actor1 = (*it).second;

        vector<Vertex*> stuff = findNeighbors (actor1);
        
        cout<<"Neighbors for: "<<(*it).first<<" Degree: "<< (*it).second->degree<<endl;

        for(Vertex* v: stuff){
            cout<<v->actorName<<"\t";
        }
        cout<<endl;
    }
*/
    vector<string> kGraph = graphDecomp(graph, k);

/*
    cout<<"Invited to awards Ceremony!!"<<endl;
    for(Vertex* v: kGraph){
        cout<<v->actorName<<" with degree: "<<v->degree<<endl;
    }
*/
    
    output<<"Actor\n";

    for(string s: kGraph){
        output<<s<<"\n";

    }

    delete graph;

    
}
