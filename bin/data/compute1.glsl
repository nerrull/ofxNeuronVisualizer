#version 440

struct Particle{
	vec4 pos;
	vec4 vel;
	vec4 color;
        vec4 mass;
};

struct Node{
    vec3 pos;
    float force;
    vec3 direction;
    float is_linear;

};

layout(std140, binding=0) buffer particle{
    Particle p[];
};

layout(std140, binding=1) buffer particleBack{
    Particle p2[];
};

layout(std140, binding=2) buffer nodes{
    Node n[];
};


uniform vec3 active_node;
uniform float timeLastFrame;
uniform float elapsedTime;
uniform float max_speed;
uniform float active_node_force;
uniform float linear_force;
uniform float node_force;

uniform float repulsion;
uniform float cohesion;
uniform float friction;
uniform int num_nodes;

vec3 rule1(vec3 my_pos, vec3 their_pos){
	vec3 dir = my_pos-their_pos;
	float sqd = dot(dir,dir);
	if(sqd < 300.0*300.0){
            return dir;
	}
	return vec3(0.0);
} 

vec3 rule2(vec3 my_pos, vec3 their_pos, vec3 my_vel, vec3 their_vel){
	vec3 d = their_pos - my_pos;
	vec3 dv = their_vel - my_vel;
	return dv / (dot(dv,dv) + 10.0);
}

vec3 rule3(vec3 my_pos, vec3 their_pos){
	vec3 dir = their_pos-my_pos;
	float sqd = dot(dir,dir);
        if(sqd < 50.0*50.0){
                float f = 1000000.0/(sqd+1);
		return normalize(dir)*f;
	}
	return vec3(0.0);
}

vec3 node_linear(vec3 my_pos, vec3 my_vel, vec3 n_pos, vec3 n_direction){

     vec3 dir = n_pos-my_pos;
     float sqd = dot(dir,dir);

     if(sqd > 50.0*50.0){
         return vec3(0.0);
     }

     vec3 vel = normalize(my_vel);
     float mag = (dot(n_direction, vel) +0.0001);
     return n_direction*mag;
}

vec3 node_attraction(vec3 my_pos, vec3 n_pos){
     vec3 dir = n_pos-my_pos;
     float sqd = dot(dir,dir);
     if(sqd < 50.0*50.0){
         float f =  10000.0/(sqd+1);
         return normalize(dir)*f;
     }
     return vec3(0.0);
}

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;
void main(){
	vec3 particle = p2[gl_GlobalInvocationID.x].pos.xyz;
	vec3 acc = vec3(0.0,0.0,0.0);
        uint m = uint(1024.0*8.0*elapsedTime);
        uint start = m%(1024*8-512);
        uint end = start + 512;
	float minDist;
	uint first = 1;
//        uint start =0;
//        uint endstructure_nodes = 1024 *8;
        for(uint i=start;i<end;i++){
                if(i!=gl_GlobalInvocationID.x){
                        acc += rule1(particle,p2[i].pos.xyz) * repulsion;
                        acc += rule2(particle,p2[i].pos.xyz, p2[gl_GlobalInvocationID.x].vel.xyz, p2[i].vel.xyz) * cohesion;
//			acc += rule3(particle,p2[i].pos.xyz) * attraction;
                }
        }

        start = 0;
        end =num_nodes;
        vec3 vel_unit = normalize(p[gl_GlobalInvocationID.x].vel.xyz + vec3(0.000001));

        for(uint i = start; i<end; i++){

                if (n[i].is_linear>0.){
                   acc += node_linear(particle, vel_unit, n[i].pos.xyz, n[i].direction)*linear_force*n[i].force.x;
                }
                else acc+= node_attraction(particle, n[i].pos.xyz)*node_force;

        }
	
	p[gl_GlobalInvocationID.x].pos.xyz += p[gl_GlobalInvocationID.x].vel.xyz*timeLastFrame;
	
        vec3 dir = active_node - particle;
        acc += normalize(dir)*active_node_force;
        acc = acc/ p2[gl_GlobalInvocationID.x].mass.x;
	p[gl_GlobalInvocationID.x].vel.xyz += acc*timeLastFrame;
        p[gl_GlobalInvocationID.x].vel.xyz *= (1.-friction);
	
        vel_unit = normalize(p[gl_GlobalInvocationID.x].vel.xyz);

	if(length(p[gl_GlobalInvocationID.x].vel.xyz)>max_speed){
                p[gl_GlobalInvocationID.x].vel.xyz = vel_unit * max_speed;
	}
	
	float max_component = max(max(dir.x,dir.y),dir.z);
	p[gl_GlobalInvocationID.x].color.rgb = dir/max_component;
	p[gl_GlobalInvocationID.x].color.a = 0.4;
}
