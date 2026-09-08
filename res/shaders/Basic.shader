#shader vertex
#version 330 core 
        
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord; 
layout(location = 2) in float zOffset;
layout(location = 3) in vec3 aColor; 
layout(location = 4) in float aPointSize;

out vec2 v_TexCoord;
out vec3 v_Color;

uniform mat4 u_Model;
uniform mat4 u_Camera;
      
void main()
{
    vec3 pos  = position;
    pos.z    += zOffset;

    gl_Position  = u_Camera * u_Model * vec4(pos, 1.0);
    gl_PointSize = aPointSize;
    v_TexCoord   = texCoord;
    v_Color      = aColor;
}

#shader fragment
#version 330 core
        
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Color;

uniform vec4      u_Color;
uniform sampler2D u_Texture;
uniform int       u_UseTexture;
        
void main()
{
    vec2 centered = gl_PointCoord - vec2(0.5);
    float dist = length(centered);
    if (dist > 0.5)
        discard;

    float alpha = 1.0 - smoothstep(0.45, 0.5, dist);
    
    if (u_UseTexture == 1)
    {
        vec4 texColor = texture(u_Texture, v_TexCoord);
        color = vec4(texColor.rgb, alpha * texColor.a);
    }
    else
    {
        color = vec4(v_Color, alpha);
    }
}