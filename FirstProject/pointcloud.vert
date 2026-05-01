#version 330 core
layout (location = 0) in vec3 aPos; //pos has attribute pos 0
layout (location = 1) in float aIntensity;

out vec3 currPos;
out float intensity;

uniform mat4 camMatrix;
uniform mat4 model;


void main(){

	
	currPos = vec3(model*vec4(aPos.x,aPos.z,aPos.y,1.0f));

	intensity = aIntensity;

	gl_Position = camMatrix * vec4(currPos,1.0f);
}