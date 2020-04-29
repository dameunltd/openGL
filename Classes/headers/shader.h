#pragma once

#include <string>
#include <unordered_map> /* this is a hash table (further research) */

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	/* sets uniform */
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
private:	
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int CompileShader(unsigned int type, const std::string& source);

	unsigned int GetUniformLocation(const std::string& name);
};

