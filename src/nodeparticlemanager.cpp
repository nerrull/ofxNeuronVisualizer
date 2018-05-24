#include "nodeparticlemanager.h"


NodeParticleManager::NodeParticleManager()
{
    compute.setupShaderFromFile(GL_COMPUTE_SHADER,"compute1.glsl");
    compute.linkProgram();

    maxSpeed = 300;
    activeNodeForce =0;
    nodeForce=0;
    linearForce = 0;
    repulsionCoeff= 0;
    cohesionCoeff = 0;
    numNodes = 16;
    draw_nodes =False;

    particles.resize(1024*8);
    structure_nodes.resize(4096);

    int i=0;
    for(auto & p: particles){
        p.pos.x = ofRandom(0,500);
        p.pos.y = ofRandom(0,500);
        p.pos.z = ofRandom(0,500);
        p.pos.w = 1;
        p.vel.set(0,0,0,0);
        p.mass.set(ofRandom(1,10),0.,0.,0.);
        i++;
    }

    i =0;
    for(auto & n: structure_nodes){
        i++;

        n.pos.x =  i*ofGetWidth()/structure_nodes.size()*10 - ofGetWidth()/2;
        n.pos.y = -sin(float(i)*TWO_PI*2/structure_nodes.size())* ofGetHeight()/2;
        n.pos.z = -ofGetHeight()/2;
        n.direction = ofVec3f(1.);
        n.is_linear=0.;
        if (i%2 ==0) n.force= 1.;
        else n.force = 0.;
    }

    particlesBuffer.allocate(particles,GL_DYNAMIC_DRAW);
    particlesBuffer2.allocate(particles,GL_DYNAMIC_DRAW);
    nodesBuffer.allocate(structure_nodes,GL_DYNAMIC_DRAW);

    vbo.setVertexBuffer(particlesBuffer,4,sizeof(Particle));
    vbo.setColorBuffer(particlesBuffer,sizeof(Particle),sizeof(ofVec4f)*2);
    vbo.disableColors();
    dirAsColor = false;

    particlesBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    particlesBuffer2.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    nodesBuffer.bindBase(GL_SHADER_STORAGE_BUFFER,2);
}

void NodeParticleManager::update(){

    compute.begin();
    compute.setUniform1f("timeLastFrame",ofGetLastFrameTime());
    compute.setUniform1f("elapsedTime",ofGetElapsedTimef());
    compute.setUniform3f("active_node",activeNode.x,activeNode.y,activeNode.z);
    compute.setUniform1f("max_speed", maxSpeed);
    compute.setUniform1f("node_force", nodeForce);
    compute.setUniform1f("linear_force", linearForce);

    compute.setUniform1f("active_node_force", activeNodeForce);
    compute.setUniform1f("repulsion", repulsionCoeff);
    compute.setUniform1f("cohesion", cohesionCoeff);
    compute.setUniform1f("friction", frictionCoeff);
    compute.setUniform1i("num_nodes", numNodes);

    compute.dispatchCompute((particles.size() + 1024 -1 )/1024, 1, 1);
    compute.end();
    particlesBuffer.copyTo(particlesBuffer2);
}

void NodeParticleManager::draw(){
    ofSetColor(255,70);
    glPointSize(5);
    vbo.draw(GL_POINTS,0,particles.size());
    ofSetColor(255);
    glPointSize(2);
    vbo.draw(GL_POINTS,0,particles.size());

    if (draw_nodes){
        for(auto & n: structure_nodes){

            if (n.is_linear ==1.){
                if(n.force ==1.) ofSetColor(ofColor::blue, 150);
                else ofSetColor(ofColor::red, 150);

                ofDrawBox(n.pos, 5,5,5);
            }

            else {
                ofSetColor(ofColor::green, 150);
                ofDrawSphere(n.pos, 5);
            }

        }
    }
}

void NodeParticleManager::update_active(ofVec3f active){
    activeNode = active;
}

/*
struct graph_node{
    int point_index;
    vector<int> near_indexes;
    vector<float> near_weights;
};*/

void NodeParticleManager::update_nodes_from_nn(vector<ofVec3f> points, vector<graph_node> nodes ){
    numNodes = points.size();
    structure_nodes.resize(numNodes);
    int i =0;
    for (auto n : nodes){
        structure_nodes[i].pos = points[n.point_index];
        structure_nodes[i].is_linear = 0.;
        structure_nodes[i].force = 1.;
        i++;
    }
    nodesBuffer.updateData(structure_nodes);
}


void NodeParticleManager::update_nodes(vector<ofVec3f> main_points, vector<ofVec3f> intermediate_points, vector<ofVec3f> directions){
    numNodes = main_points.size()+ intermediate_points.size();
    structure_nodes.resize(numNodes);
    int i =0;
    for (auto p : main_points){
        structure_nodes[i].pos=p;
        structure_nodes[i].is_linear = 0.;
        structure_nodes[i].force = 1.;
        i++;
    }

    int counter=0;
    for (auto p : intermediate_points){
        structure_nodes[i].pos=p;
        structure_nodes[i].direction = directions[counter];
        structure_nodes[i].is_linear = 1.;
        structure_nodes[i].force = 1.;
        i++;
        counter++;
    }
    nodesBuffer.updateData(structure_nodes);
}


void NodeParticleManager::toggleForces(){
    for(auto & n: structure_nodes){
        if (n.force >0.){
            n.force = -1.;
        }
        else {
            n.force = 1.;
        }
    }
    nodesBuffer.updateData(structure_nodes);
}


void NodeParticleManager::setParameters(ofParameterGroup pg){

    maxSpeed = pg.getFloat("max_speed");
    nodeForce= pg.getFloat("node_force");
    linearForce= pg.getFloat("linear_force");

    activeNodeForce = pg.getFloat("active_node_force");
    repulsionCoeff=pg.getFloat("repulsion");
    cohesionCoeff=pg.getFloat("cohesion");

    frictionCoeff=pg.getFloat("friction");
    numNodes= pg.getInt("num_nodes");
    draw_nodes= pg.getBool("show_nodes");
}
