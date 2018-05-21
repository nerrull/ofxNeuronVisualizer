#pragma once

#include "ofMain.h"
#include "ofxSpatialHash.h"
#include "ofxGui.h"
#include "nodeparticlemanager.h"


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




    typedef ofVec2f Vec2;
    typedef ofVec3f Vec3;

    enum
    {
        NUM_POINTS = 2000,
        DEFAULT_RADIUS = 100, // vector units (pixels)
        DEFAULT_NEAREST_N = 100
    };

    /// These points MUST be initialized BEFORE initing the hash.
    std::vector<Vec3> points;
    std::vector<std::vector<ofVec2f>> sim_data;

    vector<ofMesh> meshes;
    vector<vector<ofPolyline>> lines;
    vector<ofColor> colors;

    ofx::KDTree<ofVec2f> * data_hash;
    ofx::KDTree<ofVec2f>::SearchResults data_search_results;

    /// \brief The spatial hash specialized for ofVec3f.
    ofx::KDTree<Vec3> hash;

    /// \brief The search results specialized for ofVec3f.
    ofx::KDTree<Vec3>::SearchResults searchResults;

    /// \brief A little firefly that moves around the 3D space.
    Vec3 firefly;

    /// \brief The camera.
    ofEasyCam cam;


    /// \brief A mesh to make it easier to draw lots of points.
    ofMesh mesh;

    /// \brief The search modes in this example.
    enum Modes
    {
        MODE_RADIUS,
        MODE_NEAREST_N
    };

    /// \brief The current search mode.
    int mode = 0;

    /// \brief Radius used for radius search.
    int radius = 0;

    /// \brief Number of nearest neighbors to use for Nearest Nieghbor search.
    int nearestN = 0;

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

    ofVec3f activeNode;




};
