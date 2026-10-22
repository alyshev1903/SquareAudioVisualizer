#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float zOffset;
layout(location = 3) in vec3 aColor;
layout(location = 4) in float aPointSize;

out vec2 v_TexCoord;
out vec3 v_Color;
out float v_Depth;

uniform mat4 u_Model;
uniform mat4 u_Camera;
uniform int  u_Pass;
uniform float u_SizeScale;
uniform float u_Pale;

const float REF_DISTANCE = 500.0;
const float GLOW_SCALE = 3.0;
const float MAX_PALE = 0.85;

void main()
{
    vec3 pos = position;
    pos.z += zOffset;

    gl_Position = u_Camera * u_Model * vec4(pos, 1.0);

    float size = aPointSize * u_SizeScale * (REF_DISTANCE / max(gl_Position.w, 1.0));
    if (u_Pass == 1)
        size *= GLOW_SCALE;
    gl_PointSize = clamp(size, 1.0, 64.0);

    v_TexCoord = texCoord;
    v_Color = mix(aColor, vec3(1.0), u_Pale * MAX_PALE);
    v_Depth = gl_Position.w;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Color;
in float v_Depth;

uniform vec4      u_Color;
uniform sampler2D u_Texture;
uniform int       u_UseTexture;
uniform int       u_Pass;
uniform float       u_Alpha;

const float GLOW_STRENGTH = 0.15;

void main()
{
    vec2 p = gl_PointCoord * 2.0 - 1.0;
    float r2 = dot(p, p);
    if (r2 > 1.0)
        discard;

    float r = sqrt(r2);

    if (u_Pass == 1)
    {
        if (u_UseTexture == 1)
            discard;

        float falloff = 1.0 - r;
        color = vec4(v_Color, GLOW_STRENGTH * falloff * falloff * u_Alpha);
        return;
    }

    vec3 normal = vec3(p.x, -p.y, sqrt(1.0 - r2));
    vec3 lightDir = normalize(vec3(-0.4, 0.5, 0.75));
    vec3 halfVec = normalize(lightDir + vec3(0.0, 0.0, 1.0));

    float diffuse = max(dot(normal, lightDir), 0.0);
    float specular = pow(max(dot(normal, halfVec), 0.0), 32.0);

    vec3 base = v_Color;
    float baseAlpha = 1.0;
    float shade = 0.35 + 0.65 * diffuse;

    if (u_UseTexture == 1)
    {
        vec4 tex = texture(u_Texture, v_TexCoord);
        base = tex.rgb;
        baseAlpha = tex.a;
        shade = 0.6 + 0.4 * diffuse;
    }

    vec3 lit = base * shade + vec3(0.35 * specular);

    float fog = smoothstep(500.0, 620.0, v_Depth);
    lit *= 1.0 - 0.5 * fog;

    float edge = 1.0 - smoothstep(0.9, 1.0, r);
    color = vec4(lit, edge * baseAlpha * u_Alpha);
}