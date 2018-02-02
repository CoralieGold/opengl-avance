#version 330 core

in vec3 vNormal_vs;

out vec3 fFragColor;

void main() {
  fFragColor = vNormal_vs;
};