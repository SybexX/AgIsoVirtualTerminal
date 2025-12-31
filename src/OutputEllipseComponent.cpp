/*******************************************************************************
** @file       OutputEllipseComponent.cpp
** @author     Adrian Del Grosso
** @copyright  The Open-Agriculture Developers
*******************************************************************************/
#include <memory>

#include "OutputEllipseComponent.hpp"

#include <isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp>
#include <isobus/isobus/isobus_virtual_terminal_objects.hpp>
#include <isobus/isobus/can_constants.hpp>

OutputEllipseComponent::OutputEllipseComponent(std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> workingSet, isobus::OutputEllipse sourceObject) : isobus::OutputEllipse(sourceObject), parentWorkingSet(workingSet)
{
	setSize(get_width(), get_height());
}

void OutputEllipseComponent::paint(juce::Graphics &g)
{
	bool fillNeeded = false;
	bool useLineColourForFill = false;
	isobus::VTColourVector fillColour;
	
	// Ensure we fill first, then draw the outline if needed
	if (get_fill_attributes() != isobus::NULL_OBJECT_ID)
	{
		auto fill_child_object = get_object_by_id(get_fill_attributes(), parentWorkingSet->get_object_tree());

		if ((fill_child_object != nullptr) && (fill_child_object->get_object_type() == isobus::VirtualTerminalObjectType::FillAttributes))
		{
			auto fill_attributes = std::static_pointer_cast<isobus::FillAttributes>(fill_child_object);
			auto fill_type = fill_attributes->get_type();
			
			if (fill_type != isobus::FillAttributes::FillType::NoFill)
			{
				if (fill_type == isobus::FillAttributes::FillType::FillWithSpecifiedColorInFillColorAttribute)
				{
					fillColour = parentWorkingSet->get_colour(fill_attributes->get_background_color());
				}
				else if (fill_type == isobus::FillAttributes::FillType::FillWithLineColor)
				{
					useLineColourForFill = true;
				}
				else if (fill_type == isobus::FillAttributes::FillType::FillWithPatternGivenByFillPatternAttribute)
				{
					// @todo
				}
				fillNeeded = true;
			}
		}
	}

	if (get_line_attributes() != isobus::NULL_OBJECT_ID)
	{
		auto line_child_object = get_object_by_id(get_line_attributes(), parentWorkingSet->get_object_tree());
		if ((line_child_object != nullptr) && (line_child_object->get_object_type() == isobus::VirtualTerminalObjectType::LineAttributes))
		{
			auto line_attributes = std::static_pointer_cast<isobus::LineAttributes>(line_child_object);
			auto lineColour = parentWorkingSet->get_colour(line_attributes->get_background_color());

			float ellipse_width = static_cast<float>(get_width());
			float ellipse_height = static_cast<float>(get_height());
			
			float line_width = static_cast<float>(line_attributes->get_width());
			float line_height = static_cast<float>(line_attributes->get_height());
			
			float start_angle = static_cast<float>(get_start_angle());
			float end_angle = static_cast<float>(get_end_angle());
			
			float line_width_center = (line_width / 2.0f);
			float line_height_center = (line_height / 2.0f);

			float centerX = (ellipse_width / 2.0f);
			float centerY = (ellipse_height / 2.0f);

			if (useLineColourForFill)
			{
				fillColour = lineColour;
			}

			if (get_ellipse_type() == isobus::OutputEllipse::EllipseType::Closed)
			{
				if ((start_angle == end_angle) && (get_ellipse_type() != isobus::OutputEllipse::EllipseType::Closed))
				{
					/* B.10 / Table B.31 / Ellipse type / NOTE 2:
					 * If type = closed ellipse segment and start and end angle are the same, a
					 * single line with width = border width shall be drawn from the centre point to the
					 * point on the border defined by the start and end angles.*/
					auto angleRadians = juce::degreesToRadians(-((start_angle * 2.0f) - 90));
					if (angleRadians < 0.0f)
					{
						angleRadians += juce::MathConstants<float>().twoPi;
					}

					auto pointX = (centerX + ((centerX - line_width_center) * std::cos(angleRadians)));
					auto pointY = (centerY - ((centerY - line_height_center) * std::sin(angleRadians)));

					g.setColour(juce::Colour::fromFloatRGBA(lineColour.r, lineColour.g, lineColour.b, 1.0f));
					g.drawLine(centerX, centerY, pointX, pointY, line_width);
				}
				else
				{
					if (fillNeeded)
					{
						g.setColour(juce::Colour::fromFloatRGBA(fillColour.r, fillColour.g, fillColour.b, 1.0f));
						g.fillEllipse(0.0f, 0.0f, ellipse_width, ellipse_height);
					}
					/* If type > 0 (!= Closed) and start and end angles are the same, the ellipse is drawn closed. */
					g.setColour(juce::Colour::fromFloatRGBA(lineColour.r, lineColour.g, lineColour.b, 1.0f));
					g.drawEllipse(line_width_center, line_width_center, (ellipse_width - line_width), (ellipse_height - line_width), line_width);
				}
			}
			else
			{
				// Juce coordinate system 0° is at the Y axis positive, calculating clockwise
				// IsoBus coordinate system 0° is at the X axis positive, calculating counter-clockwise
				float startAngle = juce::degreesToRadians(start_angle * 2.0f);
				float endAngle = juce::degreesToRadians(end_angle * 2.0f);

				juce::Path arcPath;
				if (get_ellipse_type() == isobus::OutputEllipse::EllipseType::ClosedEllipseSegment)
				{
					// segment: the ellipse section endpoints connected to the center with two lines
					arcPath.startNewSubPath(centerX, centerY);
				}

				addArcToPath(arcPath, line_width_center, line_width_center, (ellipse_width - line_width), (ellipse_height - line_width), startAngle, endAngle, (get_ellipse_type() != isobus::OutputEllipse::EllipseType::ClosedEllipseSegment));

				if (get_ellipse_type() == isobus::OutputEllipse::EllipseType::ClosedEllipseSegment)
				{
					// segment: the ellipse section endpoints connected to the center with two lines
					arcPath.lineTo(centerX, centerY);
					if (fillNeeded)
					{
						g.setColour(juce::Colour::fromFloatRGBA(fillColour.r, fillColour.g, fillColour.b, 1.0f));
						g.fillPath(arcPath);
					}

					g.setColour(juce::Colour::fromFloatRGBA(lineColour.r, lineColour.g, lineColour.b, 1.0f));
					g.strokePath(arcPath, juce::PathStrokeType(line_width));
				}
				else if (get_ellipse_type() == isobus::OutputEllipse::EllipseType::ClosedEllipseSection)
				{
					// section: the ellipse section endpoints connected with a straight line
					arcPath.closeSubPath();
					if (fillNeeded)
					{
						g.setColour(juce::Colour::fromFloatRGBA(fillColour.r, fillColour.g, fillColour.b, 1.0f));
						g.fillPath(arcPath);
					}

					g.setColour(juce::Colour::fromFloatRGBA(lineColour.r, lineColour.g, lineColour.b, 1.0f));
					g.strokePath(arcPath, juce::PathStrokeType(line_width));
				}
				else if (get_ellipse_type() == isobus::OutputEllipse::EllipseType::OpenDefinedByStartEndAngles)
				{
					g.setColour(juce::Colour::fromFloatRGBA(lineColour.r, lineColour.g, lineColour.b, 1.0f));
					g.strokePath(arcPath, juce::PathStrokeType(line_width));
				}
			}
		}
	}
}

