#pragma once

#include "Rect.h"
#include "Component.h"

namespace dae {

	class CBoundary : public Component {

	public:
		CBoundary(const Rect& boundary);
		void Update()override;

	private:
		Rect m_Boundary;
	};
}