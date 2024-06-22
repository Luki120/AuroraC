#include "../Headers/Common.hpp"
#include "../Utils/ViewHelper.hpp"

static id auroraHomeView;

static void AuroraHomeVC_setupUI(id self);
static void AuroraHomeVC_layoutUI(id self);

// ! Lifecyle

static void AuroraHomeVC_viewDidLoad(id self, SEL _cmd) {

	struct objc_super superViewDidLoad = {
		self,
		(Class) msg(self, sel_getUid("superclass"))
	};

	void (*superCall)(struct objc_super *, SEL) = (__typeof(superCall)) objc_msgSendSuper;
	superCall(&superViewDidLoad, _cmd);

	AuroraHomeVC_setupUI(self);

}

static void AuroraHomeVC_setupUI(id self) {

	if(!auroraHomeView) {
		auroraHomeView = clsMsg(objc_getClass("AuroraHomeView"), sel_getUid("new"));
		msg_with_type<id>(
			auroraHomeView,
			sel_getUid("setBackgroundColor:"),
			msg((id) objc_getClass("UIColor"), sel_getUid("systemBackgroundColor"))
		);
		msg_with_type<bool>(auroraHomeView, sel_getUid("setTranslatesAutoresizingMaskIntoConstraints:"), false);
		msg_with_type<id>(msg(self, sel_getUid("view")), sel_getUid("addSubview:"), auroraHomeView);
	}

	AuroraHomeVC_layoutUI(self);

}

static void AuroraHomeVC_layoutUI(id self) {

	id topAnchor = anchorViewToView(auroraHomeView, msg(self, sel_getUid("view")), "topAnchor");
	id bottomAnchor = anchorViewToView(auroraHomeView, msg(self, sel_getUid("view")), "bottomAnchor");
	id leadingAnchor = anchorViewToView(auroraHomeView, msg(self, sel_getUid("view")), "leadingAnchor");
	id trailingAnchor = anchorViewToView(auroraHomeView, msg(self, sel_getUid("view")), "trailingAnchor");

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

__attribute__((constructor)) static void initAuroraHomeVC(void) {

	Class AuroraHomeVCClass = objc_allocateClassPair(objc_getClass("UIViewController"), "AuroraHomeVC", 0);

	Method vdlMethod = class_getInstanceMethod(objc_getClass("UIViewController"), sel_getUid("viewDidLoad"));
	const char *vdlTypeEncoding = method_getTypeEncoding(vdlMethod);

	class_addMethod(AuroraHomeVCClass, sel_getUid("viewDidLoad"), (IMP) AuroraHomeVC_viewDidLoad, vdlTypeEncoding);

	objc_registerClassPair(AuroraHomeVCClass);

}
