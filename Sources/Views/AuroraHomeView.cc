#include "../Headers/Common.hpp"

static id defaults;
static id randomLabel;
static id sliderValueLabel;
static id regenerateButton;
static id copyButton;
static id lengthSlider;
static id verticalStackView;
static id horizontalStackView;

static id salmonColor = msgColorWithRed(
	(id) objc_getClass("UIColor"),
	sel_getUid("colorWithRed:green:blue:alpha:"),
	1.0,
	0.55,
	0.51,
	1.0
);

static CFRange (*msgGetRange)(id, SEL) = (__typeof(msgGetRange)) objc_msgSend;
static float (*msgFloatForKey)(id, SEL, CFStringRef) = (__typeof(msgFloatForKey)) objc_msgSend;
static id (*msgButton)(id, SEL, id, int) = (__typeof(msgButton)) objc_msgSend;
static id (*msgSetFloat)(id, SEL, float, CFStringRef) = (__typeof(msgSetFloat)) objc_msgSend;
static id (*msgSetCustomSpacing)(id, SEL, CGFloat, id) = (__typeof(msgSetCustomSpacing)) objc_msgSend;
static id (*msgInitWithPattern)(id, SEL, CFStringRef, int, id) = (__typeof(msgInitWithPattern)) objc_msgSend;
static id (*msgMatchesInString)(id, SEL, CFStringRef, int, CFRange) = (__typeof(msgMatchesInString)) objc_msgSend;
static id (*msgAddAttributes)(CFStringRef, SEL, CFDictionaryRef, CFRange) = (__typeof(msgAddAttributes)) objc_msgSend;
static id (*msgSetValueAnimated)(id, SEL, float, bool) = (__typeof(msgSetValueAnimated)) objc_msgSend;
static id (*msgConstraintEqualToAnchorConstant)(id, SEL, id, CGFloat) = (__typeof(msgConstraintEqualToAnchorConstant)) objc_msgSend;
static CFStringRef (*msgStringByAppendingString)(CFStringRef, SEL, CFStringRef) = (__typeof(msgStringByAppendingString)) objc_msgSend;

static void AuroraHomeView_setupUI(id self);
static void AuroraHomeView_layoutUI(id self);
static id AuroraHomeView_createStackViewWithAxis(int axis);
static id AuroraHomeView_createLabelWithFontSize(CGFloat fontSize);
static id AuroraHomeView_createButton(id self, CFStringRef title, SEL action);
static id AuroraHomeView_createSlider(id self);
static id AuroraHomeView_getSliderValueAsString(float sliderValue);
static void AuroraHomeView_setAttributedText(void);

typedef CFStringRef CALayerCornerCurve;
typedef CFStringRef NSAttributedStringKey;

extern "C" {
	extern CALayerCornerCurve kCACornerCurveContinuous;
	extern NSAttributedStringKey NSForegroundColorAttributeName;
}

// ! Lifecycle

static id AuroraHomeView_initWithFrame(id self, SEL _cmd, CGRect frame) {

	struct objc_super superInitWithFrame = {
		self,
		(Class) msg(self, sel_getUid("superclass"))
	};

	id (*superCall)(struct objc_super *, SEL, CGRect) = (__typeof(superCall)) objc_msgSendSuper;
	self = superCall(&superInitWithFrame, _cmd, frame);

	AuroraHomeView_setupUI(self);

	return self;

}

