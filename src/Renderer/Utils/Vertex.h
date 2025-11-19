//
// Created by robsc on 11/18/25.
//

#ifndef SPARKER_ENGINE_VERTEX_H
#define SPARKER_ENGINE_VERTEX_H

#include "Utils.h"

struct Vertex2D {
	vec2 position;
	vec2 texCoord;
	vec3 color;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getBindingDescriptions();
};

struct Vertex {
	vec3 position;
	vec2 texCoord;
	vec3 color;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getBindingDescriptions();
};


#endif //SPARKER_ENGINE_VERTEX_H