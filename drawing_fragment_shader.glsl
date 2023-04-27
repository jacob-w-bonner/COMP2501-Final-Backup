// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;
uniform float length;
uniform int tiles;
uniform vec4 col;

void main()
{
    // Sample texture
    vec4 orgCol = texture2D(onetex, uv_interp * tiles);

    /*
    // Draw a circle
    vec2 coord = uv_interp - vec2(0.5, 0.5);
    float r = 0.3;

    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    float d = length(coord);
    float th = 0.05;
    float s = 1.0 - smoothstep(r - th, r + th, d);
    color = s*vec4(1.0, 0.0, 0.0, 1.0);
    color.a = 1.0;*/
    
    /*//if ((d > (r - th)) && (d < (r + th))){
    if (abs(d - r) < th){
        
    } else if (d < r){
        color = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
        color.a = 0.0;
    }*/

    /*
    // Draw animated line
    vec2 P = vec2(0.5, 0.5);
    //vec2 pv = vec2(1.0, -1.0);
    vec2 pv = vec2(cos(timer), sin(timer));
    pv = normalize(pv);
    vec2 Q = uv_interp;
    float d = abs(dot(Q - P, pv));
    float th = 0.05;
    float s = 1.0 - smoothstep(0, th, d);
    vec4 color = s*vec4(1.0, 0.0, 0.0, 1.0);
    color.a = 1.0;
    gl_FragColor = color;

    // Check for transparency
    if(color.a < 1.0)
    {
         discard;
    }*/

     //Draw energy bar
    vec4 color = texture2D(onetex, uv_interp);

    //float energy = mod(5.9, 1.0);
    float energy = length;
    //vec4 inside_color = vec4(0.1333, 0.6941, 0.3012, 0.0);
    vec4 inside_color = col;
    vec4 outside_color = vec4(0.1333, 0.6941, 0.3012, 0.0);
    if ((uv_interp.x > 0.2) && (uv_interp.x < 0.8) &&
        (uv_interp.y >= (1.0-energy))){
        if (color.r > 0.9){
            gl_FragColor = inside_color;
        } else {
            //gl_FragColor = outside_color;
            //gl_FragColor = vec4(color.r, color.g, color.b, color.a);
            gl_FragColor = inside_color;
        }
    } else {
        /*if (color.r > 0.9){ 
            gl_FragColor = vec4(0.3, 0.3, 0.3, 1.0);
        } else {
            //gl_FragColor = outside_color;
            gl_FragColor = vec4(color.r, color.g, color.b, color.a);
            //gl_FragColor = inside_color;
        }*/
        gl_FragColor = vec4(color.r, color.g, color.b, color.a);
    }

    // Draw tiled circles
    /*float s = smoothstep(0.1, 0.2, length(fract(3.0*uv_interp)-vec2(0.5, 0.5)));
    vec4 color = vec4(s, s, s, 1.0);
    gl_FragColor = color;*/

    // Show texture
    // Sample texture
    /*vec4 color = texture2D(onetex, uv_interp);

    // Assign color to fragment
    gl_FragColor = color;
    if(color.r < 0.5)
    {
         discard;
    }*/
}