static void AuroraHomeView_setupUI(id self) {

	defaults = clsMsg(objc_getClass("NSUserDefaults"), sel_getUid("standardUserDefaults"));
	float sliderValue = msgFloatForKey(defaults, sel_getUid("floatForKey:"), CFSTR("aurora_slider_value"));

	if(!verticalStackView) {
		verticalStackView = AuroraHomeView_createStackViewWithAxis(1);
		msg_with_type<bool>(verticalStackView, sel_getUid("setTranslatesAutoresizingMaskIntoConstraints:"), false);
		msg_with_type<id>(self, sel_getUid("addSubview:"), verticalStackView);
	}

	if(!randomLabel) {
		randomLabel = AuroraHomeView_createLabelWithFontSize(22);
		msg_with_type<id>(verticalStackView, sel_getUid("addArrangedSubview:"), randomLabel);
	}

	if(!regenerateButton)
		regenerateButton = AuroraHomeView_createButton(self, CFSTR("Regenerate password"), sel_getUid("didTapRegenerateButton"));

	if(!copyButton)
		copyButton = AuroraHomeView_createButton(self, CFSTR("Copy password"), sel_getUid("didTapCopyButton"));

	if(!horizontalStackView) {
		horizontalStackView = AuroraHomeView_createStackViewWithAxis(0);
		msg_with_type<id>(verticalStackView, sel_getUid("addArrangedSubview:"), horizontalStackView);
	}

	if(!lengthSlider) {
		lengthSlider = AuroraHomeView_createSlider(self);
		msgSetValueAnimated(lengthSlider, sel_getUid("setValue:animated:"), sliderValue, true);
	}

	AuroraHomeView_setAttributedText();

	if(!sliderValueLabel) {
		sliderValueLabel = AuroraHomeView_createLabelWithFontSize(10);
		msg_with_type<id>(sliderValueLabel, sel_getUid("setText:"), AuroraHomeView_getSliderValueAsString(sliderValue));
		msg_with_type<id>(horizontalStackView, sel_getUid("addArrangedSubview:"), sliderValueLabel);
	}

	msgSetCustomSpacing(verticalStackView, sel_getUid("setCustomSpacing:afterView:"), 5, regenerateButton);

	AuroraHomeView_layoutUI(self);

}

static void AuroraHomeView_layoutUI(id self) {

	id verticalStackViewTopAnchor = msgConstraintEqualToAnchorConstant(
		msg(verticalStackView, sel_getUid("topAnchor")),
		sel_getUid("constraintEqualToAnchor:constant:"),
		msg(msg(self, sel_getUid("safeAreaLayoutGuide")), sel_getUid("topAnchor")),
		20.0
	);

	id verticalStackViewCenterXAnchor = msg_with_type<id>(
		msg(verticalStackView, sel_getUid("centerXAnchor")),
		sel_getUid("constraintEqualToAnchor:"),
		msg(self, sel_getUid("centerXAnchor"))
	);

	id regenerateButtonWidthAnchor = msg_with_type<CGFloat>(
		msg(regenerateButton, sel_getUid("widthAnchor")), sel_getUid("constraintEqualToConstant:"), 220
	);

	id regenerateButtonHeightAnchor = msg_with_type<CGFloat>(
		msg(regenerateButton, sel_getUid("heightAnchor")), sel_getUid("constraintEqualToConstant:"), 44
	);

	id copyButtonWidthAnchor = msg_with_type<CGFloat>(
		msg(copyButton, sel_getUid("widthAnchor")), sel_getUid("constraintEqualToConstant:"), 220
	);

	id copyButtonHeightAnchor = msg_with_type<CGFloat>(
		msg(copyButton, sel_getUid("heightAnchor")), sel_getUid("constraintEqualToConstant:"), 44
	);

	id sliderWidthAnchor = msg_with_type<CGFloat>(
		msg(lengthSlider, sel_getUid("widthAnchor")), sel_getUid("constraintEqualToConstant:"), 180
	);

	id sliderHeightAnchor = msg_with_type<CGFloat>(
		msg(lengthSlider, sel_getUid("heightAnchor")), sel_getUid("constraintEqualToConstant:"), 22
	);

	id constraints = msgArrayInit(
		objc_getClass("NSArray"),
		sel_getUid("arrayWithObjects:"),
		verticalStackViewTopAnchor,
		verticalStackViewCenterXAnchor,
		regenerateButtonWidthAnchor,
		regenerateButtonHeightAnchor,
		copyButtonWidthAnchor,
		copyButtonHeightAnchor,
		sliderWidthAnchor,
		sliderHeightAnchor,
		nil
	);

	msg_with_type<id>(
		(id) objc_getClass("NSLayoutConstraint"),
		sel_getUid("activateConstraints:"),
		constraints
	);

}

