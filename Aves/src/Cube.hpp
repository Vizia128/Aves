#pragma once

#include <glm/glm.hpp>

namespace Aves {

	class Cube {

	public:
		Cube();
		~Cube();

	private:
		struct cube {
			glm::vec3 pos = glm::vec3(0., 1., 6.);      // position
		};
	};
}