type:<nsstring>nsparticle_render_shaderstage:35632</nsstring>
fragment.type:<uint32>5632</uint32>
fragment.source:<nsstring>#version 440

in vec2 texCoord;
in float alphaOut;

uniform sampler2D diffuseMap;
uniform bool colorMode;
uniform vec4 fragColOut;
uniform bool hasDiffuseMap = false;
uniform float lifetimeSecs;
uniform uint blendMode;

layout (location = 0) out vec4 colorOut;

void main()
{
	vec4 difCol = fragColOut;
	if (!colorMode && hasDiffuseMap)
		difCol = texture(diffuseMap, texCoord);

	if (blendMode == 0)
		difCol.a *= alphaOut;
	else
		difCol *= alphaOut;

	colorOut = difCol;
}




</nsstring>
