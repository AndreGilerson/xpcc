#ifndef CHECKBOX_HPP_
#define CHECKBOX_HPP_

#include "widget.hpp"

namespace xpcc {

namespace gui {

class CheckboxWidget : public Widget {

public:

	CheckboxWidget(bool initial, Dimension d) :
		Widget(d, true),
		state(initial)
	{
		this->cb_activate = &click_cb;
	}

	void
	render(View* view);

	bool
	getState() { return this->state; }

	void
	setState(bool s) { this->state = s; }

private:
	static void
	click_cb(const InputEvent& ev, Widget* w, void* data);

private:
	bool state;

};

}
}

#endif /* CHECKBOX_HPP_ */
