#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include "MatrixMultiply.hpp"
#include "Graph.hpp"
//#include "ActorGraph.hpp"
using namespace std;

bool sortFunc (pair<string, int> pair1, pair<string, int> pair2){
    if(pair1.second<pair2.second){
        return false;
    }

    else if(pair1.second==pair2.second){
        if(pair1.first.compare(pair2.first)>0){
            return false;
        }
    }

    return true;

}

//reads in matrix, tries to find top 4 results for link prediction
vector<string> top4ResultsConnected(vector<vector<int>> largeMatrix, vector<int> matrix, /*unordered_map<string, int> listOfActors*/ vector<string> listOfActors, string actor, int actorIndex){

    vector<string> output;
    
    //first is actor name, int is frequency value
    vector<pair<string, int>> max;

    auto it = listOfActors.begin();

    for(unsigned int j=0; j<matrix.size(); ++j){
           
        //check large matrix at [x][y] to see if 1 or 0
        if(largeMatrix[actorIndex][j]==1){

            //If link exists and it is not the same actor, insert
            if((*it)/*.first*/!=actor){
                max.push_back(make_pair((*it)/*.first*/, matrix[j]));
            }
        }

        it++;
    }

    std::sort(max.begin(), max.end(), sortFunc);

    unsigned int i = 0;
    unsigned int limit;
    if(max.size()>4){
        limit = 4;
    }
    else{
        limit = max.size();
    }

    while(i<limit){
        output.push_back(max[i].first);
        ++i;
    }
    return output;
}

//reads in matrix, tries to find top 4 results for link prediction
vector<string> top4ResultsUnconnected(vector<vector<int>> largeMatrix, vector<int> matrix,/* unordered_map<string, int> listOfActors*/ vector<string> listOfActors, string actor, int actorIndex){

    vector<string> output;
    
    //first is actor name, int is frequency value
    vector<pair<string, int>> max;

    auto it = listOfActors.begin();

    for(unsigned int j=0; j<matrix.size(); ++j){
           
        //check large matrix at [x][y] to see if 1 or 0
        if(largeMatrix[actorIndex][j]==0){
            

            //If link exists and it is not the same actor, insert
            if(matrix[j]!=0 && (*it)/*.first*/!=actor){
                max.push_back(make_pair((*it)/*.first*/, matrix[j]));
            }
        }

        it++;
    }

    std::sort(max.begin(), max.end(), sortFunc);
    unsigned int i = 0;
    unsigned int limit;
    if(max.size()>4){
        limit = 4;
    }
    else{
        limit = max.size();
    }

    while(i<limit){
        output.push_back(max[i].first);
        ++i;
    }
    
    return output;
}


int main(int argc, const char **argv){

    
    clock_t start;
    double duration;
    start = clock();

    if(argc != 5){
        cout<<"./linkpredictor called with incorrect arguments."<<endl;
        cout<<"Usage: ./linkpredictor movie_cast_tsv_file actor_list actor_existing_predictions actor_new_predictions"<<endl;
        exit(-1);
    }
    
    const char* database = argv[1];
    string inputFile = argv[2];
    string outputFile1 = argv[3];
    string outputFile2 = argv[4];

    //Load the graph
    Graph * graph = new Graph();
    graph->loadFromFile(database/*, false*/);

    
    cout<<"Finished building graph\n";
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    std::cout<<"printf: "<< duration <<'\n';

    //List of actors so we know what to map the adjacency values to.
    unordered_map<string, int> blank_List = graph->listOfActors();
    vector<string> databaseOrder = graph->vectorOfActors();
  
    ifstream infile(inputFile);
    bool have_header=false;
    vector<vector<int>> matrix;

    ofstream output1 (outputFile1);
    ofstream output2 (outputFile2);
    output1<<"Actor1,Actor2,Actor3,Actor4"<<endl;
    output2<<"Actor1,Actor2,Actor3,Actor4"<<endl;

    vector<vector<int>> largeMatrix;
    //auto it = blank_List.begin();
    auto it = databaseOrder.begin();

    vector<string> inputActors;

    while(infile){
        string s;

        if (!getline (infile, s, '\n'))
            break;

        //skip first line
        if(!have_header){
            have_header=true;
            continue;
        }

        vector<int> actorAdjacency = graph->findCoStarsMatrix(s, &blank_List);
        matrix.push_back(actorAdjacency);

        inputActors.push_back(s);
    }

    //while(it!=blank_List.end()){
    while(it!=databaseOrder.end()){
        //cout<<"Making adjacency list for: "<<(*it).first<<endl;
        string actorName = (*it)/*.first*/;

        vector<int> actorAdjacency = graph->findCoStarsMatrix(actorName, &blank_List); 
        largeMatrix.push_back(actorAdjacency);

        ++it;
    }

    cout<<"Finished matrix\n";
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout<<"printf: "<< duration <<'\n';
     
    MatrixOperations* matrixOps = new MatrixOperations(matrix, largeMatrix);
    vector<vector<int>> result= matrixOps->matrixMultiply();
    
    cout<<"Finished Matrix multiplication\n";
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout<<"printf: "<< duration <<'\n';

    for(unsigned int i=0; i<inputActors.size(); ++i){
        vector<string> output = top4ResultsConnected(matrix, result[i], databaseOrder, inputActors[i], i);
        
        cout<<"Connected results for: "<<inputActors[i]<<endl;

        for(unsigned int j=0; j<4; ++j){
            if(j<output.size()){
                output1<<output[j]<<"\t";}
            else{
                output1<<"NULL\t";
            }
        }
    
        output1<<endl;
    }

    
    for(unsigned int i=0; i<inputActors.size(); ++i){
        vector<string> output = top4ResultsUnconnected(matrix, result[i], databaseOrder, inputActors[i], i);
         cout<<"Unonnected results for: "<<inputActors[i]<<endl;
        for(unsigned int j=0; j<4; ++j){
            if(j<output.size()){
                output2<<output[j]<<"\t";
            }
            else{
                output2<<"NULL\t";
            }
        }
    
        output2<<endl;
    }

    infile.close();
    
    delete graph;
    inputActors.clear();
    //blank_List.clear();
    delete matrixOps;
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout<<"printf: "<< duration <<'\n';
    exit(0);

}
