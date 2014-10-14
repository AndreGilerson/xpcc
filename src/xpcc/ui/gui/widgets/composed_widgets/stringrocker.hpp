#ifndef STRINGROCKER_HPP_
#define STRINGROCKER_HPP_

#include "../widget.hpp"
#include "../button.hpp"
#include "../stringfield.hpp"

namespace xpcc {

namespace gui {


class StringRocker : public WidgetGroup {

	typedef const char* (*toString)(uint32_t);

public:

	StringRocker(uint32_t string_id, uint32_t start, uint32_t end, toString string_function, Dimension d) :
		WidgetGroup(d),
		string_id(string_id),
		start(start),
		end(end),
		string_function(string_function),
		button_next(true, Dimension(d.height, d.height)),
		button_previous(false, Dimension(d.height, d.height)),
		string_field("", Dimension(d.width - 2*d.height, d.height))
	{
		button_next.cb_deactivate = &next_cb;
		button_previous.cb_deactivate = &previous_cb;

		string_field.setValue(this->getValue());

		this->pack(&button_next, xpcc::glcd::Point(d.width - d.height,0));
		this->pack(&button_previous, xpcc::glcd::Point(0, 0));
		this->pack(&string_field, xpcc::glcd::Point(d.height, 0));

	}

	void
	next();

	void
	previous();

	void
	activate(const InputEvent& ev, void* data);

	void
	deactivate(const InputEvent& ev, void* data);

	const char*
	getValue()
	{
		if(string_function == NULL) {
			return "";
		}

		return string_function(string_id);
	}

	uint32_t
	getId() const
	{
		return this->string_id;
	}

private:
	static void
	next_cb(const InputEvent& ev, Widget* w, void* data);

	static void
	previous_cb(const InputEvent& ev, Widget* w, void* data);

private:
	uint32_t string_id;
	uint32_t start, end;
	toString string_function;
	ArrowButton button_next, button_previous;
	StringField string_field;

};

}
}

#endif /* STRINGROCKER_HPP_ */
