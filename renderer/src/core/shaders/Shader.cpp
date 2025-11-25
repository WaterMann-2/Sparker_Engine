//
// Created by robsc on 11/18/25.
//

#include "Shader.h"

namespace fs = std::filesystem;

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

	compiledCheck(vertexName, vertexShaderPath.extension().string());
	compiledCheck(fragmentName, fragmentShaderPath.extension().string());

	recompileDateCheck(vertexName, vertexShaderPath.extension().string());
	recompileDateCheck(fragmentName, fragmentShaderPath.extension().string());

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

}

void Shader::compiledCheck(std::string fileName, std::string fileExtension) {
	std::string dataFolder = RENDERER_DATA_DIR "/shaders/";
	fs::path shaderPath = dataFolder;

	if (!fs::exists(shaderPath)) {
		SpConsole::Write(SP_MESSAGE_ERROR, ("Path to " + shaderPath.string() + " does not exist!").c_str());
		return;
	}
	for (const auto& entry : fs::directory_iterator(shaderPath)) {
		if (entry.path().stem().string() == fileName) {
			return;
		}
	}

	if (fileExtension.compare(".vert")) {
		mCompileFlags = mCompileFlags | CompileVertex;
	}else if (fileExtension.compare(".frag")) {
		mCompileFlags = mCompileFlags | CompileFragment;
	}

}


void Shader::recompileDateCheck(std::string fileName, std::string fileExtension) {
	std::string dataFolder = std::string(RENDERER_DATA_DIR "/shaders/") + fileName.c_str() + SHADER_EXTENSION_COMPILED;
	fs::path compiledShaderPath = dataFolder;

	dataFolder = std::string(RENDERER_RESOURCE_DIR "/shaders/") + fileName.c_str();
	fs::path shaderPath = dataFolder;

	const auto compiledFileTime = std::filesystem::last_write_time(compiledShaderPath);
	const auto fileTime = std::filesystem::last_write_time(shaderPath);

	if (compiledFileTime > fileTime) {
		if (fileExtension.compare(".vert")) {
			mCompileFlags = mCompileFlags | CompileVertex;
		}else if (fileExtension.compare(".frag")) {
			mCompileFlags = mCompileFlags | CompileFragment;
		}
	}
}
