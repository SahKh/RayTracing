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

void Renderer::Render(const Camera& camera) {

	Ray ray;
	ray.Origin = camera.GetPosition();
	 
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

			glm::vec4 color = TraceRay(ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utilities::ConvertVec4ToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray) {
	float radius = 0.5f;

	//rayDirection = glm::normalize(rayDirection);

	// a = ray origin
	// b = ray direction
	// r = radius
	// t = distance

	//calculate ray intersection with object using quadratic formula
	// if there is a solution/intersection then the ray hit
	
	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
	float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

	//Discriminant
	float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0.0f) {
		return glm::vec4(1, 1, 1, .8);
	}

	float x0 = (-b + glm::sqrt(discriminant) / 2.0f * a); //ray on way out
	float x1 = (-b - glm::sqrt(discriminant) / 2.0f * a);

	glm::vec3 hitPosition = ray.Origin + ray.Direction * x1;
	glm::vec3 normal = glm::normalize(hitPosition);
	glm::vec3 lightDirection = glm::normalize(glm::vec3(1, 1, 1));

	float intensity = glm::max(glm::dot(normal, -lightDirection), 0.0f);

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor = normal * 0.5f + 0.5f;
	sphereColor *= intensity;

	return glm::vec4(sphereColor, 1.0f); 
}

// no scaling, need to account for viewport width and height when accounting for -1 to 1 range

