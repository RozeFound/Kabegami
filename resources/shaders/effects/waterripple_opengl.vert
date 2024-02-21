#version 450
#include "common.h"

layout(set=0, binding = 0) uniform globals {

	uniform mat4 g_ModelViewProjectionMatrix;
	uniform float g_Time;
	uniform vec4 g_Texture0Resolution;
	uniform vec4 g_Texture2Resolution;

	uniform float g_AnimationSpeed; // {"material":"ui_editor_properties_animation_speed","default":0.15,"range":[0,0.5]}
	uniform float g_Scale; // {"material":"ui_editor_properties_ripple_scale","default":1,"range":[0,10]}
	uniform float g_ScrollSpeed; // {"material":"ui_editor_properties_scroll_speed","default":0,"range":[0,0.5]}
	uniform float g_Direction; // {"material":"ui_editor_properties_scroll_direction","default":0,"range":[0,6.28]}

};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(location = 0) out vec4 v_TexCoord;
layout(location = 1) out vec4 v_TexCoordRipple;


void main() {

	gl_Position = vec4(a_Position, 1.0) * g_ModelViewProjectionMatrix;
	v_TexCoord.xy = a_TexCoord;

	vec2 coordsRotated = v_TexCoord.xy;
	vec2 coordsRotated2 = v_TexCoord.xy * 1.333;
	
	vec2 scroll = rotateVec2(vec2(0, -1), g_Direction) * g_ScrollSpeed * g_ScrollSpeed * g_Time;
	
	v_TexCoordRipple.xy = coordsRotated + g_Time * g_AnimationSpeed * g_AnimationSpeed + scroll;
	v_TexCoordRipple.zw = coordsRotated2 - g_Time * g_AnimationSpeed * g_AnimationSpeed + scroll;
	v_TexCoordRipple *= g_Scale;

	float rippleTextureAdjustment = (g_Texture0Resolution.x / g_Texture0Resolution.y);
	v_TexCoordRipple.xz *= rippleTextureAdjustment;
	
	v_TexCoord.zw = vec2(v_TexCoord.x * g_Texture2Resolution.z / g_Texture2Resolution.x,
						v_TexCoord.y * g_Texture2Resolution.w / g_Texture2Resolution.y);
}
