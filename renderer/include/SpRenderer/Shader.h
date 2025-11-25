//
// Created by robsc on 11/18/25.
//

#ifndef SPARKER_ENGINE_SHADER_H
#define SPARKER_ENGINE_SHADER_H

#include "Utils.h"

#define SHADER_EXTENSION_COMPILED ".comp"
#define SHADER_EXTENSION_VERTEX ".vert.comp"
#define SHADER_EXTENSION_FRAGMENT ".frag.comp"

class Shader {
private:
	enum ShaderCompileFlags {
		CompileVertex = 0x00000001,
		CompileFragment = 0x00000010
	};
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

	void compileShader(std::filesystem::path filePath);

	void compiledCheck(std::string fileName, std::string fileExtension);
	void recompileDateCheck(std::string fileName, std::string fileExtension);
};


#endif //SPARKER_ENGINE_SHADER_H