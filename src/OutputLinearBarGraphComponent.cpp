/*******************************************************************************
** @file       OutputLinearBarGraphComponent.cpp
** @author     Adrian Del Grosso
** @copyright  The Open-Agriculture Developers
*******************************************************************************/
#include <memory>

#include "OutputLinearBarGraphComponent.hpp"

#include <isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp>
#include <isobus/isobus/isobus_virtual_terminal_objects.hpp>
#include <isobus/isobus/can_constants.hpp>

OutputLinearBarGraphComponent::OutputLinearBarGraphComponent(std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> workingSet, isobus::OutputLinearBarGraph sourceObject) : isobus::OutputLinearBarGraph(sourceObject), parentWorkingSet(workingSet)
{
	setSize(get_width(), get_height());
}

void OutputLinearBarGraphComponent::paint(juce::Graphics &g)
{
	float minValue = static_cast<float>(get_min_value());
	float maxValue = static_cast<float>(get_max_value());

	float value = static_cast<float>(get_value());
	float targetValue = static_cast<float>(get_target_value());

	auto numberOfTicks = (static_cast<int>(get_number_of_ticks()) - 1);

	float valueRatioToMax = (value / maxValue);
	float targetRatioToMax = (targetValue / maxValue);
	
	auto vtBackgroundColour = parentWorkingSet->get_colour(get_colour());
	auto vtTargetLineColour = parentWorkingSet->get_colour(get_target_line_colour());
	
	float object_width = static_cast<float>(get_width());
	float object_height = static_cast<float>(get_height());
	
	float bar_graph_width = static_cast<float>(getWidth());
	float bar_graph_height = static_cast<float>(getHeight());

	auto ticks_width = (bar_graph_width / numberOfTicks);
	auto ticks_height = (bar_graph_height / numberOfTicks);
	
	g.setColour(juce::Colour::fromFloatRGBA(vtBackgroundColour.r, vtBackgroundColour.g, vtBackgroundColour.b, 1.0f));

	if (get_option(isobus::OutputLinearBarGraph::Options::DrawBorder))
	{
		// Draws a rectangular outline, using the current colour or brush.
		// drawRect(float x, float y, float width, float height, float lineThickness = 1.0f)
		// The lines are drawn inside the given rectangle, and greater line thicknesses extend inwards.
		g.drawRect(0.0f, 0.0f, bar_graph_width, bar_graph_height, 1.0f);
	}

	if (get_variable_reference() != isobus::NULL_OBJECT_ID)
	{
		auto varNum_child_object = get_object_by_id(get_variable_reference(), parentWorkingSet->get_object_tree());
		if ((varNum_child_object != nullptr) && (varNum_child_object->get_object_type() == isobus::VirtualTerminalObjectType::NumberVariable))
		{
			auto number_variable = std::static_pointer_cast<isobus::NumberVariable>(varNum_child_object);
			valueRatioToMax = (static_cast<float>(number_variable->get_value()) / static_cast<float>(get_max_value()));
		}
	}

	// Figure out what kind of bar graph we are
	if (get_option(isobus::OutputLinearBarGraph::Options::BarGraphType))
	{
		// Not filled, but has value line
		if (get_option(isobus::OutputLinearBarGraph::Options::AxisOrientation))
		{
			// X Axis
			if (get_option(isobus::OutputLinearBarGraph::Options::Direction))
			{
				// From left
				// Draws a line between two points with a given thickness.
				// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
				g.drawLine((object_width * valueRatioToMax), 0.0f, (object_width * valueRatioToMax), object_height, 3.0f);

				if (get_option(isobus::OutputLinearBarGraph::Options::DrawTargetLine))
				{
					g.setColour(juce::Colour::fromFloatRGBA(vtTargetLineColour.r, vtTargetLineColour.g, vtTargetLineColour.b, 1.0f));
					// Draws a vertical line of pixels at a given x position.
					// drawVerticalLine(int x, float top, float bottom)
					g.drawVerticalLine((object_width * targetRatioToMax), 0.0f, object_height);
				}
			}
			else
			{
				// From right
				// Draws a line between two points with a given thickness.
				// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
				g.drawLine((object_width * (1.0f - valueRatioToMax)), 0.0f, (object_width * (1.0f - valueRatioToMax)), object_height, 3.0f);

				if (get_option(isobus::OutputLinearBarGraph::Options::DrawTargetLine))
				{
					g.setColour(juce::Colour::fromFloatRGBA(vtTargetLineColour.r, vtTargetLineColour.g, vtTargetLineColour.b, 1.0f));
					// Draws a vertical line of pixels at a given x position.
					// drawVerticalLine(int x, float top, float bottom)
					g.drawVerticalLine((object_width * (1.0f - targetRatioToMax)), 0.0f, object_height);
				}
			}

			if (get_option(isobus::OutputLinearBarGraph::Options::DrawTicks))
			{
				g.setColour(juce::Colour::fromFloatRGBA(vtBackgroundColour.r, vtBackgroundColour.g, vtBackgroundColour.b, 1.0f));

				auto tick_height = (object_height / 3);
				if (tick_height > 6)
				{
					tick_height = 6;
				}
				else if (tick_height < 2)
				{
					tick_height = 2;
				}

				for (int i = 0; i < (numberOfTicks - 1); i++)
				{
					// Draws a line between two points with a given thickness.
					// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
					g.drawLine((ticks_width + (i * ticks_width)), 0.0f, (ticks_width + (i * ticks_width)), tick_height, 1.0f);

					// Draws a line between two points with a given thickness.
					// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
					g.drawLine((ticks_width + (i * ticks_width)), (object_height - tick_height), (ticks_width + (i * ticks_width)), object_height, 1.0f);
				}
			}
		}
		else
		{
			// Y Axis
			if (get_option(isobus::OutputLinearBarGraph::Options::Direction))
			{
				// From bottom
				// Draws a line between two points with a given thickness.
				// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
				g.drawLine(0, ((1.0f - valueRatioToMax) * bar_graph_height), bar_graph_width, ((1.0f - valueRatioToMax) * bar_graph_height), 3.0f);

				if (get_option(isobus::OutputLinearBarGraph::Options::DrawTargetLine))
				{
					g.setColour(juce::Colour::fromFloatRGBA(vtTargetLineColour.r, vtTargetLineColour.g, vtTargetLineColour.b, 1.0f));
					// Draws a horizontal line of pixels at a given y position.
					// drawHorizontalLine(int y, float left, float right);
					g.drawHorizontalLine((object_height * (1.0f - targetRatioToMax)), 0.0f, object_width);
				}
			}
			else
			{
				// From top
				// Draws a line between two points with a given thickness.
				// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
				g.drawLine(0, (valueRatioToMax * bar_graph_height), bar_graph_width, (valueRatioToMax * bar_graph_height), 3.0f);

				if (get_option(isobus::OutputLinearBarGraph::Options::DrawTargetLine))
				{
					g.setColour(juce::Colour::fromFloatRGBA(vtTargetLineColour.r, vtTargetLineColour.g, vtTargetLineColour.b, 1.0f));
					// Draws a horizontal line of pixels at a given y position.
					// drawHorizontalLine(int y, float left, float right);
					g.drawHorizontalLine((object_height * targetRatioToMax), 0.0f, object_width);
				}
			}

			if (get_option(isobus::OutputLinearBarGraph::Options::DrawTicks))
			{
				g.setColour(juce::Colour::fromFloatRGBA(vtBackgroundColour.r, vtBackgroundColour.g, vtBackgroundColour.b, 1.0f));

				auto tick_width = (object_width / 3);
				if (tick_width > 6)
				{
					tick_width = 6;
				}
				else if (tick_width < 2)
				{
					tick_width = 2;
				}

				for (int i = 0; i < (numberOfTicks - 1); i++)
				{
					// Draws a line between two points with a given thickness.
					// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
					g.drawLine(0.0f, (ticks_height + (i * ticks_height)), tick_width, (ticks_height + (i * ticks_height)), 1.0f);

					// Draws a line between two points with a given thickness.
					// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
					g.drawLine((object_width - tick_width), (ticks_height + (i * ticks_height)), object_width, (ticks_height + (i * ticks_height)), 1.0f);
				}
			}
		}
	}
	else
	{
		// Filled
		if (get_option(isobus::OutputLinearBarGraph::Options::AxisOrientation))
		{
			// X Axis
			if (get_option(isobus::OutputLinearBarGraph::Options::Direction))
			{
				// From left
				g.fillRect(0.0f, 0.0f, object_width * valueRatioToMax, object_height);

				if (get_option(isobus::OutputLinearBarGraph::Options::DrawTargetLine))
				{
					g.setColour(juce::Colour::fromFloatRGBA(vtTargetLineColour.r, vtTargetLineColour.g, vtTargetLineColour.b, 1.0f));
					// Draws a vertical line of pixels at a given x position.
					// drawVerticalLine(int x, float top, float bottom)
					g.drawVerticalLine((object_width * targetRatioToMax), 0.0f, object_height);
				}
			}
			else
			{
				// From right
				g.fillRect((object_width * (1.0f - valueRatioToMax)), 0.0f, (object_width * valueRatioToMax), object_height);

				if (get_option(isobus::OutputLinearBarGraph::Options::DrawTargetLine))
				{
					g.setColour(juce::Colour::fromFloatRGBA(vtTargetLineColour.r, vtTargetLineColour.g, vtTargetLineColour.b, 1.0f));
					// Draws a vertical line of pixels at a given x position.
					// drawVerticalLine(int x, float top, float bottom)
					g.drawVerticalLine((object_width * (1.0f - targetRatioToMax)), 0.0f, object_height);
				}
			}

			if (get_option(isobus::OutputLinearBarGraph::Options::DrawTicks))
			{
				g.setColour(juce::Colour::fromFloatRGBA(vtBackgroundColour.r, vtBackgroundColour.g, vtBackgroundColour.b, 1.0f));

				auto tick_height = (object_height / 3);
				if (tick_height > 6)
				{
					tick_height = 6;
				}
				else if (tick_height < 2)
				{
					tick_height = 2;
				}

				for (int i = 0; i < (numberOfTicks - 1); i++)
				{
					// Draws a line between two points with a given thickness.
					// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
					g.drawLine((ticks_width + (i * ticks_width)), 0.0f, (ticks_width + (i * ticks_width)), tick_height, 1.0f);

					// Draws a line between two points with a given thickness.
					// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
					g.drawLine((ticks_width + (i * ticks_width)), (object_height - tick_height), (ticks_width + (i * ticks_width)), object_height, 1.0f);
				}
			}
		}
		else
		{
			// Y Axis
			if (get_option(isobus::OutputLinearBarGraph::Options::Direction))
			{
				// From bottom
				g.fillRect(0.0f, (object_height * (1.0f - valueRatioToMax)), object_width, (object_height * valueRatioToMax));

				if (get_option(isobus::OutputLinearBarGraph::Options::DrawTargetLine))
				{
					g.setColour(juce::Colour::fromFloatRGBA(vtTargetLineColour.r, vtTargetLineColour.g, vtTargetLineColour.b, 1.0f));
					// Draws a horizontal line of pixels at a given y position.
					// drawHorizontalLine(int y, float left, float right);
					g.drawHorizontalLine((object_height * (1.0f - targetRatioToMax)), 0.0f, object_width);
				}
			}
			else
			{
				// From top
				g.fillRect(0.0f, 0.0f, object_width, (object_height * valueRatioToMax));

				if (get_option(isobus::OutputLinearBarGraph::Options::DrawTargetLine))
				{
					g.setColour(juce::Colour::fromFloatRGBA(vtTargetLineColour.r, vtTargetLineColour.g, vtTargetLineColour.b, 1.0f));
					// Draws a horizontal line of pixels at a given y position.
					// drawHorizontalLine(int y, float left, float right);
					g.drawHorizontalLine((object_height * targetRatioToMax), 0.0f, object_width);
				}
			}

			if (get_option(isobus::OutputLinearBarGraph::Options::DrawTicks))
			{
				g.setColour(juce::Colour::fromFloatRGBA(vtBackgroundColour.r, vtBackgroundColour.g, vtBackgroundColour.b, 1.0f));

				auto tick_width = (object_width / 3);
				if (tick_width > 6)
				{
					tick_width = 6;
				}
				else if (tick_width < 2)
				{
					tick_width = 2;
				}

				for (int i = 0; i < (numberOfTicks - 1); i++)
				{
					// Draws a line between two points with a given thickness.
					// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
					g.drawLine(0.0f, (ticks_height + (i * ticks_height)), tick_width, (ticks_height + (i * ticks_height)), 1.0f);

					// Draws a line between two points with a given thickness.
					// drawLine(float startX, float startY, float endX, float endY, float lineThickness)
					g.drawLine((object_width - tick_width), (ticks_height + (i * ticks_height)), object_width, (ticks_height + (i * ticks_height)), 1.0f);
				}
			}
		}
	}
}
