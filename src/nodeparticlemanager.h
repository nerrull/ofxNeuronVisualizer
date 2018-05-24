#ifndef NODEPARTICLEMANAGER_H
#define NODEPARTICLEMANAGER_H
#include "ofxGui.h"
#include "ofBufferObject.h"
#include "ofMain.h"
#include "randomdistributiongenerator.h"

class NodeParticleManager
{
public:
    NodeParticleManager();
    void update();
    void draw();

    void toggleForces();
    void setParameters(ofParameterGroup pg);
    void update_active(ofVec3f active);
    void update_nodes(vector<ofVec3f> , vector<ofVec3f>, vector<ofVec3f>);
    void updateNodesFromLines(vector<ofPolyline>);
    void update_nodes_from_nn(vector<ofVec3f> points, vector<graph_node> nodes );

    struct Particle{
        ofVec4f pos;
        ofVec4f vel;
        ofFloatColor color;
        ofVec4f mass;
    };

    struct Node{
        ofVec3f pos;
        ofVec3f direction;
        float force;
        float is_linear;
    };

    vector<Particle> particles;
    vector<Node> structure_nodes;
    ofVec3f activeNode;


    ofShader compute;
    ofBufferObject particlesBuffer, particlesBuffer2, nodesBuffer;
    GLuint vaoID;
    ofVbo vbo;
    bool dirAsColor;

    bool draw_nodes;
    float maxSpeed;
    float nodeForce, activeNodeForce, linearForce;
    float repulsionCoeff, cohesionCoeff, frictionCoeff;
    int numNodes;



};

#endif // NODEPARTICLEMANAGER_H
