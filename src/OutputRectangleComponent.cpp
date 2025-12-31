/*******************************************************************************
** @file       OutputRectangleComponent.cpp
** @author     Adrian Del Grosso
** @copyright  The Open-Agriculture Developers
*******************************************************************************/
#include <cstdint>
#include <memory>

#include "OutputRectangleComponent.hpp"

#include <isobus/isobus/isobus_virtual_terminal_objects.hpp>

OutputRectangleComponent::OutputRectangleComponent(std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> workingSet, isobus::OutputRectangle sourceObject) : isobus::OutputRectangle(sourceObject), parentWorkingSet(workingSet)
{
	setSize(get_width(), get_height());
}

void OutputRectangleComponent::paint(juce::Graphics &g)
{
	float line_thickness = 1.0f;
	auto fill_type = isobus::FillAttributes::FillType::NoFill;
	auto line_colour = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	auto fill_colour = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);

	float rectangle_width = static_cast<float>(get_width());
	float rectangle_height = static_cast<float>(get_height());
	bool anyLineSuppressed = (get_line_suppression_bitfield() != 0);

	// Resets the current colour, brush, and font to default settings.
	g.resetToDefaultState();

	if (get_line_attributes() != isobus::NULL_OBJECT_ID)
	{
		auto line_child_object = get_object_by_id(get_line_attributes(), parentWorkingSet->get_object_tree());
		if ((line_child_object != nullptr) && (line_child_object->get_object_type() == isobus::VirtualTerminalObjectType::LineAttributes))
		{
			auto line_attributes = std::static_pointer_cast<isobus::LineAttributes>(line_child_object);
			line_thickness = static_cast<float>(line_attributes->get_width());
			auto temp_line_colour = parentWorkingSet->get_colour(line_attributes->get_background_color());
			line_colour = juce::Colour::fromFloatRGBA(temp_line_colour.r, temp_line_colour.g, temp_line_colour.b, 1.0f);
		}
	}

	if (get_fill_attributes() != isobus::NULL_OBJECT_ID)
	{
		auto fill_child_object = get_object_by_id(get_fill_attributes(), parentWorkingSet->get_object_tree());
		if ((fill_child_object != nullptr) && (fill_child_object->get_object_type() == isobus::VirtualTerminalObjectType::FillAttributes))
		{
			auto fill_attributes = std::static_pointer_cast<isobus::FillAttributes>(fill_child_object);
			fill_type = fill_attributes->get_type();
			auto temp_fill_colour = parentWorkingSet->get_colour(fill_attributes->get_background_color());
			fill_colour = juce::Colour::fromFloatRGBA(temp_fill_colour.r, temp_fill_colour.g, temp_fill_colour.b, 1.0f);
		}
	}

	if (fill_type == isobus::FillAttributes::FillType::FillWithLineColor)
	{
		// Changes the current drawing colour.
		g.setColour(line_colour);

		g.fillRect(0.0f, 0.0f, rectangle_width, rectangle_height);
	}
	else if (fill_type == isobus::FillAttributes::FillType::FillWithSpecifiedColorInFillColorAttribute)
	{
		// Changes the current drawing colour.
		g.setColour(fill_colour);

		g.fillRect(0.0f, 0.0f, rectangle_width, rectangle_height);
	}
	else if (fill_type == isobus::FillAttributes::FillType::FillWithPatternGivenByFillPatternAttribute)
	{
		// @todo
		juce::Rectangle<float> house(0.0f, 0.0f, rectangle_width, rectangle_height);
		g.fillCheckerBoard(house, 10, 10, juce::Colours::white, juce::Colours::black);
	}

	if (line_thickness > 0.0f)
	{
		// Resets the current colour, brush, and font to default settings.
		g.resetToDefaultState();

		// Changes the current drawing colour.
		g.setColour(line_colour);
				
		if (!anyLineSuppressed)
		{
			// Draws a rectangular outline, using the current colour or brush.
			// drawRect(float x, float y, float width, float height, float lineThickness = 1.0f)
			// The lines are drawn inside the given rectangle, and greater line thicknesses extend inwards.
			g.drawRect(0.0f, 0.0f, rectangle_width, rectangle_height, line_thickness);
		}
		else
		{
			// Something is suppressed
			if (((0x01 << static_cast<std::uint8_t>(isobus::OutputRectangle::LineSuppressionOption::SuppressTopLine)) == 0) & get_line_suppression_bitfield())
			{
				// Draws a line between two points with a given thickness.
				// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
				g.drawLine(0.0f, 0.0f, rectangle_width, 0.0f, line_thickness);
			}
					
			if (((0x01 << static_cast<std::uint8_t>(isobus::OutputRectangle::LineSuppressionOption::SuppressLeftSideLine)) == 0) & get_line_suppression_bitfield())
			{
				// Draws a line between two points with a given thickness.
				// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
				g.drawLine(0.0f, 0.0f, 0.0f, rectangle_height, line_thickness);
			}
					
			if (((0x01 << static_cast<std::uint8_t>(isobus::OutputRectangle::LineSuppressionOption::SuppressRightSideLine)) == 0) & get_line_suppression_bitfield())
			{
				// Draws a line between two points with a given thickness.
				// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
				g.drawLine(rectangle_width, 0.0f, rectangle_width, rectangle_height, line_thickness);
			}
					
			if (((0x01 << static_cast<std::uint8_t>(isobus::OutputRectangle::LineSuppressionOption::SuppressBottomLine)) == 0) & get_line_suppression_bitfield())
			{
				// Draws a line between two points with a given thickness.
				// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
				g.drawLine(0.0f, rectangle_height, rectangle_width, rectangle_height, line_thickness);
			}
		}
	}
}
