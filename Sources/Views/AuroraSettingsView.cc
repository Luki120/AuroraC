#include "../Headers/Common.hpp"
#include "../Utils/ViewHelper.hpp"

static id tableView;

static id (*msgInitWithFrame)(id, SEL, CGRect, unsigned) = (__typeof(msgInitWithFrame)) objc_msgSend;
static id (*msgSetFrame)(id, SEL, CGRect) = (__typeof(msgSetFrame)) objc_msgSend;

static void AuroraSettingsView_setupUI(id self);
static void AuroraSettingsView_layoutUI(id self);
static id AuroraSettingsView_createFooterView(void);

// ! Lifecycle

static id AuroraSettingsView_initWithFrame(id self, SEL _cmd, CGRect frame) {

	struct objc_super superInitWithFrame = {
		self,
		(Class) msg(self, sel_getUid("superclass"))
	};

	id (*superCall)(struct objc_super *, SEL, CGRect) = (__typeof(superCall)) objc_msgSendSuper;
	self = superCall(&superInitWithFrame, _cmd, frame);

	AuroraSettingsView_setupUI(self);

	return self;

}

static void AuroraSettingsView_setupUI(id self) {

	if(!tableView) {
		tableView = msgInitWithFrame(
			clsMsg(objc_getClass("UITableView"), sel_getUid("alloc")),
			sel_getUid("initWithFrame:style:"),
			CGRectZero,
			2
		);
		msg_with_type<CGFloat>(tableView, sel_getUid("setRowHeight:"), 44);
		msg_with_type<id>(tableView, sel_getUid("setTableFooterView:"), AuroraSettingsView_createFooterView());
		msg_with_type<bool>(tableView, sel_getUid("setTranslatesAutoresizingMaskIntoConstraints:"), false);
		msg_with_type<id>(self, sel_getUid("addSubview:"), tableView);
	}

	AuroraSettingsView_layoutUI(self);

}


static void AuroraSettingsView_layoutUI(id self) {

	id topAnchor = anchorViewToView(tableView, self, "topAnchor");
	id bottomAnchor = anchorViewToView(tableView, self, "bottomAnchor");
	id leadingAnchor = anchorViewToView(tableView, self, "leadingAnchor");
	id trailingAnchor = anchorViewToView(tableView, self, "trailingAnchor");

	id constraints = msgArrayInit(
		objc_getClass("NSArray"),
		sel_getUid("arrayWithObjects:"),
		topAnchor, bottomAnchor, leadingAnchor, trailingAnchor, nil
	);

	msg_with_type<id>(
		(id) objc_getClass("NSLayoutConstraint"),
		sel_getUid("activateConstraints:"),
		constraints
	);

}

// ! Reusable

static id AuroraSettingsView_createFooterView(void) {

	id footerView = clsMsg(objc_getClass("UIView"), sel_getUid("new"));
	msgSetFrame(footerView, sel_getUid("setFrame:"), CGRectMake(0, 0, 0, 60));

	id copyrightLabel = clsMsg(objc_getClass("UILabel"), sel_getUid("new"));
	msg_with_type<id>(
		copyrightLabel,
		sel_getUid("setFont:"),
		msg_with_type<CGFloat>(
			(id) objc_getClass("UIFont"),
			sel_getUid("systemFontOfSize:"),
			12
		)
	);
	msg_with_type<CFStringRef>(copyrightLabel, sel_getUid("setText:"), CFSTR("© 2024 Luki120"));
	msg_with_type<id>(
		copyrightLabel,
		sel_getUid("setTextColor:"),
		clsMsg(objc_getClass("UIColor"), sel_getUid("systemGrayColor"))
	);
	msg_with_type<bool>(copyrightLabel, sel_getUid("setTranslatesAutoresizingMaskIntoConstraints:"), false);
	msg_with_type<id>(footerView, sel_getUid("addSubview:"), copyrightLabel);

	id centerXAnchor = anchorViewToView(copyrightLabel, footerView, "centerXAnchor");
	id centerYAnchor = anchorViewToView(copyrightLabel, footerView, "centerYAnchor");

	id constraints = msgArrayInit(
		objc_getClass("NSArray"),
		sel_getUid("arrayWithObjects:"),
		centerXAnchor, centerYAnchor, nil
	);

	msg_with_type<id>(
		(id) objc_getClass("NSLayoutConstraint"),
		sel_getUid("activateConstraints:"),
		constraints
	);

	return footerView;

}

__attribute__((constructor)) static void initAuroraSettingsView(void) {

	Class AuroraSettingsViewClass = objc_allocateClassPair(objc_getClass("UIView"), "AuroraSettingsView", 0);

	Method method = class_getInstanceMethod(objc_getClass("UIView"), sel_getUid("initWithFrame:"));
	const char *typeEncoding = method_getTypeEncoding(method);

	class_addMethod(AuroraSettingsViewClass, sel_getUid("initWithFrame:"), (IMP) AuroraSettingsView_initWithFrame, typeEncoding);

	objc_registerClassPair(AuroraSettingsViewClass);

}
