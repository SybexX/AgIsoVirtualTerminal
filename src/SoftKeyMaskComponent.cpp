/*******************************************************************************
** @file       SoftKeyMaskComponent.cpp
** @author     Adrian Del Grosso
** @copyright  The Open-Agriculture Developers
*******************************************************************************/
#include "SoftKeyMaskComponent.hpp"
#include "JuceManagedWorkingSetCache.hpp"

#include "SoftKeyMaskRenderAreaComponent.hpp"

SoftKeyMaskComponent::SoftKeyMaskComponent(std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> workingSet, isobus::SoftKeyMask sourceObject, SoftKeyMaskDimensions dimensions) :
  isobus::SoftKeyMask(sourceObject), parentWorkingSet(workingSet), dimensionInfo(dimensions)
{
	setOpaque(true);
	setBounds(0, 0, dimensions.total_width(), dimensions.height);
	on_content_changed(true);
}

void SoftKeyMaskComponent::on_content_changed(bool initial)
{
	int row = 0;
	int x = ((dimensionInfo.keyColumnCount - 1) * (dimensionInfo.keyWidth + dimensionInfo.keyPadding)) + dimensionInfo.keyPadding;
	int y = dimensionInfo.keyPadding;

	for (std::uint16_t i = 0; i < this->get_number_children(); i++)
	{
		auto child = get_object_by_id(get_child_id(i), parentWorkingSet->get_object_tree());

		if (nullptr != child)
		{
			childComponents.push_back(JuceManagedWorkingSetCache::create_component(parentWorkingSet, child));

			if (isobus::VirtualTerminalObjectType::ObjectPointer == child->get_object_type())
			{
				childComponents.back()->setSize(dimensionInfo.keyWidth, dimensionInfo.keyHeight);
			}

			if (nullptr != childComponents.back())
			{
				addAndMakeVisible(*childComponents.back());
				childComponents.back()->setTopLeftPosition(x, y);
				y += (dimensionInfo.keyWidth + dimensionInfo.keyPadding);

				row++;

				if (row >= dimensionInfo.keyRowCount)
				{
					row = 0;
					x -= (dimensionInfo.keyWidth + dimensionInfo.keyPadding);
					y = dimensionInfo.keyPadding;
				}
			}
		}
	}

	if (!initial)
	{
		repaint();
	}
}

void SoftKeyMaskComponent::paint(Graphics &g)
{
	auto vtColour = parentWorkingSet->get_colour(backgroundColor);

	g.fillAll(Colour::fromFloatRGBA(vtColour.r, vtColour.g, vtColour.b, 1.0f));
}

int SoftKeyMaskDimensions::key_count() const
{
	return keyColumnCount * keyRowCount;
}

int SoftKeyMaskDimensions::total_width() const
{
	return (keyColumnCount * (keyWidth + keyPadding)) + keyPadding;
}

int SoftKeyMaskDimensions::total_height() const
{
	return (keyRowCount * (keyHeight + keyPadding)) + keyPadding;
}
