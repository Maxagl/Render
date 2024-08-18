#include "Assets/TextRenderer.h"
#include <iostream>

TextRenderer::TextRenderer(std::string text, std::string font, int size, glm::vec3 color, GLuint program)
{
    this->text = text;
    this->color = color;
    this->scale = 1.0f;
    this->program = program;
    this->setPosition(position);
    
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(800), 0.0f, static_cast<GLfloat>(600));
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    FT_Library ft;

    if(FT_Init_FreeType(&ft))
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    FT_Face face;
    if(FT_New_Face(ft, font.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE:Failed to load font" << std::endl;
    }
    FT_Set_Pixel_Sizes(face, 0, size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte i = 0; i < 128; ++i)
    {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), 
                                        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), 
                                        face->glyph->advance.x};
        Characters.insert(std::pair<GLchar, Character>(i, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 参数没太看懂，复习下
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
TextRenderer::~TextRenderer()
{}

void TextRenderer::draw()
{
    glm::vec2 textPos = this->position;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(program);
    glUniform3f(glGetUniformLocation(program, "textColor"), this->color.x, this->color.y, this->color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); ++c)
    {
        Character ch = Characters[*c];

        GLfloat xpos = textPos.x + ch.Bearing.x * this->scale;
        GLfloat ypos = textPos.y - (ch.Size.y - ch.Bearing.y) * this->scale;

        GLfloat w = ch.Size.x * this->scale;
        GLfloat h = ch.Size.y * this->scale;

        GLfloat vertices[6][4] = {
            {xpos, ypos + h, 0.0, 0.0},
            {xpos, ypos, 0.0, 1.0},
            {xpos + w, ypos, 1.0, 1.0},

            {xpos, ypos + h, 0.0, 0.0},
            {xpos + w, ypos, 1.0, 1.0},
            {xpos + w, ypos + h, 1.0, 0.0}
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        textPos.x += (ch.Advance >> 6) * this->scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);

}
void TextRenderer::setPosition(glm::vec2 _position)
{
    this->position = _position;
}
void TextRenderer::setText(std::string _text)
{
    this->text = _text;
}