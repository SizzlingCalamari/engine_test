#version 330 core

// Fragment shaders contain the built in output gl_FragDepth.
// out float gl_FragDepth

void main()
{
    // If this gl_FragDepth is not written to, it will take on the value
    // of gl_FragCoord.z
    // This is what we want for the shadow map texture.
    // Hence, empty fragment shader.
}
