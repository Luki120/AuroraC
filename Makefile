ARCHS = arm64
TARGET := iphone:clang:latest:14.0
INSTALL_TARGET_PROCESSES = Aurora
APPLICATION_NAME = Aurora

Aurora_FILES = $(wildcard **/**/*.cc) Sources/Core/main.c
Aurora_FRAMEWORKS = UIKit CoreGraphics QuartzCore

include $(THEOS)/makefiles/common.mk
include $(THEOS_MAKE_PATH)/application.mk
