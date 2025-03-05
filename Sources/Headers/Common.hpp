#pragma once

#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>

template <typename _PrimitiveType>
static inline _PrimitiveType msg_get_primitive(id self, SEL _cmd) {
	_PrimitiveType (*msg)(id, SEL) = (__typeof(msg)) objc_msgSend;
	return msg(self, _cmd);
}

template <typename _Type>
static inline id msg_with_type(id self, SEL _cmd, _Type type) {
	id(*msg)(id, SEL, _Type) = (__typeof(msg)) objc_msgSend;
	return msg(self, _cmd, type);
}

static id (*msg)(id, SEL) = (__typeof(msg)) objc_msgSend;
static id (*clsMsg)(Class, SEL) = (__typeof(clsMsg)) objc_msgSend;
static id (*msgVariadic)(id, SEL, id, ...) = (__typeof(msgVariadic)) objc_msgSend;
static id (*msgAddTarget)(id, SEL, id, SEL, int) = (__typeof(msgAddTarget)) objc_msgSend;
static id (*const msgArrayInit)(Class, SEL, id, ...) = (__typeof(msgArrayInit)) objc_msgSend;
static id (*msgColorWithRed)(Class, SEL, CGFloat, CGFloat, CGFloat, CGFloat) = (__typeof(msgColorWithRed)) objc_msgSend;
static bool (*msgBoolForKey)(id, SEL, CFStringRef) = (__typeof(msgBoolForKey)) objc_msgSend;

static id auroraColor = msgColorWithRed(
	objc_getClass("UIColor"),
	sel_getUid("colorWithRed:green:blue:alpha:"),
	0.74,
	0.78,
	0.98,
	1.0
);
