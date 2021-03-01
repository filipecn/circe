#version 440 core

uniform int selected;

void main() {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 0.5);
    if (selected == gl_PrimitiveID)
    gl_FragColor.w = 1.0;
}
