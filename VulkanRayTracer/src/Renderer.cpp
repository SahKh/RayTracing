#include "Renderer.h"

#include "Walnut/Random.h"

#include <cmath>

namespace Utilities {
	static uint32_t ConvertVec4ToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		//bitwise operations to shift values into correct place in memory 
		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}
void Renderer::OnResize(uint32_t width, uint32_t height) {

	if (m_FinalImage) {
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height) { return; }
		m_FinalImage->Resize(width, height);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

}

void Renderer::Render() {
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * -2.0f + 1.0f;
			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utilities::ConvertVec4ToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord) {
	glm::vec3 rayOrigin(0.0f, 0.0f, -1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;

	//rayDirection = glm::normalize(rayDirection);

	// a = ray origin
	// b = ray direction
	// r = radius
	// t = distance

	//calculate ray intersection with object using quadratic formula
	// if there is a solution/intersection then the ray hit
	
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	//Discriminant
	float discriminant = b * b - (4.0f * a * c);

	if (discriminant < 0.0f) {
		return glm::vec4(0, 0, 0, 1);
	}

	float x0 = (-b + glm::sqrt(discriminant) / 2.0f * a); //ray on way out
	float x1 = (-b - glm::sqrt(discriminant) / 2.0f * a);

	glm::vec3 hitPosition = rayOrigin + rayDirection * x1;
	glm::vec3 normal = glm::normalize(hitPosition);
	glm::vec3 lightDirection = glm::normalize(glm::vec3(-1, -1, -1));

	float intensity = glm::max(glm::dot(normal, -lightDirection), 0.0f);

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor = normal * 0.5f + 0.5f;
	sphereColor *= intensity;
	return glm::vec4(sphereColor, 1.0f);
}

// no scaling, need to account for viewport width and height when accounting for -1 to 1 range