// ! Reusable

static float AuroraHomeView_getSliderValue(id slider) {

	float sliderValue = msg_get_primitive<float>(slider, sel_getUid("value"));
	AuroraHomeView_setAttributedText();
	return sliderValue;

}

static id AuroraHomeView_getSliderValueAsString(float sliderValue) {

	id intNumber = msg_with_type<int>((id) objc_getClass("NSNumber"), sel_getUid("numberWithInt:"), (int) sliderValue);
	return msg(intNumber, sel_getUid("stringValue"));

}

static id AuroraHomeView_createStackViewWithAxis(int axis) {

	id stackView = clsMsg(objc_getClass("UIStackView"), sel_getUid("new"));
	msg_with_type<int>(stackView, sel_getUid("setAxis:"), axis);
	msg_with_type<CGFloat>(stackView, sel_getUid("setSpacing:"), 10);
	return stackView;

}

static id AuroraHomeView_createLabelWithFontSize(CGFloat fontSize) {

	id label = clsMsg(objc_getClass("UILabel"), sel_getUid("new"));
	msg_with_type<id>(
		label,
		sel_getUid("setFont:"),
		msg_with_type<CGFloat>(
			(id) objc_getClass("UIFont"),
			sel_getUid("systemFontOfSize:"),
			fontSize
		)
	);
	msg_with_type<int>(label, sel_getUid("setTextAlignment:"), 1);
	msg_with_type<bool>(label, sel_getUid("setAdjustsFontSizeToFitWidth:"), true);

	return label;

}

static id AuroraHomeView_createButton(id self, CFStringRef title, SEL action) {

	id button = msg_with_type<int>((id) objc_getClass("UIButton"), sel_getUid("buttonWithType:"), 1);
	msg_with_type<id>(button, sel_getUid("setBackgroundColor:"), auroraColor);
	msgAddTarget(
		button,
		sel_getUid("addTarget:action:forControlEvents:"),
		self,
		action,
		13
	);
	msgButton(button, sel_getUid("setTitle:forState:"), (id) title, 0);
	msgButton(
		button,
		sel_getUid("setTitleColor:forState:"),
		clsMsg(objc_getClass("UIColor"), sel_getUid("labelColor")),
		0
	);
	msg_with_type<bool>(button, sel_getUid("setTranslatesAutoresizingMaskIntoConstraints:"), false);
	msg_with_type<CFStringRef>(msg(button, sel_getUid("layer")), sel_getUid("setCornerCurve:"), kCACornerCurveContinuous);
	msg_with_type<CGFloat>(msg(button, sel_getUid("layer")), sel_getUid("setCornerRadius:"), 22);
	msg_with_type<id>(verticalStackView, sel_getUid("addArrangedSubview:"), button);

	return button;

}

static id AuroraHomeView_createSlider(id self) {

	id slider = clsMsg(objc_getClass("UISlider"), sel_getUid("new"));
	msg_with_type<float>(slider, sel_getUid("setMinimumValue:"), 10.0);
	msg_with_type<float>(slider, sel_getUid("setMaximumValue:"), 25.0);
	msg_with_type<id>(slider, sel_getUid("setMinimumTrackTintColor:"), auroraColor);
	msg_with_type<bool>(slider, sel_getUid("setTranslatesAutoresizingMaskIntoConstraints:"), false);
	msg_with_type<id>(horizontalStackView, sel_getUid("addArrangedSubview:"), slider);
	msgAddTarget(
		slider,
		sel_getUid("addTarget:action:forControlEvents:"),
		self,
		sel_getUid("sliderValueDidChange"),
		12
	);

	return slider;

}

