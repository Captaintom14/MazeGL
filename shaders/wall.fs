#version 330 core 

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;  // Fixed: Changed sampler2d to sampler2D

void main(){
    FragColor = texture(texture1, TexCoord);
}
