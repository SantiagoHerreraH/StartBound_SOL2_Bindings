#include "Boundary.h"
#include "GameObject.h"

namespace dae {



	CBoundary::CBoundary(const Rect& boundary) : m_Boundary(boundary)
	{
	}

	void CBoundary::Update()
	{
		if (Owner().Transform().GetWorldTransform().Position.x < m_Boundary.Left)
		{
			Owner().Transform().SetLocalPositionX(m_Boundary.Right());
		}
		else if (Owner().Transform().GetWorldTransform().Position.x > m_Boundary.Right())
		{
			Owner().Transform().SetLocalPositionX(m_Boundary.Left);
		}

		if (Owner().Transform().GetWorldTransform().Position.y < m_Boundary.Top)
		{
			Owner().Transform().SetLocalPositionY(m_Boundary.Bottom());
		}
		else if (Owner().Transform().GetWorldTransform().Position.y > m_Boundary.Bottom())
		{
			Owner().Transform().SetLocalPositionY(m_Boundary.Top);
		}
	}

}