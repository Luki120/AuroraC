#include "../Headers/Common.hpp"

struct AppDel {
	Class isa;
	id window;
};

static id (*msgSetTabBarItem)(id, SEL, CFStringRef, id, int) = (id (*)(id, SEL, CFStringRef, id, int)) objc_msgSend;	
static id (*const msgSetViewControllers)(id, SEL, CFArrayRef, bool) = (__typeof(msgSetViewControllers))objc_msgSend;

static BOOL AppDel_didFinishLaunching(struct AppDel *self, SEL _cmd, void *application, void *options) {

	self->window = clsMsg(objc_getClass("UIWindow"), sel_getUid("new"));

	id tabBarController = clsMsg(objc_getClass("UITabBarController"), sel_getUid("new"));

	id homeVC = clsMsg(objc_getClass("AuroraHomeVC"), sel_getUid("new"));
	msg_with_type<CFStringRef>(homeVC, sel_getUid("setTitle:"), CFSTR("Aurora"));

	id settingsVC = clsMsg(objc_getClass("AuroraSettingsVC"), sel_getUid("new"));
	msg_with_type<CFStringRef>(settingsVC, sel_getUid("setTitle:"), CFSTR("Settings"));

	CFTypeRef viewControllers[2];

	id firstNavVC = msg_with_type<id>(
		clsMsg(objc_getClass("UINavigationController"), sel_getUid("alloc")),
		sel_getUid("initWithRootViewController:"),
		homeVC
	);

	id secondNavVC = msg_with_type<id>(
		clsMsg(objc_getClass("UINavigationController"), sel_getUid("alloc")),
		sel_getUid("initWithRootViewController:"),
		settingsVC
	);

	msg_with_type<id>(
		firstNavVC,
		sel_getUid("setTabBarItem:"),
		msgSetTabBarItem(
			clsMsg(objc_getClass("UITabBarItem"), sel_getUid("alloc")),
			sel_getUid("initWithTitle:image:tag:"),
			CFSTR("Home"),
			msg_with_type<CFStringRef>((id) objc_getClass("UIImage"), sel_getUid("systemImageNamed:"), CFSTR("house.fill")),
			0
		)
	);

	msg_with_type<id>(
		secondNavVC,
		sel_getUid("setTabBarItem:"),
		msgSetTabBarItem(
			clsMsg(objc_getClass("UITabBarItem"), sel_getUid("alloc")),
			sel_getUid("initWithTitle:image:tag:"),
			CFSTR("Settings"),
			msg_with_type<CFStringRef>((id) objc_getClass("UIImage"), sel_getUid("systemImageNamed:"), CFSTR("gear")),
			1
		)
	);

	viewControllers[0] = firstNavVC;
	viewControllers[1] = secondNavVC;
	CFArrayRef viewControllersArray = CFArrayCreate(NULL, (const void **)viewControllers, 2, &kCFTypeArrayCallBacks);

	msgSetViewControllers(tabBarController, sel_getUid("setViewControllers:animated:"), viewControllersArray, true);

	msg_with_type<id>(self->window, sel_getUid("setTintColor:"), auroraColor);
	msg_with_type<id>(self->window, sel_getUid("setRootViewController:"), tabBarController);
	msg(self->window, sel_getUid("makeKeyAndVisible"));

	CFRelease(viewControllersArray);

	return true;

}

__attribute__((constructor)) static void initAppDelegate(void) {

	Class AppDelClass = objc_allocateClassPair(objc_getClass("UIResponder"), "AppDelegate", 0);

	class_addIvar(AppDelClass, "window", sizeof(id), 0, "@");
	class_addMethod(AppDelClass, sel_getUid("application:didFinishLaunchingWithOptions:"), (IMP) AppDel_didFinishLaunching, "i@:@@");

	objc_registerClassPair(AppDelClass);

}
