/*******************************************************************************
** @file SoftKeyMaskComponent.hpp
**
** @brief Defines a GUI component to draw a soft key mask.
** @author Adrian Del Grosso
**
** @copyright 2023 Adrian Del Grosso
*******************************************************************************/
#pragma once

#ifndef SOFT_KEY_MASK_COMPONENT_HPP
#define SOFT_KEY_MASK_COMPONENT_HPP

#include "isobus/isobus/isobus_virtual_terminal_objects.hpp"
#include "isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp"

#include "JuceHeader.h"

class SoftKeyMaskDimensions
{
public:
	SoftKeyMaskDimensions() = default;

	/**
	 * @brief total_width
	 * @return The total width of the softkeymask (including inner and outer column paddings)
	 */
	int total_width() const;

	/**
   * @brief total_height
   * @return The total height of the softkeymask (including inner and outer column paddings)
   */
	int total_height() const;

	/**
	 * @brief key_count
	 * @return the number of the possible key positions in the softkey mask
	 */
	int key_count() const;

	int height = 480;

	int keyHeight = 60;
	int keyWidth = 60;

	int keyColumnCount = 1;
	int keyRowCount = 6;

	// Orientation Values:
	// Top                                    =   0
	// Bottom                                 =   1
	// Left                                   =   2
	// Right                                  =   3
	// LeftLeft                               =   4
	// LeftRight                              =   5
	// RightRight                             =   6
	// TopTop                                 =   7
	// TopBottom                              =   8
	// BottomBottom                           =   9
	std::uint8_t keyOrientation = 6;

	// Order Values:
	// TopLeft                                =   0
	// TopRight                               =   1
	// BottomLeft                             =   2
	// BottomRight                            =   3
	std::uint8_t keyOrder = 1;

	std::uint8_t keyPadding = 0;
};

class SoftKeyMaskComponent : public isobus::SoftKeyMask, public Component
{
public:
	SoftKeyMaskComponent(std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> workingSet, isobus::SoftKeyMask sourceObject, SoftKeyMaskDimensions dimensions);

	void on_content_changed(bool initial = false);

	void paint(Graphics &g) override;

private:
	std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> parentWorkingSet;
	std::vector<std::shared_ptr<Component>> childComponents;
	SoftKeyMaskDimensions dimensionInfo;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoftKeyMaskComponent)
};

#endif // SOFT_KEY_MASK_COMPONENT_HPP