// ! Selectors

static void AuroraHomeView_didTapRegenerateButton(id self, SEL _cmd) {

	id transition = clsMsg(objc_getClass("CATransition"), sel_getUid("new"));
	msg_with_type<CGFloat>(transition, sel_getUid("setDuration:"), 0.5);
	msgVariadic(msg(randomLabel, sel_getUid("layer")), sel_getUid("addAnimation:forKey:"), transition, NULL);

	AuroraHomeView_getSliderValue(lengthSlider);

}

static void AuroraHomeView_didTapCopyButton(id self, SEL _cmd) {

	msg_with_type<id>(
		clsMsg(objc_getClass("UIPasteboard"), sel_getUid("generalPasteboard")),
		sel_getUid("setString:"),
		msg(randomLabel, sel_getUid("text"))
	);

}

static void AuroraHomeView_sliderValueDidChange(id self, SEL _cmd) {

	float sliderValue = AuroraHomeView_getSliderValue(lengthSlider);

	msg_with_type<id>(sliderValueLabel, sel_getUid("setText:"), AuroraHomeView_getSliderValueAsString(sliderValue));
	msgSetFloat(defaults, sel_getUid("setFloat:forKey:"), sliderValue, CFSTR("aurora_slider_value"));

}

// ! Utilities

static CFStringRef AuroraHomeView_generateRandomString(int length) {

	CFStringRef numbers = CFSTR("0123456789");
	CFStringRef characters = CFSTR("abcdefghijklmnopqrstuvwxyz");
	CFStringRef specialCharacters = CFSTR("!@#$%^&*()_+-=[]{}|;':,./<>?`~");
	CFStringRef uppercaseCharacters = CFSTR("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	bool uppercaseSwitchState = msgBoolForKey(defaults, sel_getUid("boolForKey:"), CFSTR("uppercase_switch_state"));
	bool numbersSwitchState = msgBoolForKey(defaults, sel_getUid("boolForKey:"), CFSTR("numbers_switch_state"));
	bool specialCharactersSwitchState = msgBoolForKey(defaults, sel_getUid("boolForKey:"), CFSTR("special_characters_switch_state"));

	CFStringRef randomString = (CFStringRef) msg_with_type<int>(
		(id) objc_getClass("NSMutableString"),
		sel_getUid("stringWithCapacity:"),
		length
	);

	if(uppercaseSwitchState)
		characters = msgStringByAppendingString(characters, sel_getUid("stringByAppendingString:"), uppercaseCharacters);

	if(numbersSwitchState)
		characters = msgStringByAppendingString(characters, sel_getUid("stringByAppendingString:"), numbers);

	if(specialCharactersSwitchState)
		characters = msgStringByAppendingString(characters, sel_getUid("stringByAppendingString:"), specialCharacters);

	for(int i = 0; i < length; i++) {
		id index = msg_with_type<int>(
			(id) characters,
			sel_getUid("characterAtIndex:"),
			arc4random_uniform(CFStringGetLength(characters))
		);

		msgVariadic((id) randomString, sel_getUid("appendFormat:"), (id) CFSTR("%C"), index);
	}

	return randomString;

}

static CFArrayRef AuroraHomeView_findRangesInString(CFStringRef string, CFStringRef pattern) {

	id regex = msgInitWithPattern(
		clsMsg(objc_getClass("NSRegularExpression"), sel_getUid("alloc")),
		sel_getUid("initWithPattern:options:error:"),
		pattern,
		0,
		nil
	);

	id matches = msgMatchesInString(
		regex,
		sel_getUid("matchesInString:options:range:"),
		string,
		0,
		(CFRange){ 0, CFStringGetLength(string) }
	);

	return (CFArrayRef)msg_with_type<CFStringRef>(matches, sel_getUid("valueForKeyPath:"), CFSTR("@unionOfObjects.range"));

}

static CFStringRef AuroraHomeView_attributedString(CFStringRef string, CFDictionaryRef numbersAttribute, CFDictionaryRef symbolsAttribute) {

	CFArrayRef numberRanges = AuroraHomeView_findRangesInString(string, CFSTR("[0-9]+"));
	CFArrayRef symbolRanges = AuroraHomeView_findRangesInString(string, CFSTR("[^\\w]"));

	unsigned numberCount = msg_get_primitive<unsigned>((id) numberRanges, sel_getUid("count"));
	unsigned symbolCount = msg_get_primitive<unsigned>((id) symbolRanges, sel_getUid("count"));

	CFStringRef attributedString = (CFStringRef) msg_with_type<CFStringRef>(
		clsMsg(objc_getClass("NSMutableAttributedString"), sel_getUid("alloc")),
		sel_getUid("initWithString:"),
		string
	);

	for(unsigned i = 0; i < numberCount; i++) {
		msgAddAttributes(
			attributedString,
			sel_getUid("addAttributes:range:"),
			numbersAttribute,
			msgGetRange(
				msg_with_type<unsigned>((id) numberRanges, sel_getUid("objectAtIndex:"), i),
				sel_getUid("rangeValue")
			)
		);
	}

	for(unsigned i = 0; i < symbolCount; i++) {
		msgAddAttributes(
			attributedString,
			sel_getUid("addAttributes:range:"),
			symbolsAttribute,
			msgGetRange(
				msg_with_type<unsigned>((id) symbolRanges, sel_getUid("objectAtIndex:"), i),
				sel_getUid("rangeValue")
			)
		);
	}

	return attributedString;

}

static CFDictionaryRef AuroraHomeView_createDictionary(const void **keys, const void **values) {

	return CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);

}

