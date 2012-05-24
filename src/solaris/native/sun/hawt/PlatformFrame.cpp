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

#include "PlatformFrame.h"

#include <stdio.h>

/*
 * TODO:
 * Get/set maximize bounds
 * Z order toBack/Front
 * Blocking
 */

PlatformFrame::PlatformFrame(jobject platformWindow, bool decorated)
	:
	BWindow(BRect(0, 0, 0, 0), NULL, decorated ? B_TITLED_WINDOW_LOOK
		: B_NO_BORDER_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 0),
	fView(platformWindow, true),
	fPlatformWindow(platformWindow)
{
	AddChild(&fView);

	// After this initial bounds set the view will size itself
	// to match the frame
	BRect frame = Bounds();
	fView.SetBounds(Rectangle(frame.left, frame.top, frame.IntegerWidth() + 1,
		frame.IntegerHeight() + 1));
}


// #pragma mark PlatformWindow functions


Rectangle
PlatformFrame::GetBounds()
{
	if (!LockLooper())
		return Rectangle(0, 0, 0, 0);
	BRect frame = Frame();
	UnlockLooper();

	return Rectangle(frame.left, frame.top, frame.IntegerWidth() + 1,
		frame.IntegerHeight() + 1);
}


PlatformView*
PlatformFrame::GetContainer()
{
	return &fView;
}


Drawable*
PlatformFrame::GetDrawable()
{
	return fView.GetDrawable();
}


Point
PlatformFrame::GetLocation()
{
	Rectangle bounds = GetBounds();
	return Point(bounds.x, bounds.y);
}


Point
PlatformFrame::GetLocationOnScreen()
{
	return GetLocation();
}


int
PlatformFrame::GetState()
{
	if (!LockLooper())
		return 0;

	int state = 0;
	if (IsHidden() || IsMinimized())
		state |= kStateMinimized;
	if (fMaximized)
		state |= kStateMaximized;

	if (state == 0)
		state |= kStateNormal;

	UnlockLooper();
	return state;
}


void
PlatformFrame::SetBounds(Rectangle bounds)
{
	if (!LockLooper())
		return;
	MoveTo(bounds.x, bounds.y);
	ResizeTo(bounds.width - 1, bounds.height - 1);
	UnlockLooper();
}


void
PlatformFrame::SetParent(PlatformView* parent)
{
	fprintf(stderr, "Attempted to parent a frame");
}


void
PlatformFrame::SetResizable(bool resizable)
{
	if (!LockLooper())
		return;
	if (resizable)
		SetFlags(Flags() | B_NOT_RESIZABLE);
	else
		SetFlags(Flags() & ~B_NOT_RESIZABLE);
	UnlockLooper();
}


void
PlatformFrame::SetState(int state)
{
	if (!LockLooper())
		return;

	// Should a maximize cancel out a minimize?
	// Or should it be 'behind-the-scenes' maximized,
	// so it shows as maximized when it becomes unminimized?
	
	if ((state & kStateMinimized) != 0)
		Minimize(true);
	if ((state & kStateMaximized) != 0)
		BWindow::Zoom();

	// Normal should cancel out the two other states
	if ((state & kStateNormal) != 0) {
		Minimize(false);
		if (fMaximized)
			BWindow::Zoom();
	}
	UnlockLooper();
}


bool
PlatformFrame::GetVisible()
{
	if (!LockLooper())
		return false;

	bool visible = !IsHidden();
	UnlockLooper();
	return visible;
}


void
PlatformFrame::SetVisible(bool visible)
{
	if (!LockLooper())
		return;

	if (visible) {
		while (IsHidden())
			Show();
	} else {
		while (!IsHidden())
			Hide();
	}
	UnlockLooper();
}


void
PlatformFrame::Dispose()
{
	LockLooper();
	// Remove the root view first, otherwise it
	// gets deleted twice.
	RemoveChild(&fView);
	Quit();
}


void
PlatformFrame::Focus()
{
	LockLooper();
	// Focus the root view
	fView.Focus();
	UnlockLooper();
}


// #pragma mark Overriden BWindow/BLooper hooks


void
PlatformFrame::FrameMoved(BPoint origin)
{
	int x = origin.x;
	int y = origin.y;
	UnlockLooper();
	DoCallback(fPlatformWindow, "eventMove", "(II)V", x, y);
	LockLooper();
	BWindow::FrameMoved(origin);
}


void
PlatformFrame::FrameResized(float width, float height)
{
	int w = width + 1;
	int h = height + 1;
	UnlockLooper();
	DoCallback(fPlatformWindow, "eventResize", "(II)V", w, h);
	LockLooper();
	BWindow::FrameResized(width, height);
}


void
PlatformFrame::Minimize(bool minimize)
{
	DoCallback(fPlatformWindow, "eventMinimize", "(Z)V", minimize);
	BWindow::Minimize(minimize);
}


bool
PlatformFrame::QuitRequested()
{
	DoCallback(fPlatformWindow, "eventWindowClosing", "()V");
	
	// According to WindowEvent docs, we should ignore the
	// user's request to quit and send an event to the peer.
	// AWT will then decide what to do.
	return false;
}


void
PlatformFrame::WindowActivated(bool active)
{
	// I think we should only handle this in PlatformView?
	// DoCallback(fPlatformWindow, "eventActivate", "(Z)V", active);
	BWindow::WindowActivated(active);
}


void
PlatformFrame::Zoom(BPoint origin, float width, float height)
{
	// For whatever reason, there is no getter for this
	// so we record the state ourselves.
	fMaximized = !fMaximized;
	DoCallback(fPlatformWindow, "eventMaximize", "(Z)V", fMaximized);
	
	BWindow::Zoom(origin, width, height);
}
