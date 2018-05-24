#ifndef RANDOMDISTRIBUTIONGENERATOR_H
#define RANDOMDISTRIBUTIONGENERATOR_H

#include "ofMain.h"
#include "ofxSpatialHash.h"

struct graph_node{
    int point_index;
    vector<int> near_indexes;
    vector<float> near_weights;
};

class RandomDistributionGenerator
{
public:
    RandomDistributionGenerator();
    vector<vector<ofVec2f>> sim_data;
    vector<ofVec3f> data_points;

    vector<vector<int>> data_sets;
    vector<vector<ofVec3f>> data_subset_points;

    ofx::KDTree<ofVec2f> * data_hash;
    ofx::KDTree<ofVec2f>::SearchResults data_search_results;

    void initData(int data_dims, int num_data_points);
    vector<graph_node> get_weighted_nearest_neighbours(vector<ofVec3f> points,int num_neighbours);
    std::vector<std::pair<int, int>> get_unique_nearest_neighbour_pairs(vector<ofVec3f> points);
    vector<ofPolyline> lines_from_point_pairs(vector<ofVec3f> points, vector<pair<int,int>> point_pairs, float noise_value);
    vector<ofVec3f> get_data_subset(int index);
    int get_num_subsets();




};

#endif // RANDOMDISTRIBUTIONGENERATOR_H
