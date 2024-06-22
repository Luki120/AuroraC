#include "../Headers/Common.hpp"
#include "../Utils/ViewHelper.hpp"

static id defaults;
static CFArrayRef settingsOptions;
static CFArrayRef otherApps;
static CFArrayRef appDescriptions;

static id auroraSettingsView;
static id settingsSwitch;

static id (*msgInitWithStyle)(id, SEL, int, CFStringRef) = (__typeof(msgInitWithStyle)) objc_msgSend;
static id (*msgDequeueReusableCell)(id, SEL, CFStringRef) = (__typeof(msgDequeueReusableCell)) objc_msgSend;
static id (*msgSetOn)(id, SEL, bool, bool) = (__typeof(msgSetOn)) objc_msgSend;
static id (*msgSetBool)(id, SEL, bool, CFStringRef) = (__typeof(msgSetBool)) objc_msgSend;
static id (*msgOpenURL)(id, SEL, CFURLRef, CFDictionaryRef, id) = (__typeof(msgOpenURL)) objc_msgSend;
static id (*msgDeselectRow)(id, SEL, id, bool) = (__typeof(msgDeselectRow)) objc_msgSend;

static void AuroraSettingsVC_setupUI(id self);
static void AuroraSettingsVC_layoutUI(id self);
static void AuroraSettingsVC_setSwitchState(CFStringRef state);
static void AuroraSettingsVC_openURLWithString(CFStringRef urlString);

static const CFStringRef kAreeshaURL = CFSTR("https://github.com/Luki120/Areesha");
static const CFStringRef kAzureURL = CFSTR("https://github.com/Luki120/Azure");
static const CFStringRef kSourceCodeURL = CFSTR("https://github.com/Luki120/AuroraC");

// ! Lifecycle

static void AuroraSettingsVC_viewDidLoad(id self, SEL _cmd) {

	struct objc_super superViewDidLoad = {
		self,
		(Class) msg(self, sel_getUid("superclass"))
	};

	void (*superCall)(struct objc_super *, SEL) = (__typeof(superCall)) objc_msgSendSuper;
	superCall(&superViewDidLoad, _cmd);

	AuroraSettingsVC_setupUI(self);

	defaults = clsMsg(objc_getClass("NSUserDefaults"), sel_getUid("standardUserDefaults"));

	CFStringRef options[3] = { CFSTR("A-Z"), CFSTR("0-9"), CFSTR("!@#$%^&*") };
	CFStringRef apps[2] = { CFSTR("Areesha"), CFSTR("Azure") };
	CFStringRef descriptions[2] = {
		CFSTR("Keep track of your favorite TV shows"),
		CFSTR("FOSS TOTP 2FA app with a clean, straightforward UI")
	};

	if(!settingsOptions) settingsOptions = CFArrayCreate(NULL, (const void **)options, 3, &kCFTypeArrayCallBacks);
	if(!otherApps) otherApps = CFArrayCreate(NULL, (const void **)apps, 2, &kCFTypeArrayCallBacks);
	if(!appDescriptions) appDescriptions = CFArrayCreate(NULL, (const void **)descriptions, 2, &kCFTypeArrayCallBacks);

}

static void AuroraSettingsVC_setupUI(id self) {

	if(!auroraSettingsView) {
		auroraSettingsView = clsMsg(objc_getClass("AuroraSettingsView"), sel_getUid("new"));
		msg_with_type<id>(
			auroraSettingsView,
			sel_getUid("setBackgroundColor:"),
			clsMsg(objc_getClass("UIColor"), sel_getUid("systemBackgroundColor"))
		);
		msg_with_type<bool>(auroraSettingsView, sel_getUid("setTranslatesAutoresizingMaskIntoConstraints:"), false);
		msg_with_type<id>(msg(self, sel_getUid("view")), sel_getUid("addSubview:"), auroraSettingsView);
	}

	id subviews = msg(auroraSettingsView, sel_getUid("subviews"));
	unsigned subviewsCount = msg_get_primitive<unsigned>(subviews, sel_getUid("count"));

	for(unsigned i = 0; i < subviewsCount; i++) {
		id tableView = msg_with_type<unsigned>(subviews, sel_getUid("objectAtIndex:"), i);

		if(!msg_with_type<Class>(tableView, sel_getUid("isKindOfClass:"), objc_getClass("UITableView"))) return;

		msg_with_type<id>(tableView, sel_getUid("setDelegate:"), self);
		msg_with_type<id>(tableView, sel_getUid("setDataSource:"), self);
	}

	AuroraSettingsVC_layoutUI(self);

}

