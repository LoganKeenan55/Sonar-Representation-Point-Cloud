#version 330 core

out vec4 fragColor;

in vec3 currPos;
in float intensity;

uniform float gateBottem;
uniform float gateTop;

uniform vec3 camPos;

void main(){

	if(currPos.y < gateBottem || currPos.y > gateTop){
		discard;
	}
	float intensityClamped = clamp(intensity,0.0f,1.0f);
	
	fragColor = vec4(1,1-intensityClamped,1-intensityClamped,1.0f);
}