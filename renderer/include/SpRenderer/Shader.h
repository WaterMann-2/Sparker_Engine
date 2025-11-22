//
// Created by robsc on 11/18/25.
//

#ifndef SPARKER_ENGINE_SHADER_H
#define SPARKER_ENGINE_SHADER_H

#include "Utils.h"

class Shader {
public:
	struct ShaderContext {
		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;
	};


	/**
	 *
	 * @param vertexShaderFilename Required .vert file extension
	 * @param fragmentShaderFilename Required .frag file extension
	 */
	void createShader(const std::vector<char> vertexShaderFilename, const std::vector<char> fragmentShaderFilename, VkDevice device);

	ShaderContext getShaderContext();


private:
	VkShaderModule VertexModule;
	VkShaderModule FragmentModule;

	VkDevice mDevice;
};


#endif //SPARKER_ENGINE_SHADER_H