static void AuroraSettingsVC_layoutUI(id self) {

	id topAnchor = anchorViewToView(auroraSettingsView, msg(self, sel_getUid("view")), "topAnchor");
	id bottomAnchor = anchorViewToView(auroraSettingsView, msg(self, sel_getUid("view")), "bottomAnchor");
	id leadingAnchor = anchorViewToView(auroraSettingsView, msg(self, sel_getUid("view")), "leadingAnchor");
	id trailingAnchor = anchorViewToView(auroraSettingsView, msg(self, sel_getUid("view")), "trailingAnchor");

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

static void AuroraSettingsVC_configureSwitches(void) {

	unsigned settingsCount = msg_get_primitive<unsigned>((id) settingsOptions, sel_getUid("count"));
	unsigned tag = msg_get_primitive<unsigned>(settingsSwitch, sel_getUid("tag"));

	for(int i = 0; i < settingsCount; i++) {
		switch(tag) {
			case 0: AuroraSettingsVC_setSwitchState(CFSTR("uppercase_switch_state")); break;
			case 1: AuroraSettingsVC_setSwitchState(CFSTR("numbers_switch_state")); break;
			case 2: AuroraSettingsVC_setSwitchState(CFSTR("special_characters_switch_state")); break;
			default: break;
		}
	}

}

// ! UITableViewDataSource

static int AuroraSettingsVC_numberOfSectionsInTableView(id self, SEL _cmd, id tableView) {

	return 3;

}

static int AuroraSettingsVC_numberOfRowsInSection(id self, SEL _cmd, id tableView, int section) {

	switch(section) {
		case 0: return msg_get_primitive<int>((id) settingsOptions, sel_getUid("count"));
		case 1: return msg_get_primitive<int>((id) otherApps, sel_getUid("count"));
		case 2: return 1;
		default: return 0;
	}

}

static id AuroraSettingsVC_cellForRowAtIndexPath(id self, SEL _cmd, id tableView, id indexPath) {

	id cell = msgDequeueReusableCell(
		tableView,
		sel_getUid("dequeueReusableCellWithIdentifier:"),
		CFSTR("VanillaCell")
	);

	if(!cell) cell = msgInitWithStyle(
		clsMsg(objc_getClass("UITableViewCell"), sel_getUid("alloc")),
		sel_getUid("initWithStyle:reuseIdentifier:"),
		3,
		CFSTR("VanillaCell")
	);

	int section = msg_get_primitive<int>(indexPath, sel_getUid("section"));

	switch(section) {
		case 0:
			msg_with_type<id>(
				msg(cell, sel_getUid("textLabel")),
				sel_getUid("setText:"),
				msg_with_type<unsigned>(
					(id) settingsOptions,
					sel_getUid("objectAtIndex:"),
					msg_get_primitive<int>(indexPath, sel_getUid("row"))
				)
			);
			break;

		case 1:
			msg_with_type<id>(
				msg(cell, sel_getUid("textLabel")),
				sel_getUid("setText:"),
				msg_with_type<unsigned>(
					(id) otherApps,
					sel_getUid("objectAtIndex:"),
					msg_get_primitive<int>(indexPath, sel_getUid("row"))
				)
			);
			msg_with_type<id>(
				msg(cell, sel_getUid("detailTextLabel")),
				sel_getUid("setText:"),
				msg_with_type<unsigned>(
					(id) appDescriptions,
					sel_getUid("objectAtIndex:"),
					msg_get_primitive<int>(indexPath, sel_getUid("row"))
				)
			);
			msg_with_type<id>(
				msg(cell, sel_getUid("detailTextLabel")),
				sel_getUid("setTextColor:"),
				clsMsg(objc_getClass("UIColor"), sel_getUid("systemGrayColor"))
			);
			msg_with_type<id>(
				msg(cell, sel_getUid("detailTextLabel")),
				sel_getUid("setFont:"),
				msg_with_type<CGFloat>(
					(id) objc_getClass("UIFont"),
					sel_getUid("systemFontOfSize:"),
					10
				)
			);
			break;

		case 2:
			msg_with_type<CFStringRef>(
				msg(cell, sel_getUid("textLabel")),
				sel_getUid("setText:"),
				CFSTR("Source Code")
			);
			break;

		default: break;
	}

	if(section != 0) return cell;

	settingsSwitch = clsMsg(objc_getClass("UISwitch"), sel_getUid("new"));
	msg_with_type<unsigned>(
		settingsSwitch,
		sel_getUid("setTag:"),
		msg_get_primitive<unsigned>(indexPath, sel_getUid("row"))
	);
	msg_with_type<id>(settingsSwitch, sel_getUid("setOnTintColor:"), auroraColor);
	msgAddTarget(
		settingsSwitch,
		sel_getUid("addTarget:action:forControlEvents:"),
		self,
		sel_getUid("switchValueDidChange:"),
		0x1000
	);

	AuroraSettingsVC_configureSwitches();
	msg_with_type<id>(cell, sel_getUid("setAccessoryView:"), settingsSwitch);

	return cell;

}

static CFStringRef AuroraSettingsVC_titleForHeaderInSection(id self, SEL _cmd, id tableView, unsigned section) {

	switch(section) {
		case 0: return CFSTR("Options");
		case 1: return CFSTR("Other apps you may like");
		case 2: return CFSTR("Madness");
		default: return nil;
	}

}

// ! UITableViewDelegate

static void AuroraSettingsVC_didSelectRowAtIndexPath(id self, SEL _cmd, id tableView, id indexPath) {

	msgDeselectRow(tableView, sel_getUid("deselectRowAtIndexPath:animated:"), indexPath, true);

	int row = msg_get_primitive<int>(indexPath, sel_getUid("row"));
	int section = msg_get_primitive<int>(indexPath, sel_getUid("section"));

	switch(section) {
		case 0: break;
		case 1:
			switch(row) {
				case 0: AuroraSettingsVC_openURLWithString(kAreeshaURL); break;
				case 1: AuroraSettingsVC_openURLWithString(kAzureURL); break;
			}
			break;

		case 2: AuroraSettingsVC_openURLWithString(kSourceCodeURL); break;
	}

}

static CGFloat AuroraSettingsVC_heightForHeaderInSection(id self, SEL _cmd, id tableView, unsigned section) {

	switch(section) {
		case 0: return 55.5;
		default: return 38;
	}

}

// ! Selectors

static void AuroraSettingsVC_switchValueDidChange(id self, SEL _cmd, id sender) {

	bool isOn = msg_get_primitive<bool>(sender, sel_getUid("isOn"));
	int tag = msg_get_primitive<int>(sender, sel_getUid("tag"));

	switch(tag) {
		case 0: msgSetBool(defaults, sel_getUid("setBool:forKey:"), isOn, CFSTR("uppercase_switch_state")); break;
		case 1: msgSetBool(defaults, sel_getUid("setBool:forKey:"), isOn, CFSTR("numbers_switch_state")); break;
		case 2: msgSetBool(defaults, sel_getUid("setBool:forKey:"), isOn, CFSTR("special_characters_switch_state")); break;
		default: break;
	}

}

static void AuroraSettingsVC_setSwitchState(CFStringRef state) {

	msgSetOn(
		settingsSwitch,
		sel_getUid("setOn:animated:"),
		msgBoolForKey(defaults, sel_getUid("boolForKey:"), state),
		true
	);

}

static void AuroraSettingsVC_openURLWithString(CFStringRef urlString) {

	CFURLRef url = CFURLCreateWithString(NULL, urlString, NULL);
	msgOpenURL(
		clsMsg(objc_getClass("UIApplication"), sel_getUid("sharedApplication")),
		sel_getUid("openURL:options:completionHandler:"),
		url,
		NULL,
		NULL
	);

	CFRelease(url);

}

__attribute__((constructor)) static void initAuroraSettingsVC(void) {

	Class AuroraSettingsVCClass = objc_allocateClassPair(objc_getClass("UIViewController"), "AuroraSettingsVC", 0);

	Method viewDidLoadMethod = class_getInstanceMethod(objc_getClass("UIViewController"), sel_getUid("viewDidLoad"));
	const char *vdlTypeEncoding = method_getTypeEncoding(viewDidLoadMethod);

	class_addMethod(AuroraSettingsVCClass, sel_getUid("viewDidLoad"), (IMP) AuroraSettingsVC_viewDidLoad, vdlTypeEncoding);
	class_addMethod(AuroraSettingsVCClass, sel_getUid("switchValueDidChange:"), (IMP) AuroraSettingsVC_switchValueDidChange, "v@:@");
	class_addMethod(AuroraSettingsVCClass, sel_getUid("numberOfSectionsInTableView:"), (IMP) AuroraSettingsVC_numberOfSectionsInTableView, "q@:@");
	class_addMethod(AuroraSettingsVCClass, sel_getUid("tableView:numberOfRowsInSection:"), (IMP) AuroraSettingsVC_numberOfRowsInSection, "q@:@q");
	class_addMethod(AuroraSettingsVCClass, sel_getUid("tableView:cellForRowAtIndexPath:"), (IMP) AuroraSettingsVC_cellForRowAtIndexPath, "@@:@@");
	class_addMethod(AuroraSettingsVCClass, sel_getUid("tableView:titleForHeaderInSection:"), (IMP) AuroraSettingsVC_titleForHeaderInSection, "@@:@q");
	class_addMethod(AuroraSettingsVCClass, sel_getUid("tableView:didSelectRowAtIndexPath:"), (IMP) AuroraSettingsVC_didSelectRowAtIndexPath, "v@:@@");
	class_addMethod(AuroraSettingsVCClass, sel_getUid("tableView:heightForHeaderInSection:"), (IMP) AuroraSettingsVC_heightForHeaderInSection, "d@:@q");

	objc_registerClassPair(AuroraSettingsVCClass);

}
