#include "Items.hpp"
#include "core/backend/FiberPool.hpp"

namespace VV2
{
	ImGuiItem::ImGuiItem(std::function<void()> callback) :
	    m_Callback(callback)
	{
	}

	void ImGuiItem::Draw()
	{
		m_Callback();
	}
}