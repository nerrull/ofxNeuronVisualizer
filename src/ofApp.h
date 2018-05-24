#pragma once

#include "ofMain.h"
#include "ofxSpatialHash.h"
#include "ofxGui.h"
#include "nodeparticlemanager.h"
#include "randomdistributiongenerator.h"

class ofApp : public ofBaseApp{

public:
    ofApp();
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void initData();
    void updateNodes();
    void updateNodesFromLines(vector<ofPolyline> lines);



    vector<ofMesh> meshes;
    vector<vector<ofPolyline>> lines;
    vector<ofColor> colors;

    ofEasyCam cam;
    ofMesh mesh;


    int MESH_INDEX =0;

    ofxPanel gui;
    ofParameterGroup shaderUniforms;
    ofParameter<float> fps;
    ofParameter<float> maxSpeed;
    ofParameter<float> nodeForce, activeNodeForce, linearForce;
    ofParameter<float> repulsionCoeff, cohesionCoeff, frictionCoeff;
    ofParameter<int> numNodes;
    ofParameter<bool> showNodes, showLines;
    NodeParticleManager particleManager;
    RandomDistributionGenerator distribution;

    ofVec3f activeNode;




};
