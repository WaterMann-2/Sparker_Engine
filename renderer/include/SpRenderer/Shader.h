//
// Created by robsc on 11/18/25.
//

#ifndef SPARKER_ENGINE_SHADER_H
#define SPARKER_ENGINE_SHADER_H

#include "Utils.h"
#include "shaderc/shaderc.hpp"

#define SHADER_EXTENSION_COMPILED ".comp"
#define SHADER_EXTENSION_COMPILED_VERTEX ".vert.comp"
#define SHADER_EXTENSION_COMPILED_FRAGMENT ".frag.comp"

class Shader {
private:
	enum ShaderCompileFlags {
		CompileVertex = 0x00000001,
		CompileFragment = 0x00000010
	};

	enum ShaderType {
		SHADER_VERTEX,
		SHADER_FRAGMENT
	};

	std::string shaderExtension(ShaderType type);
public:
	struct ShaderContext {
		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;
	};

	/**
	 *
	 * @param vertexShaderFilePath Required .vert file extension
	 * @param fragmentShaderFilePath Required .frag file extension
	 */
	void createShader(const std::string vertexShaderFilePath, const std::string fragmentShaderFilePath, VkDevice device);

	ShaderContext getShaderContext();


private:
	ShaderContext mShaderContext;

	VkDevice mDevice;

	uint8 mCompileFlags = 0x00000000;

	std::vector<uint32> mVertSPIRV = {};
	std::vector<uint32> mFragSPIRV = {};

	std::vector<uint32> compileShader(std::filesystem::path filePath);

	void compiledCheck(std::string fileName, ShaderType type);
	void recompileDateCheck(std::string fileName, ShaderType type);
	void writeToFile(std::string fileName, ShaderType type);
	std::vector<uint32> readFromFile(std::string fileName, ShaderType type);
};


#endif //SPARKER_ENGINE_SHADER_H