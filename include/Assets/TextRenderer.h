#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "freetypefont/ft2build.h"
#include FT_FREETYPE_H

#include <string>
#include <map>

struct Character
{
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};

class TextRenderer
{
public:
    TextRenderer(std::string text, std::string font, int size, glm::vec3 color, GLuint program);
    ~TextRenderer();

    void draw();
    void setPosition(glm::vec2 _position);
    void setText(std::string _text);

private:
    std::string text;
    GLfloat scale;
    glm::vec3 color;
    glm::vec2 position;

    GLuint VAO, VBO, program;
    std::map<GLchar, Character> Characters;
};