/**
 * @brief OutputEllipseComponent::addArcToPath
 * Method to draw ellipse segments to keep the angle between the start and end accurate.
 * Juce Path::addArc does not keep the angle when drawing.
 */
void OutputEllipseComponent::addArcToPath(juce::Path &path, float x, float y, float w, float h, float fromRadians, float toRadians, bool startAsNewSubPath) const
{
	float halfWidth = w / 2.0f;
	float halfHeight = h / 2.0f;
	bool first = true;
	
	if (toRadians < fromRadians)
	{
		toRadians += juce::MathConstants<float>().twoPi;
	}

	for (float drawAngle = fromRadians; drawAngle < toRadians; drawAngle += 0.05)
	{
		float angle = std::fmod(drawAngle, juce::MathConstants<float>().twoPi);
		float tanAngle = std::tan(angle);
		float div = std::sqrt((halfHeight * halfHeight) + (halfWidth * halfWidth) * (tanAngle * tanAngle));
		float x_ = (halfWidth * halfHeight) / div;
		float y_ = -(halfWidth * halfHeight * tanAngle) / div;

		if (std::fabs(angle - juce::MathConstants<float>().halfPi) < 1e-6)
		{
			// handle 90°
			x_ = 0.0f;
			y_ = -halfHeight;
		}
		else if (std::fabs(angle - 3.0f * juce::MathConstants<float>().halfPi) < 1e-6)
		{
			// handle 270°
			x_ = 0.0f;
			y_ = halfHeight;
		}
		else if (!(angle < juce::MathConstants<float>().halfPi || angle > 3.0f * juce::MathConstants<float>().halfPi))
		{
			// handle left quadrants 1, 2
			x_ = -x_;
			y_ = -y_;
		}

		x_ += w / 2.0f + x;
		y_ += h / 2.0f + y;

		if (first && startAsNewSubPath)
		{
			first = false;
			path.startNewSubPath(x_, y_);
		}
		else
		{
			path.lineTo(x_, y_);
		}
	}
}
