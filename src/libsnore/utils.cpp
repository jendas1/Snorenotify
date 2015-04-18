/*
    SnoreNotify is a Notification Framework based on Qt
    Copyright (C) 2015  Patrick von Reth <vonreth@kde.org>

    SnoreNotify is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SnoreNotify is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with SnoreNotify.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "utils.h"
#include "log.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <winuser.h>
#endif

using namespace Snore;

Utils::Utils(QObject *parent):
    QObject(parent)
{

}

Utils::~Utils()
{

}

void Utils::bringWindowToFront(qlonglong _wid, bool focus)
{
#ifdef Q_OS_WIN
    HWND wid = (HWND)_wid;
    int active = attatchToActiveProcess();
    SetForegroundWindow(wid);
    if (focus) {
        SetFocus(wid);
    }
    detatchActiveProcess(active);
#else
    Q_UNUSED(_wid);
    Q_UNUSED(focus);
#endif
}

void Utils::raiseWindowToFront(qlonglong wid)
{
    // Looks like qt is handling it on linux.
#ifdef Q_OS_WIN
    int active = attatchToActiveProcess();
    SetWindowPos((HWND)wid, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    detatchActiveProcess(active);
#else
    Q_UNUSED(wid);
#endif
}

#ifdef Q_OS_WIN
int Utils::attatchToActiveProcess()
{
    int idActive = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
    return AttachThreadInput(GetCurrentThreadId(), idActive, TRUE) ? idActive : -1;
}

void Utils::detatchActiveProcess(int idActive)
{
    if (idActive!= -1) {
        AttachThreadInput(GetCurrentThreadId(), idActive, FALSE);
    }
}

#endif