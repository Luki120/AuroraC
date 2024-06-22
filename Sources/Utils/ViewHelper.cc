#include "ViewHelper.hpp"

id anchorViewToView(id view, id toView, const char *anchor) {

	return msg_with_type<id>(
		msg(view, sel_getUid(anchor)),
		sel_getUid("constraintEqualToAnchor:"),
		msg(toView, sel_getUid(anchor))
	);

}
