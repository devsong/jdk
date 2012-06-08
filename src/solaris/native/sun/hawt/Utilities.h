/*
 * Copyright (c) 2012, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */
#ifndef UTILITIES_H
#define UTILITIES_H

#include <jni.h>
#include <jni_util.h>
#include <debug_util.h>

#include <Rect.h>

extern "C" {

extern JavaVM* jvm;

}

struct Point {
	Point(int x, int y) : x(x), y(y) { }
	int x, y;
};

struct Rectangle {
	Rectangle(int x, int y, int width, int height) : x(x), y(y),
		width(width), height(height) { }
	int x, y, width, height;
};

JNIEnv* GetEnv();
void DoCallback(jobject obj, const char* name, const char* description, ...);

class BString;

jint ConvertButtonsToJava(int32 buttons);
int32 ConvertKeyCodeToNative(jint jkeycode);
void ConvertKeyCodeToJava(int32 keycode, uint32 modifiers, jint *jkeyCode, jint *jkeyLocation);
void GetKeyChar(BString* keyChar, int32 keycode, int32 modifiers);
jint ConvertModifiersToJava(uint32 modifiers);
jint ConvertInputModifiersToJava(uint32 modifiers);

#endif	/* UTILITIES_H */
