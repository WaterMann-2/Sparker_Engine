//
// Created by robsc on 11/18/25.
//

#include "Shader.h"

namespace fs = std::filesystem;

std::string Shader::shaderExtension(ShaderType type) {
	switch (type) {
		case ShaderType::SHADER_VERTEX:
			return ".vert";
			break;

		case ShaderType::SHADER_FRAGMENT:
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

	compiledCheck(vertexName, SHADER_VERTEX);
	compiledCheck(fragmentName, SHADER_FRAGMENT);

	recompileDateCheck(vertexName, SHADER_VERTEX);
	recompileDateCheck(fragmentName, SHADER_FRAGMENT);

	if (mCompileFlags == 0) {
		return;
	}

	if (mCompileFlags & CompileVertex) {
		mVertSPIRV = compileShader(vertexShaderPath);
		writeToFile(vertexName, SHADER_VERTEX);
	}
	if (mCompileFlags & CompileFragment) {
		mFragSPIRV = compileShader(fragmentShaderPath);
		writeToFile(fragmentName, SHADER_FRAGMENT);
	}

	readFromFile(vertexName, SHADER_VERTEX);
	readFromFile(fragmentName, SHADER_FRAGMENT);

}


std::vector<uint32> Shader::compileShader(std::filesystem::path filePath) {
	shaderc::Compiler compiler;
	shaderc::CompileOptions compileOptions;
	shaderc::SpvCompilationResult result;

	if (!compiler.IsValid()) {
		SpConsole::FatalExit("Compiler is not valid!", SP_FAILURE);
	}

	if (!exists(filePath)) {
		SpConsole::FatalExit("File does not exist", SP_FAILURE);
	}

	std::vector<char> rawCode = Utils::FileUtils::readTextFile(filePath);
	size_t codeSize = rawCode.size() * sizeof(char);

	result = compiler.AssembleToSpv(rawCode.data(),  codeSize, compileOptions);

	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::string message = result.GetErrorMessage();
		SpConsole::Write(SP_MESSAGE_FATAL, message);
		SpConsole::FatalExit("Shader compilation error", SP_FAILURE);
	}

	std::vector<uint32> compiledData(result.cbegin(), result.cend());

	return compiledData;
}

void Shader::compiledCheck(std::string fileName, ShaderType type) {
	std::string dataFolder = RENDERER_DATA_DIR "/shaders/";
	fs::path shaderPath = dataFolder;

	if (!fs::exists(shaderPath)) {
		SpConsole::Write(SP_MESSAGE_WARNING, "Creating Directory " + shaderPath.string());
		fs::create_directory(shaderPath);
	}
	for (const auto& entry : fs::directory_iterator(shaderPath)) {
		if (fileName.compare(entry.path().filename().string())) {
			SpConsole::Write(SP_MESSAGE_INFO, fileName + " has been compiled");
			return;
		}
	}

	SpConsole::Write(SP_MESSAGE_VERBOSE, "Did not find compiled shader for " + fileName);

	switch (type) {
		case ShaderType::SHADER_VERTEX:
			mCompileFlags = mCompileFlags | CompileVertex;
			break;
		case ShaderType::SHADER_FRAGMENT:
			mCompileFlags = mCompileFlags | CompileFragment;
			break;
	}

}


void Shader::recompileDateCheck(std::string fileName, ShaderType type) {
	switch (type) {
		case ShaderType::SHADER_VERTEX:
			if (mCompileFlags & CompileVertex) {
				SpConsole::Write(SP_MESSAGE_INFO, fileName + ".vert has not been compiled before!");
				return;
			}
			break; // end case Vertex
		case ShaderType::SHADER_FRAGMENT:
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
			case ShaderType::SHADER_VERTEX:
				mCompileFlags = mCompileFlags | CompileVertex;
				break;
			case ShaderType::SHADER_FRAGMENT:
				mCompileFlags = mCompileFlags | CompileFragment;
				break;
		}
	}
}

void Shader::writeToFile(std::string fileName, ShaderType type) {
	std::vector<char> code = {};
	switch (type) {
		case ShaderType::SHADER_VERTEX:
			if (mVertSPIRV.size() == 0) SpConsole::FatalExit("Vertex shader has not been compiled!", SP_FAILURE);
			code.assign(mVertSPIRV.begin(), mVertSPIRV.end());
			Utils::FileUtils::writeBinaryFile(RENDERER_DATA_DIR "/" + fileName + SHADER_EXTENSION_COMPILED_VERTEX, code);
			break;

		case ShaderType::SHADER_FRAGMENT:
			if (mFragSPIRV.size() == 0) SpConsole::FatalExit("Fragment shader has not been compiled!", SP_FAILURE);
			code.assign(mFragSPIRV.begin(), mFragSPIRV.end());
			Utils::FileUtils::writeBinaryFile(RENDERER_DATA_DIR "/" + fileName + SHADER_EXTENSION_COMPILED_FRAGMENT, code);
			break;
	}
}

std::vector<uint32> Shader::readFromFile(std::string fileName, ShaderType type) {
	std::vector<char> code = {};
	fs::path shaderPath = RENDERER_DATA_DIR "/" + fileName;

	switch (type) {
		case ShaderType::SHADER_VERTEX:
			if (!mVertSPIRV.empty()) {
				SpConsole::Write(SP_MESSAGE_INFO, fileName + ".vert has just been compiled");
				return mVertSPIRV;
			}
			shaderPath = shaderPath.string() + SHADER_EXTENSION_COMPILED_VERTEX;
			if (!fs::exists(shaderPath)) SpConsole::FatalExit(fileName + SHADER_EXTENSION_COMPILED_VERTEX + " does not exist!", SP_FAILURE);

			code = Utils::FileUtils::readBinaryFile(shaderPath.string() + SHADER_EXTENSION_COMPILED_VERTEX);
			return std::vector<uint32>(code.begin(), code.end());
			break;

		case ShaderType::SHADER_FRAGMENT:
			if (!mFragSPIRV.empty()) {
				SpConsole::Write(SP_MESSAGE_INFO, fileName + ".frag has just been compiled");
				return mFragSPIRV;
			}
			shaderPath = shaderPath.string() + SHADER_EXTENSION_COMPILED_FRAGMENT;

			if (!fs::exists(shaderPath)) SpConsole::FatalExit(fileName + SHADER_EXTENSION_COMPILED_FRAGMENT + " does not exist!", SP_FAILURE);

			code = Utils::FileUtils::readTextFile(shaderPath.string() + SHADER_EXTENSION_COMPILED_FRAGMENT);
			return std::vector<uint32>(code.begin(), code.end());
			break;

	}

	SpConsole::FatalExit("You're not supposed to be able to get here???", SP_FAILURE);
	return {};
}
