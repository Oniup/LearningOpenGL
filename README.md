# Learning OpenGL

Followed [LearnOpenGL](https://learnopengl.com) tutorial by [Joey de Vries](http://joeydevries.com/#home)

## Template that most examples use

```cpp
#include <glad/glad.h>

#include "Common/Context.h"

int main(int argc, char** argv)
{
    Cm::Context context(PROJECT_NAME, 600, 600);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT);

    // Only if you want ImGui to be enabled and usable in the runtime loop
    context.EnableImGui();

    // Setup code ...

    // Runtime loop
    while (context.BeginFrame())
    {
        // Draw here ...

        context.EndFrame();
    }
    return 0;
}
```
