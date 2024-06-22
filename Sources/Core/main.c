#include <CoreFoundation/CoreFoundation.h>

extern int UIApplicationMain(int argc, char * _Nullable *argv, CFStringRef principalClassName, CFStringRef delegateClassName);

int main(int argc, char *argv[]) {
	UIApplicationMain(argc, argv, nil, CFSTR("AppDelegate"));
}