static void AuroraHomeView_setAttributedText(void) {

	const void *numbersAttributeKeys[] = { NSForegroundColorAttributeName };
	const void *numbersAttributeValues[] = { auroraColor };
	const void *symbolsAttributeKeys[] = { NSForegroundColorAttributeName };
	const void *symbolsAttributeValues[] = { salmonColor };

	CFDictionaryRef numbersDictionary = AuroraHomeView_createDictionary(numbersAttributeKeys, numbersAttributeValues);
	CFDictionaryRef symbolsDictionary = AuroraHomeView_createDictionary(symbolsAttributeKeys, symbolsAttributeValues);

	float sliderValue = msg_get_primitive<float>(lengthSlider, sel_getUid("value"));

	msg_with_type<CFStringRef>(
		randomLabel,
		sel_getUid("setAttributedText:"),
		AuroraHomeView_attributedString(
			AuroraHomeView_generateRandomString((int) sliderValue),
			numbersDictionary,
			symbolsDictionary
		)
	);

	CFRelease(numbersDictionary);
	CFRelease(symbolsDictionary);

}

__attribute__((constructor)) static void initAuroraHomeView(void) {

	Class AuroraHomeViewClass = objc_allocateClassPair(objc_getClass("UIView"), "AuroraHomeView", 0);

	Method method = class_getInstanceMethod(objc_getClass("UIView"), sel_getUid("initWithFrame:"));
	const char *typeEncoding = method_getTypeEncoding(method);

	class_addMethod(AuroraHomeViewClass, sel_getUid("initWithFrame:"), (IMP) AuroraHomeView_initWithFrame, typeEncoding);
	class_addMethod(AuroraHomeViewClass, sel_getUid("didTapRegenerateButton"), (IMP) AuroraHomeView_didTapRegenerateButton, "v@:");
	class_addMethod(AuroraHomeViewClass, sel_getUid("didTapCopyButton"), (IMP) AuroraHomeView_didTapCopyButton, "v@:");
	class_addMethod(AuroraHomeViewClass, sel_getUid("sliderValueDidChange"), (IMP) AuroraHomeView_sliderValueDidChange, "v@:");

	objc_registerClassPair(AuroraHomeViewClass);

}
