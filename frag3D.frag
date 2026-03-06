#version 330 core
in vec2 Texcoord;

uniform sampler2D texture;

out vec4 FragColor;

void main()
{
    // �e�N�X�`�����g���ĐF���o�͂���悤�C��
    FragColor = texture2D(texture, Texcoord);
}