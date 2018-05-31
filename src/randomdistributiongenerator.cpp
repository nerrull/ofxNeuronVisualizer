#include "randomdistributiongenerator.h"


bool haveNextNextGaussian = false;
float nextNextGaussian;

float ofxGaussian() {
  if (haveNextNextGaussian){
    haveNextNextGaussian = false;
    return nextNextGaussian;
  }
  else {
    float v1, v2, s;
    do {
      v1 = 2 * ofRandomf() - 1;
      v2 = 2 * ofRandomf() - 1;
      s = v1 * v1 + v2 * v2;
    }
    while (s >= 1 || s == 0);

    float multiplier = sqrt(-2 * log(s)/s);
    nextNextGaussian = v2 * multiplier;
    haveNextNextGaussian = true;

    return v1 * multiplier;
  }
}




vector<float> random_fractions(){
    float sum = 0;
    vector<float> fractions;
    float frac;
    while (sum<1.0){
        frac = ofRandomuf();
        sum +=frac;
        fractions.push_back(sum);
    }
    fractions.pop_back();
    return fractions;
}

vector<float> intermediate(int num){
    float sum = 0;
    vector<float> fractions;
    float frac;
    while (sum<1.0){
        frac = 1./num;
        sum +=frac;
        fractions.push_back(sum);
    }
    fractions.pop_back();
    return fractions;
}



template <typename T>
struct unorderLess
{
    bool operator () (const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const
    {
        const auto lhs_order = lhs.first < lhs.second ? lhs : std::tie(lhs.second, lhs.first);
        const auto rhs_order = rhs.first < rhs.second ? rhs : std::tie(rhs.second, rhs.first);

        return lhs_order < rhs_order;
    }
};

struct vec3_compare {
    bool operator() (const ofVec3f& lhs, const ofVec3f& rhs) const {
        bool equals = (lhs.x==rhs.x)&& (lhs.y ==rhs.y) && (lhs.z == rhs.z);
        return !equals;
    }
};


RandomDistributionGenerator::RandomDistributionGenerator()
{

}



void RandomDistributionGenerator::initData(int data_dims, int num_points){


    sim_data.resize(data_dims);
    data_points.resize(num_points);
    for (std::size_t i = 0; i < num_points; ++i)
    {
        data_points[i] =ofVec3f(ofRandomWidth()-ofGetWidth()/2, ofRandomHeight()-ofGetHeight()/2, ofRandom(-500, 500));
        for (int i =0; i<data_dims; i++) {
            sim_data[i].push_back(ofVec2f((ofxGaussian()+1.)/2.,0.));
        }
    }

    float data_step = 0.05;
    float num_meshes = 1.0/data_step;

    //for (int i = 0; i < data_dims; i++){
    data_hash = new ofx::KDTree<ofVec2f>(sim_data[0]);
    data_hash->buildIndex();
    for( float interval = 0.; interval<1.; interval+=data_step){

        std::vector<ofVec3f> dataset_points;
        ofVec2f searchPoint(interval, 0.);
        data_hash->findPointsWithinRadius(searchPoint, data_step*0.8, data_search_results);
        for (size_t idx = 0; idx<data_search_results.size(); idx++){
           int index = data_search_results[idx].first;
           dataset_points.push_back(data_points[index]);
        }
        data_subset_points.push_back(dataset_points);
    }
}

vector<ofVec3f> RandomDistributionGenerator::get_data_subset(int index){
    return data_subset_points[index];
}

//void RandomDistributionGenerator::make_meshes(){
//    ofColor meshColor = ofColor::fromHsb(interval*255,0.8*255,0.8*255,1.*255);
//    //colors.push_back(meshColor);
//}

//get back a list of point indexes
vector<graph_node> RandomDistributionGenerator::get_weighted_nearest_neighbours(vector<ofVec3f> points,int num_neighbours){
    ofx::KDTree<ofVec3f> hash(points);
    ofx::KDTree<ofVec3f>::SearchResults results;
    vector<graph_node> neighbour_nodes;

    for (size_t idx = 0; idx< points.size(); idx++){
        graph_node n;
        n.point_index = idx;
        ofVec3f point = points[idx];
        //get the 4 nearest neighbours
        hash.findNClosestPoints(point, num_neighbours+1, results);
        // disregard first point because it's the search point
        vector<float> distances;
        distances.resize(num_neighbours);
        float total_distance = 0.;

        for (size_t i = 1; i<results.size(); i++){
           float d=results[i].second;
           total_distance +=d;
           distances[i-1] =d;
           n.near_indexes.push_back(results[i].first);
        }
        float sum= 0;
        for (auto d : distances){
           sum +=(total_distance - d)/total_distance;
           n.near_weights.push_back(sum);
        }
        for (size_t i = 0; i< n.near_weights.size(); i++){
           n.near_weights[i] =n.near_weights[i]/ sum;
        }
        neighbour_nodes.push_back(n);
    }
    return neighbour_nodes;
}

vector<pair<int,int>> RandomDistributionGenerator::get_unique_nearest_neighbour_pairs(vector<ofVec3f> points){
    ofx::KDTree<ofVec3f> hash(points);
    ofx::KDTree<ofVec3f>::SearchResults results;

    std::set<std::pair<int, int>, unorderLess<int>> pair_set;
    for (size_t idx = 0; idx< points.size(); idx++){
        ofVec3f point = points[idx];
        hash.findNClosestPoints(point, 8, results);
        int success_count = 0;
        for (size_t i = 1; i<results.size(); i++){
           int index = results[i].first;
           if(pair_set.insert({idx, index}).second) success_count++;
           if (success_count >0) break;
        }
    }
    vector<pair<int,int>> out;
    out.assign(pair_set.begin(),pair_set.end());
    return out;
}


vector<ofPolyline> lines_from_point_pairs(vector<ofVec3f> points, vector<pair<int,int>> point_pairs, float noise_value){
    vector<ofPolyline> lines;
    vector<pair<int,int>>::iterator it;
    for (it= point_pairs.begin(); it!= point_pairs.end(); ++it){

        ofVec3f start = points[(*it).first];
        ofVec3f end = points[(*it).second];
        ofVec3f direction= (end - start);
        vector<float> fractions = intermediate(10);
        ofPolyline line;
        line.addVertex(start);
        for (size_t idx=0; idx <fractions.size(); idx++){
            ofVec3f noise = ofVec3f(ofRandomf(), ofRandomf(), ofRandomf())*10.;
            ofVec3f p = start + fractions[idx]*direction+ noise *noise_value;
            line.addVertex(p);
        }
        line.addVertex(end);
        lines.push_back(line);
    }
    return lines;
}


int RandomDistributionGenerator::get_num_subsets() {
    return data_subset_points.size();
}
