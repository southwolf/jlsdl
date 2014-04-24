#!/bin/sh
xcodebuild -alltargets
cp build/Release-iphoneos/libjlsdl.a libs/
