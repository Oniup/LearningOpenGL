#include "Common/Shader.h"

namespace Cm
{
    constexpr unsigned int g_InfoLogLength = 512;

    Shader::Shader()
        : m_GpuId(std::numeric_limits<unsigned int>::max())
    {
    }

    Shader::Shader(const std::vector<std::string_view>& paths)
    {
        std::vector<std::pair<ShaderStage, std::string>> sources;
        for (const std::string_view& path : paths)
            sources.push_back(ReadSource(path));
        Create(sources);
    }

    Shader::~Shader()
    {
        Destroy();
    }

    Shader::Shader(Shader&& shader)
        : m_GpuId(shader.m_GpuId)
    {
        shader.m_GpuId = std::numeric_limits<unsigned int>::max();
    }

    Shader& Shader::operator=(Shader&& shader)
    {
        m_GpuId = shader.m_GpuId;
        shader.m_GpuId = std::numeric_limits<unsigned int>::max();
        return *this;
    }

    void Shader::Bind()
    {
        glUseProgram(m_GpuId);
    }

    void Shader::Destroy()
    {
        if (m_GpuId != std::numeric_limits<unsigned int>::max())
        {
            glDeleteProgram(m_GpuId);
            m_GpuId = std::numeric_limits<unsigned int>::max();
        }
    }

    void Shader::Create(const std::vector<std::pair<ShaderStage, std::string>>& sources)
    {
        Destroy();
        std::vector<unsigned int> shaders;
        for (auto&[stage, source] : sources)
            shaders.push_back(CreateShaderInstance(stage, source));
        CreateProgram(shaders);
    }

    void Shader::Create(const std::vector<std::pair<ShaderStage, std::string_view>>& sources)
    {
        Destroy();
        std::vector<unsigned int> shaders;
        for (auto&[stage, source] : sources)
            shaders.push_back(CreateShaderInstance(stage, source));
        CreateProgram(shaders);
    }

    void Shader::UniformI(const std::string_view& location, int val)
    {
        glUniform1i(glGetUniformLocation(m_GpuId, location.data()), val);
    }

    void Shader::UniformI2(const std::string_view& location, const glm::ivec2& vec)
    {
        glUniform2iv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformI3(const std::string_view& location, const glm::ivec3& vec)
    {
        glUniform3iv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformI4(const std::string_view& location, const glm::ivec4& vec)
    {
        glUniform4iv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformU(const std::string_view& location, unsigned int val)
    {
        glUniform1ui(glGetUniformLocation(m_GpuId, location.data()), val);
    }

    void Shader::UniformU2(const std::string_view& location, const glm::uvec2& vec)
    {
        glUniform2uiv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformU3(const std::string_view& location, const glm::uvec3& vec)
    {
        glUniform3uiv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformU4(const std::string_view& location, const glm::uvec4& vec)
    {
        glUniform4uiv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformF(const std::string_view& location, float val)
    {
        glUniform1f(glGetUniformLocation(m_GpuId, location.data()), val);
    }

    void Shader::UniformF2(const std::string_view& location, const glm::vec2& vec)
    {
        glUniform2fv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformF3(const std::string_view& location, const glm::vec3& vec)
    {
        glUniform3fv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformF4(const std::string_view& location, const glm::vec4& vec)
    {
        glUniform4fv(glGetUniformLocation(m_GpuId, location.data()), 1, &vec[0]);
    }

    void Shader::UniformMat2(const std::string_view& location, glm::mat2 mat)
    {
        glUniformMatrix2fv(glGetUniformLocation(m_GpuId, location.data()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::UniformMat3(const std::string_view& location, glm::mat3 mat)
    {
        glUniformMatrix3fv(glGetUniformLocation(m_GpuId, location.data()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::UniformMat4(const std::string_view& location, glm::mat4 mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_GpuId, location.data()), 1, GL_FALSE, &mat[0][0]);
    }

    std::pair<ShaderStage, std::string> Shader::ReadSource(const std::string_view& path)
    {
        std::FILE* file = std::fopen(path.data(), "rb");
        if (!file)
        {
            std::cerr << "Shader source file path doesn't exit \"" << path << "\"\n";
            std::abort();
        }
        std::fseek(file, 0, SEEK_END);
        int length = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);

        std::string source;
        source.resize(length);
        std::fread(source.data(), 1, source.size() - 1, file);
        std::fclose(file);

        ShaderStage stage = ShaderStage::Invalid;
        size_t extOffset = path.find_last_of('.');
        if (extOffset == std::string::npos)
        {
            std::cerr << "Shader file must have a file extension \"" << path << "\"\n";
            std::abort();
        }
        std::string_view ext = path.substr(extOffset);
        for (size_t i = 0; i < ShaderFileExtensions.size(); ++i)
        {
            if (ext == ShaderFileExtensions[i])
            {
                stage = (ShaderStage)i;
                break;
            }
        }
        if (stage == ShaderStage::Invalid)
        {
            std::cerr << "Invalid shader path extension \"" << path << "\"\n";
            std::abort();
        }
        return std::pair{stage, std::move(source)};
    }

    int Shader::ShaderStageToOpenGL(ShaderStage stage) const
    {
        switch (stage)
        {
        case ShaderStage::Vertex:
            return GL_VERTEX_SHADER;
        case ShaderStage::Fragment:
            return GL_FRAGMENT_SHADER;
        case ShaderStage::Geometry:
            return GL_GEOMETRY_SHADER;
        case ShaderStage::Compute:
            return GL_COMPUTE_SHADER;
        default:
            std::abort();
        }
    }

    std::string_view Shader::ShaderStageToString(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::Vertex:
            return "Vertex";
        case ShaderStage::Fragment:
            return "Fragment";
        case ShaderStage::Geometry:
            return "Geometry";
        case ShaderStage::Compute:
            return "Compute";
        default:
            return "Invalid";
        }
    }

    unsigned int Shader::CreateShaderInstance(ShaderStage stage, const std::string_view& source)
    {
        unsigned int glType = ShaderStageToOpenGL(stage);
        const char* src = source.data();
        unsigned int shader = glCreateShader(glType);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[g_InfoLogLength];
            glGetShaderInfoLog(shader, g_InfoLogLength, nullptr, infoLog);
            std::cerr << "Failed to compile " << ShaderStageToString(stage) << " Shader:\n" << infoLog << "\n";
            std::abort();
        }
        return shader;
    }

    void Shader::CreateProgram(const std::vector<unsigned int>& shaders)
    {
        unsigned int program = glCreateProgram();
        for (unsigned int shader : shaders)
            glAttachShader(program, shader);
        glLinkProgram(program);

        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[g_InfoLogLength];
            glGetProgramInfoLog(program, g_InfoLogLength, nullptr, infoLog);
            std::cerr << "Failed to link shader program's shaders: " << infoLog << "\n";
            std::abort();
        }
        for (unsigned int shader : shaders)
            glDeleteShader(shader);
        m_GpuId = program;
    }
}
