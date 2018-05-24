#include "ofApp.h"

//--------------------------------------------------------------

ofApp::ofApp()
{
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
    distribution.initData(3, 2000);
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

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);

    cam.begin();

    ofPushMatrix();

//    if (showLines){
//        for (size_t i=0;i < lines.size(); ++i){
//            ofColor c = colors[i];

//            if (i !=MESH_INDEX){
//                c.a = 30;
//            }

//            else c.a = 200;

//            ofSetColor(c);

//            for(std::vector<ofPolyline>::iterator it = lines[i].begin(); it!=  lines[i].end(); ++it){
//                   (*it).draw();
//            }
//        }
//    }

//    for(std::vector<ofMesh>::iterator it = meshes.begin(); it!= meshes.end(); ++it){
//        (*it).draw();
//    }

    ofNoFill();
    ofSetColor(255);

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



//void ofApp::updateNodesFromLines(vector<ofPolyline> lines){
//    vector<ofVec3f> main_nodes;
//    vector<ofVec3f> intermediate_nodes;
//    vector<ofVec3f> directions;

//    std::set<ofVec3f, vec3_compare> node_set;

//    for (auto l: lines){
//        vector<ofVec3f> line_points = l.getVertices();
//        ofVec3f front = line_points.front();
//        ofVec3f back = line_points.back();
//        node_set.insert(front);
//        node_set.insert(back);
//        line_points.pop_back();
//        ofVec3f dir =  (front -back).getNormalized();
//        int count =0;
//        for (auto p: line_points){
//            if (count ==0){
//                count++;
//                continue;
//            }
//            intermediate_nodes.push_back(p);
//            directions.push_back(dir);
//        }
//    }
//    for (auto p: node_set){
//        main_nodes.push_back(p);
//    }
//    int n = main_nodes.size()+ intermediate_nodes.size();
//    numNodes.set(n);
//    particleManager.update_nodes(main_nodes, intermediate_nodes, directions);
//}



void ofApp::updateNodes(){
    vector<ofVec3f> points = distribution.data_subset_points[MESH_INDEX];
    vector<graph_node> nodes = distribution.get_weighted_nearest_neighbours(points, 4);
    numNodes.set(nodes.size());
    particleManager.update_nodes_from_nn(points, nodes);
    //updateNodesFromLines(lines[MESH_INDEX]);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (' ' == key)
    {
        MESH_INDEX = (MESH_INDEX +1)%distribution.get_num_subsets();
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
