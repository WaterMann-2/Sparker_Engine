//
// Created by robsc on 11/18/25.
//

#include "Shader.h"

namespace fs = std::filesystem;

std::string Shader::shaderExtension(ShaderType type) {
	switch (type) {
		case ShaderType::ShaderVertex:
			return ".vert";
			break;

		case ShaderType::ShaderFragment:
			return ".frag";
			break;
	}
}

void Shader::createShader(const std::string vertexShaderFilePath,
                          const std::string fragmentShaderFilePath,
                          VkDevice device) {

	fs::path vertexShaderPath(vertexShaderFilePath);
	fs::path fragmentShaderPath(fragmentShaderFilePath);

	if (vertexShaderPath.extension() != ".vert") {
		SpConsole::FatalExit("Incorrect file extension. NEEDS TO BE .vert !!!", SP_FAILURE);
	}
	if (fragmentShaderPath.extension() != ".frag") {
		SpConsole::FatalExit("Incorrect file extension. NEEDS TO BE .frag !!!", SP_FAILURE);
	}

	std::string vertexName = vertexShaderPath.stem().string();
	std::string fragmentName = fragmentShaderPath.stem().string();

	compiledCheck(vertexName, ShaderVertex);
	compiledCheck(fragmentName, ShaderFragment);

	recompileDateCheck(vertexName, ShaderVertex);
	recompileDateCheck(fragmentName, ShaderFragment);

	if (mCompileFlags == 0) {
		return;
	}

	if (mCompileFlags & CompileVertex) {
		compileShader(vertexShaderPath);
	}
	if (mCompileFlags & CompileFragment) {
		compileShader(fragmentShaderPath);
	}
}


void Shader::compileShader(std::filesystem::path filePath) {
	//shaderc::Compiler compiler;



}

void Shader::compiledCheck(std::string fileName, ShaderType type) {
	std::string dataFolder = RENDERER_DATA_DIR "/shaders/";
	fs::path shaderPath = dataFolder;

	if (!fs::exists(shaderPath)) {
		SpConsole::Write(SP_MESSAGE_WARNING, ("Creating Directory " + shaderPath.string()).c_str());
		fs::create_directory(shaderPath);
	}
	for (const auto& entry : fs::directory_iterator(shaderPath)) {
		if (fileName.compare(entry.path().filename().string())) {
			SpConsole::Write(SP_MESSAGE_INFO, (fileName + " has been copmpiled").c_str());
			return;
		}
	}

	SpConsole::Write(SP_MESSAGE_VERBOSE, ("Did not find compiled shader for " + fileName).c_str());

	switch (type) {
		case ShaderType::ShaderVertex:
			mCompileFlags = mCompileFlags | CompileVertex;
			break;
		case ShaderType::ShaderFragment:
			mCompileFlags = mCompileFlags | CompileFragment;
			break;
	}

}


void Shader::recompileDateCheck(std::string fileName, ShaderType type) {
	switch (type) {
		case ShaderType::ShaderVertex:
			if (mCompileFlags & CompileVertex) {
				SpConsole::Write(SP_MESSAGE_INFO, fileName + ".vert has not been compiled before!");
				return;
			}
			break; // end case Vertex
		case ShaderType::ShaderFragment:
			if (mCompileFlags & CompileFragment) {
				SpConsole::Write(SP_MESSAGE_INFO, fileName + ".frag has not been compiled before!");
				return;
			}
			break; // end case Fragment
	}

	std::string dataFolder = std::string(RENDERER_DATA_DIR "/shaders/") + fileName.c_str() + SHADER_EXTENSION_COMPILED;
	fs::path compiledShaderPath = dataFolder;

	dataFolder = std::string(RENDERER_RESOURCE_DIR "/shaders/") + fileName.c_str();
	fs::path shaderPath = dataFolder;

	const auto compiledFileTime = std::filesystem::last_write_time(compiledShaderPath);
	const auto fileTime = std::filesystem::last_write_time(shaderPath);

	if (compiledFileTime > fileTime) {
		switch (type) {
			case ShaderType::ShaderVertex:
				mCompileFlags = mCompileFlags | CompileVertex;
				break;
			case ShaderType::ShaderFragment:
				mCompileFlags = mCompileFlags | CompileFragment;
				break;
		}
	}
}
