#include "Items.hpp"

namespace VV2
{
	CollapsingHeaderItem::CollapsingHeaderItem(const std::string& name) :
	    m_Name(name)
	{
	}

	void CollapsingHeaderItem::Draw()
	{
		if (ImGui::CollapsingHeader(m_Name.c_str()))
		{
			for (auto& item : m_Items)
			{
				if (item->CanDraw())
					item->Draw();
			}
		}
	}
}