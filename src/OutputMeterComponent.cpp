/*******************************************************************************
** @file       OutputMeterComponent.cpp
** @author     Adrian Del Grosso
** @copyright  The Open-Agriculture Developers
*******************************************************************************/
#include <cmath>
#include <memory>
#include <cstdint>

#include "OutputMeterComponent.hpp"

#include <isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp>
#include <isobus/isobus/isobus_virtual_terminal_objects.hpp>
#include <isobus/isobus/can_constants.hpp>

OutputMeterComponent::OutputMeterComponent(std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> workingSet, isobus::OutputMeter sourceObject) : isobus::OutputMeter(sourceObject), parentWorkingSet(workingSet)
{
	setSize(get_width(), get_height());
}

void OutputMeterComponent::paint(juce::Graphics &g)
{
	float object_width = static_cast<float>(get_width());
	float object_height = static_cast<float>(get_height());
	
	float endVtAngleDeg = (get_end_angle() * 2.0f);
	float startVtAngleDeg = (get_start_angle() * 2.0f);
	
	if (get_option(isobus::OutputMeter::Options::DrawBorder))
	{
		auto border_colour = parentWorkingSet->get_colour(get_border_colour());
		g.setColour(juce::Colour::fromFloatRGBA(border_colour.r, border_colour.g, border_colour.b, 1.0f));
		
		// Draws a rectangular outline, using the current colour or brush.
		// drawRect(float x, float y, float width, float height, float lineThickness = 1.0f)
		// The lines are drawn inside the given rectangle, and greater line thicknesses extend inwards.
		g.drawRect(0.0f, 0.0f, object_width, object_height, 1.0f);
	}
	
	if (get_option(isobus::OutputMeter::Options::DrawArc))
	{
		juce::Path meterPath;
		juce::PathStrokeType pathStroke(1.0f, juce::PathStrokeType::JointStyle::curved);

		float startVtAngle = (startVtAngleDeg * 0.0174533f);
		float endVtAngle = (endVtAngleDeg * 0.0174533f);
		float ellipseRotation = (3.14159f / 2.0f);

		if (endVtAngle < startVtAngle)
		{
			endVtAngle += (2.0f * 3.14159f);
		}

		if (startVtAngle < endVtAngle)
		{
			ellipseRotation = -ellipseRotation;
		}

		meterPath.addCentredArc((object_width / 2.0f), (object_height / 2.0f), (object_width / 2.0f) - 1, (object_height / 2.0f) - 1, ellipseRotation, startVtAngle, endVtAngle, true);
		g.setColour(juce::Colours::black);
		g.strokePath(meterPath, pathStroke);
	}

	std::uint32_t needleValue = get_value();
	if (get_variable_reference() != isobus::NULL_OBJECT_ID)
	{
		auto varNum_child_object = get_object_by_id(get_variable_reference(), parentWorkingSet->get_object_tree());
		if ((varNum_child_object != nullptr) && (varNum_child_object->get_object_type() == isobus::VirtualTerminalObjectType::NumberVariable))
		{
			auto number_variable = std::static_pointer_cast<isobus::NumberVariable>(varNum_child_object);
			needleValue = number_variable->get_value();
		}
	}

	if (endVtAngleDeg < startVtAngleDeg)
	{
		endVtAngleDeg += (360);
	}

	float theta = ((static_cast<float>(needleValue) / get_max_value()) * (startVtAngleDeg - endVtAngleDeg));
	float needleEndAngle = 0.0f;

	if (get_option(isobus::OutputMeter::Options::DeflectionDirection) == true)
	{
		// clockwise
		needleEndAngle = (endVtAngleDeg + theta);
	}
	else
	{
		// counter clockwise
		needleEndAngle = (endVtAngleDeg - theta);
	}

	float xOffset = (((object_width / 2.0f) - 1) * std::cos(needleEndAngle * 3.14159265f / 180.0f));
	float yOffset = (-((object_width / 2.0f) - 1) * std::sin(needleEndAngle * 3.14159265f / 180.0f));

	auto needle_colour = parentWorkingSet->get_colour(get_needle_colour());
	g.setColour(Colour::fromFloatRGBA(needle_colour.r, needle_colour.g, needle_colour.b, 1.0f));

	// Draws a line with an arrowhead at its end.
	// drawArrow(Line<float> line, float lineThickness, float arrowheadWidth, float arrowheadLength)
	juce::Line<float> line(juce::Point<float>((object_width / 2.0f), (object_height / 2.0f)) , juce::Point<float>(((object_width / 2.0f) + xOffset), ((object_width / 2.0f) + yOffset)));
	g.drawArrow(line, 5.0f, 5.0f, ((object_width / 2.0f) - 6));
	
	if (get_option(isobus::OutputMeter::Options::DrawTicks) && (get_number_of_ticks() > 0))
	{
		g.setColour(juce::Colours::black);

		auto numberOfTicks = static_cast<int>(get_number_of_ticks());
		float degreesPerTick = ((startVtAngleDeg - endVtAngleDeg) / (numberOfTicks - 1));

		for (int i = 0; i < numberOfTicks; i++)
		{
			if (get_option(isobus::OutputMeter::Options::DeflectionDirection))
			{
				// clockwise
				needleEndAngle = (startVtAngleDeg + (degreesPerTick * i));
			}
			else
			{
				// counter clockwise
				needleEndAngle = (endVtAngleDeg - (degreesPerTick * i));
			}

			float xCoord_start = (((object_width / 2.0f) - 1) * std::cos(needleEndAngle * 3.14159265f / 180.0f));
			float yCoord_start = (((object_width / 2.0f) - 1) * std::sin(needleEndAngle * 3.14159265f / 180.0f));

			float xCoord_end = (((object_width / 2.0f) - 6) * std::cos(needleEndAngle * 3.14159265f / 180.0f));
			float yCoord_end = (((object_width / 2.0f) - 6) * std::sin(needleEndAngle * 3.14159265f / 180.0f));

			g.drawLine(((object_width / 2.0f) + xCoord_start), ((object_height / 2.0f) + yCoord_start), ((object_width / 2.0f) + xCoord_end), ((object_height / 2.0f) + yCoord_end), 1.0f);
		}
	}
}
