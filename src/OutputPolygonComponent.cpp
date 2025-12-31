/*******************************************************************************
** @file       OutputPolygonComponent.cpp
** @author     Adrian Del Grosso
** @copyright  The Open-Agriculture Developers
*******************************************************************************/
#include <cstdint>
#include <memory>

#include "OutputPolygonComponent.hpp"

#include <isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp>
#include <isobus/isobus/isobus_virtual_terminal_objects.hpp>
#include <isobus/isobus/can_constants.hpp>

OutputPolygonComponent::OutputPolygonComponent(std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> workingSet, isobus::OutputPolygon sourceObject) : isobus::OutputPolygon(sourceObject), parentWorkingSet(workingSet)
{
	setSize(get_width(), get_height());
}

void OutputPolygonComponent::paint(juce::Graphics &g)
{
    auto number_of_points = get_number_of_points() - 1;
	
	// 3 Points MUST exist or the object cannot be drawn
	if (number_of_points >= 2)
	{
		juce::Path polygonPath;

		float line_thickness = 1.0f;
		auto fill_type = isobus::FillAttributes::FillType::NoFill;
		auto line_colour = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);
		auto fill_colour = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);

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

		for (std::uint8_t i = 0; i <= number_of_points; i++)
		{
			auto thisPoint = get_point(static_cast<std::uint8_t>(i));

			if (line_thickness > 1.0f)
			{
				thisPoint.xValue = (thisPoint.xValue + (line_thickness / 2.0f));
				thisPoint.yValue = (thisPoint.yValue + (line_thickness / 2.0f));
			}

			if (i == 0)
			{
				// Begins a new subpath with a given starting position. 
				// polygonPath.startNewSubPath(static_cast<float>(thisPoint.xValue), static_cast<float>(thisPoint.yValue));
				polygonPath.startNewSubPath(juce::Point<float>(thisPoint.xValue, thisPoint.yValue));
			}
			else
			{
				// draws a line from the current position up to, but not including, the specified point.
				// polygonPath.lineTo(static_cast<float>(thisPoint.xValue), static_cast<float>(thisPoint.yValue));
				polygonPath.lineTo(juce::Point<float>(thisPoint.xValue, thisPoint.yValue));
			}
		}

		// only closed polygons can be filled
		if (get_type() != isobus::OutputPolygon::PolygonType::Open)
		{
			// Appends a line from the current point to the starting point of the current subpath and ends the subpath.
			polygonPath.closeSubPath();

			// Resets the current colour, brush, and font to default settings.
			g.resetToDefaultState();

			if (fill_type == isobus::FillAttributes::FillType::FillWithLineColor)
			{
				// Changes the current fill settings (a fill type of a solid colour).
				g.setFillType(juce::FillType(line_colour));
				
				// Fills a path using the currently selected colour or brush.
				g.fillPath(polygonPath);
			}
			else if (fill_type == isobus::FillAttributes::FillType::FillWithSpecifiedColorInFillColorAttribute)
			{
				// Changes the current fill settings (a fill type of a solid colour).
				g.setFillType(juce::FillType(fill_colour));
				
				// Fills a path using the currently selected colour or brush.
				g.fillPath(polygonPath);
			}
			else if (fill_type == isobus::FillAttributes::FillType::FillWithPatternGivenByFillPatternAttribute)
			{
				// @ TODO support gradient fill
				// FillType (const ColourGradient &gradient)
				// ColourGradient (Colour colour1, float x1, float y1, Colour colour2, float x2, float y2, bool isRadial)

				// Changes the current fill settings (a fill type of a solid colour).
				g.setFillType(juce::FillType(line_colour));

				// Fills a path using the currently selected colour or brush.
				g.fillPath(polygonPath);
			}
		}

		// Resets the current colour, brush, and font to default settings.
		g.resetToDefaultState();

		// Changes the current drawing colour.
		g.setColour(line_colour);

		// and draw it to a graphics context with a lineWidth thick outline
		// JointStyle
		//     mitered : Indicates that corners should be drawn with sharp joints.
		//     curved  : Indicates that corners should be drawn as rounded - off.
		//     beveled : Indicates that corners should be drawn with a line flattening their outside edge.
		// EndCapStyle
		//     butt    : Ends of lines are flat and don't extend beyond the end point.
		//     square  : Ends of lines are flat, but stick out beyond the end point for half the thickness of the stroke.
		//     rounded : Ends of lines are rounded-off with a circular shape.
		g.strokePath(polygonPath, juce::PathStrokeType(line_thickness, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::square));
		// g.strokePath(polygonPath, juce::PathStrokeType(line_thickness));
	}
}
