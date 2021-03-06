GL_GLSL(

attribute vec2 position;
attribute vec2 extent;
attribute float rotation;
attribute float index;
attribute float life;

varying highp vec2 v_texCoords;
varying highp vec4 v_color;

uniform highp mat3 projectionMatrix;
uniform highp mat3 transformMatrix;

// :todo: use samplers for the modifiers.

uniform highp vec2 scaleTable[8]; 
uniform highp int scaleCount;

uniform highp vec4 colorTable[8];
uniform highp int colorCount;

vec2 getInterpolatedVec2(in vec2 table[8], in int count, in float factor)
{
    int lower;
    int upper;
    float f = factor * float(count - 1);

    lower = int(floor(f));
    upper = int(ceil(f));

    f = f - float(lower);

    vec2 result;
    vec2 upper_value;
    vec2 lower_value;

    for (int i = 0; i < 8; i++) { if (i == lower) { lower_value = table[i]; } }
    for (int i = 0; i < 8; i++) { if (i == upper) { upper_value = table[i]; } }

    result = mix(lower_value, upper_value, f);

    return result;
}

vec4 getInterpolatedVec4(in vec4 table[8], in int count, in float factor)
{
    int lower;
    int upper;
    float f = factor * float(count - 1);

    lower = int(floor(f));
    upper = int(ceil(f));

    f = f - float(lower);

    vec4 result;
    vec4 upper_value;
    vec4 lower_value;

    for (int i = 0; i < 8; i++) { if (i == lower) { lower_value = table[i]; } }
    for (int i = 0; i < 8; i++) { if (i == upper) { upper_value = table[i]; } }

    result = mix(lower_value, upper_value, f);

    return result;
}

void main()
{
    vec2 finalPosition;
    int i = int(index);

    vec2 halfExtent = extent * getInterpolatedVec2(scaleTable, scaleCount, life) * 0.5;
    vec4 color = getInterpolatedVec4(colorTable, colorCount, life);

    float c = cos(rotation);
    float s = sin(rotation);

    if(i == 0)
    {
        finalPosition.x = position.x - halfExtent.x * c - halfExtent.y * s;
        finalPosition.y = position.y - halfExtent.x * s + halfExtent.y * c;
        v_texCoords = vec2(0, 0);
    }
    else if(i == 1)
    {
        finalPosition.x = position.x + halfExtent.x * c - halfExtent.y * s;
        finalPosition.y = position.y + halfExtent.x * s + halfExtent.y * c;
        v_texCoords = vec2(1, 0);
    }
    else if(i == 2)
    {
        finalPosition.x = position.x + halfExtent.x * c + halfExtent.y * s;
        finalPosition.y = position.y + halfExtent.x * s - halfExtent.y * c;
        v_texCoords = vec2(1, 1);
    }
    else if(i == 3)
    {
        finalPosition.x = position.x - halfExtent.x * c + halfExtent.y * s;
        finalPosition.y = position.y - halfExtent.x * s - halfExtent.y * c;
        v_texCoords = vec2(0, 1);
    }

    v_color = color;

    vec3 res = transformMatrix * vec3(finalPosition, 1.0) * projectionMatrix;
    gl_Position = vec4(res, 1.0);
}

);