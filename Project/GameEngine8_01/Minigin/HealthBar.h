#pragma once

#include "Component.h"
#include <Windows.h>
#include "Rect.h"

namespace dae {

	struct HealthBarData {
		COLORREF Color{};
		Rect Rect{};
	};

	class CHealthBar : public Component {

	public:
		CHealthBar(const HealthBarData& healthBarData);
		void Start()override;
		void Render()const override;

	private:
		HealthBarData m_HealthBarData;
		std::shared_ptr<float> m_Percentage;
	};
}