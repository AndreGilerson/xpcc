// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#include "view_stack.hpp"

// ----------------------------------------------------------------------------
xpcc::gui::GuiViewStack::GuiViewStack(xpcc::GraphicDisplay* display, xpcc::gui::inputQueue* queue) :
	ViewStack(display),
	input_queue(queue)
{
}

// ----------------------------------------------------------------------------
xpcc::gui::GuiViewStack::~GuiViewStack()
{
}

// ----------------------------------------------------------------------------
void
xpcc::gui::GuiViewStack::pop()
{
	xpcc::gui::View *topElement = this->stack.get();
	this->stack.pop();
	
	delete topElement;
}

// ----------------------------------------------------------------------------
void
xpcc::gui::GuiViewStack::update()
{
	xpcc::gui::View* top = this->get();

	if(top == NULL)
		return;

	top->update();
	if (top->isAlive())
	{
		if (top->hasChanged())
		{
			top->draw();
			this->display->update();
		}
	}
	else
	{
		// Remove old view
		top->onRemove();
		this->pop();
		
		// Get new screen
		top = this->get();
		top->markDirty();	// Only difference to xpcc::ViewStack::update()
		top->update();
		this->display->clear();
		top->draw();
		this->display->update();
	}
}
