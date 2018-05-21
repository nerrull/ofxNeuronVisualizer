#include "ofApp.h"

//--------------------------------------------------------------

ofApp::ofApp():
    hash(points),
    mode(MODE_RADIUS),
    radius(DEFAULT_RADIUS),
    nearestN(DEFAULT_NEAREST_N)
{
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

void ofApp::setup(){
    ofEnableBlendMode(OF_BLENDMODE_ADD);

    mesh.setMode(OF_PRIMITIVE_POINTS);

    gui.setup();
    gui.add(fps.set("fps",60,0,60));
    shaderUniforms.add(maxSpeed.set("max_speed",2500,0,5000));
    shaderUniforms.add(activeNodeForce.set("active_node_force",100,0,5000));
    shaderUniforms.add(nodeForce.set("node_force",100,0,5000));
    shaderUniforms.add(linearForce.set("linear_force",100,0,5000));

    shaderUniforms.add(repulsionCoeff.set("repulsion",0.1,0,1));
    shaderUniforms.add(cohesionCoeff.set("cohesion",0.05,0,1));
    shaderUniforms.add(frictionCoeff.set("friction", 0.01, 0.,.1));
    shaderUniforms.add(numNodes.set("num_nodes",16,0,1024));
    shaderUniforms.add(showNodes.set("show_nodes", false));
    shaderUniforms.add(showLines.set("show_lines", false));


    gui.add(shaderUniforms);
    firefly = Vec3(ofGetWidth() / 2, ofGetHeight() / 2, 0);

    initData();
}

void ofApp::initData(){
    int data_dims = 3;

    for (int i = 0; i < data_dims; i++){
        std::vector<ofVec2f> data;
        sim_data.push_back(data );
    }


    for (std::size_t i = 0; i < NUM_POINTS; ++i)
    {
        Vec3 point(ofRandomWidth(), ofRandomHeight(), ofRandom(-500, 500));

        for (int i =0; i<data_dims; i++) sim_data[i].push_back(ofVec2f((ofxGaussian()+1.)/2., 0.));
        points.push_back(point);
        mesh.addVertex(point);
    }

    float data_step = 0.05;
    float num_meshes = 1.0/data_step;

    vector<vector<int>> data_sets;

    //for (int i = 0; i < data_dims; i++){
    data_hash = new ofx::KDTree<ofVec2f>(sim_data[0]);

    for( float interval = 0.; interval<1.; interval+=data_step){
        vector<ofPolyline> temp_lines;
        ofColor meshColor = ofColor::fromHsb(interval*255,0.8*255,0.8*255,1.*255);
        colors.push_back(meshColor);
        std::vector<ofVec3f> meshPoints;
        ofVec2f searchPoint(interval, 0.);
        data_hash->findPointsWithinRadius(searchPoint, data_step*0.8, data_search_results);
        for (size_t idx = 0; idx<data_search_results.size(); idx++){
           int index = data_search_results[idx].first;
           meshPoints.push_back(points[index]);
        }

        ofMesh newMesh;
        newMesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
        newMesh.addVertices(meshPoints);

        ofx::KDTree<ofVec3f> meshHash(meshPoints);
        ofx::KDTree<ofVec3f>::SearchResults meshResults;

        std::set<std::pair<int, int>, unorderLess<int>> pair_set;
        for (size_t idx = 0; idx< meshPoints.size(); idx++){
            ofVec3f point = meshPoints[idx];
            meshHash.findNClosestPoints(point, 8, meshResults);
            int success_count = 0;
            for (size_t i = 3; i<meshResults.size(); i++){
               int index = meshResults[i].first;
               if(pair_set.insert({idx, index}).second) success_count++;
               if (success_count >0) break;
            }
            newMesh.addColor(meshColor);
        }

        std::set<std::pair<int, int>>::iterator it;
        for (it= pair_set.begin(); it!= pair_set.end(); ++it){
            ofVec3f start = meshPoints[(*it).first];
            ofVec3f end = meshPoints[(*it).second];
            ofVec3f direction= (end - start);
            vector<float> fractions = intermediate(10);
            ofPolyline line;
            line.addVertex(start);
            for (size_t idx=0; idx <fractions.size(); idx++){
                //ofVec3f noise = ofVec3f(ofRandomf(), ofRandomf(), ofRandomf())*10.;
                ofVec3f p = start + fractions[idx]*direction;
                line.addVertex(p);
            }
            line.addVertex(end);
            temp_lines.push_back(line);
        }
        lines.push_back(temp_lines);
        meshes.push_back(newMesh);
    }

    hash.buildIndex();
}


//--------------------------------------------------------------
void ofApp::update(){

    fps = ofGetFrameRate();
    activeNode.set(ofGetWidth()/4,ofGetHeight()/4,250.);
    particleManager.update_active(activeNode);
    particleManager.setParameters(shaderUniforms);
    particleManager.update();


    // Random walk.
    float time = ofGetElapsedTimef() / 10;

    firefly.x = ofMap(ofNoise(time +   0), 0, 1, -500, ofGetWidth() + 500);
    firefly.y = ofMap(ofNoise(time + 100), 0, 1, -500, ofGetHeight() + 500);
    firefly.z = ofMap(ofNoise(time + 500), 0, 1, -1000, 1000);



    if (MODE_RADIUS == mode)
    {
        // An estimate of the number of points we are expecting to find.

        // Estimate the volume of our seach envelope as a cube.
        // A cube already overestimates a spherical search space.
        float approxCubicSearchBoxSize = (radius * 2 * radius * 2 * radius * 2);

        // Calculate the volume of our total search space as a cube.
        float approxCubicSearchSpaceSize = (ofGetWidth() * ofGetHeight() * 2 * 500);

        // Determine the percentage of the total search space we expect to capture.
        float approxPercentageOfTotalPixels = approxCubicSearchBoxSize / approxCubicSearchSpaceSize;

        // Assuming an uniform distribution of points in our search space,
        // get a percentage of them.
        std::size_t approximateNumPointsToFind = points.size() * approxPercentageOfTotalPixels;

        searchResults.resize(approximateNumPointsToFind);

        hash.findPointsWithinRadius(firefly, radius, searchResults);
    }
    else
    {
        searchResults.resize(nearestN);

        // NOTE: this method signature is slower that the alternative method
        // signature as this method signature requires extra copies.  This is
        // done to simplify the example and use the same search results data
        // structure.
        hash.findNClosestPoints(firefly, nearestN, searchResults);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);

    cam.begin();

    ofPushMatrix();
    //ofTranslate(- ofGetWidth() / 2, - ofGetHeight() / 2, 0);



//    ofSetColor(colors[MESH_INDEX]);
//    for(std::vector<ofPolyline>::iterator it = lines[MESH_INDEX].begin(); it!=  lines[MESH_INDEX].end(); ++it){
//           (*it).draw();
//    }
//    meshes[MESH_INDEX].draw();
    if (showLines){
        for (size_t i=0;i < lines.size(); ++i){
            ofColor c = colors[i];

            if (i !=MESH_INDEX){
                c.a = 30;
            }

            else c.a = 200;

            ofSetColor(c);

            for(std::vector<ofPolyline>::iterator it = lines[i].begin(); it!=  lines[i].end(); ++it){
                   (*it).draw();
            }
            //meshes[i].draw();
        }
    }

//    for(std::vector<ofMesh>::iterator it = meshes.begin(); it!= meshes.end(); ++it){
//        (*it).draw();
//    }

    ofNoFill();
    ofSetColor(255);

    // Draw all of the points.
    //mesh.draw();

    ofFill();
    ofSetColor(255, 255, 0, 80);


    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofSetColor(ofColor::red);
    ofDrawSphere(activeNode,20);
    particleManager.draw();

    ofPopMatrix();

    cam.end();


    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofSetColor(255);
    gui.draw();
}



void ofApp::updateNodesFromLines(vector<ofPolyline> lines){
    vector<ofVec3f> main_nodes;
    vector<ofVec3f> intermediate_nodes;
    vector<ofVec3f> directions;

    std::set<ofVec3f, vec3_compare> node_set;

    for (auto l: lines){
        vector<ofVec3f> line_points = l.getVertices();
        ofVec3f front = line_points.front();
        ofVec3f back = line_points.back();
        node_set.insert(front);
        node_set.insert(back);
        line_points.pop_back();
        ofVec3f dir =  (front -back).getNormalized();
        int count =0;
        for (auto p: line_points){
            if (count ==0){
                count++;
                continue;
            }
            intermediate_nodes.push_back(p);
            directions.push_back(dir);
        }
    }
    for (auto p: node_set){
        main_nodes.push_back(p);
    }
    int n = main_nodes.size()+ intermediate_nodes.size();
    numNodes.set(n);
    particleManager.update_nodes(main_nodes, intermediate_nodes, directions);
}
void ofApp::updateNodes(){
    updateNodesFromLines(lines[MESH_INDEX]);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (' ' == key)
    {
        MESH_INDEX = (MESH_INDEX +1)%meshes.size();
        updateNodes();

    }

    else if (key == 'f'){
        ofToggleFullscreen();
    }

    else if (key == 't'){
        particleManager.toggleForces();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
