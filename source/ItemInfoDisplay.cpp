/* ItemInfoDisplay.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "ItemInfoDisplay.h"

#include "Color.h"
#include "FillShader.h"
#include "FontSet.h"
#include "GameData.h"
#include "Screen.h"
#include "Table.h"

#include <algorithm>

using namespace std;

namespace {
	static const int HOVER_TIME = 60;
}



ItemInfoDisplay::ItemInfoDisplay()
{
	description.SetAlignment(WrappedText::JUSTIFIED);
	description.SetWrapWidth(WIDTH - 20);
	description.SetFont(FontSet::Get(14));
	
	hoverText.SetAlignment(WrappedText::JUSTIFIED);
	hoverText.SetWrapWidth(WIDTH - 20);
	hoverText.SetFont(FontSet::Get(14));
}



// Get the panel width.
int ItemInfoDisplay::PanelWidth()
{
	return WIDTH;
}



// Get the height of each of the three panels.
int ItemInfoDisplay::MaximumHeight() const
{
	return maximumHeight;
}



int ItemInfoDisplay::DescriptionHeight() const
{
	return descriptionHeight;
}



int ItemInfoDisplay::AttributesHeight() const
{
	return attributesHeight;
}



// Draw each of the panels.
void ItemInfoDisplay::DrawDescription(const Point &topLeft) const
{
	description.Draw(topLeft + Point(10., 12.), *GameData::Colors().Get("medium"));
}



void ItemInfoDisplay::DrawAttributes(const Point &topLeft) const
{
	Draw(topLeft, attributeLabels, attributeValues);
}



void ItemInfoDisplay::DrawTooltips() const
{
	if(!hoverCount || hoverCount-- < HOVER_TIME || !hoverText.Height())
		return;
	
	Point textSize(hoverText.WrapWidth(), hoverText.Height());
	Point boxSize = textSize + Point(20., 20.);
	
	Point topLeft = hoverPoint;
	if(topLeft.X() + boxSize.X() > Screen::Right())
		topLeft.X() -= boxSize.X();
	if(topLeft.Y() + boxSize.Y() > Screen::Bottom())
		topLeft.Y() -= boxSize.Y();
	
	FillShader::Fill(topLeft + .5 * boxSize, boxSize, Color(.2, 1.));
	hoverText.Draw(topLeft + Point(10., 10.), Color(.5, 0.));
}



// Update the location where the mouse is hovering.
void ItemInfoDisplay::Hover(const Point &point)
{
	hoverPoint = point;
}



void ItemInfoDisplay::UpdateDescription(const string &text)
{
	description.Wrap(text);
	
	// Pad by 10 pixels on the top and bottom.
	descriptionHeight = description.Height() + 20;
}



Point ItemInfoDisplay::Draw(Point point, const vector<string> &labels, const vector<string> &values) const
{
	// Add ten pixels of padding at the top.
	point.Y() += 10.;
	
	// Get standard colors to draw with.
	Color labelColor = *GameData::Colors().Get("medium");
	Color valueColor = *GameData::Colors().Get("bright");
	
	Table table;
	// Use 10-pixel margins on both sides.
	table.AddColumn(10, Table::LEFT);
	table.AddColumn(WIDTH - 10, Table::RIGHT);
	table.SetHighlight(0, WIDTH);
	table.DrawAt(point);
	
	for(unsigned i = 0; i < labels.size() && i < values.size(); ++i)
	{
		if(labels[i].empty())
		{
			table.DrawGap(10);
			continue;
		}
		
		CheckHover(table, labels[i]);
		table.Draw(labels[i], values[i].empty() ? valueColor : labelColor);
		table.Draw(values[i], valueColor);
	}
	return table.GetPoint();
}



void ItemInfoDisplay::CheckHover(const Table &table, const string &label) const
{
	Point distance = hoverPoint - table.GetCenterPoint();
	Point radius = .5 * table.GetRowSize();
	if(abs(distance.X()) < radius.X() && abs(distance.Y()) < radius.Y())
	{
		hoverCount += 2 * (label == hover);
		hover = label;
		if(hoverCount >= HOVER_TIME)
		{
			hoverCount = HOVER_TIME;
			hoverText.Wrap(GameData::Tooltip(label));
		}
	}
}
