/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include "CachedHandler.h"
#include "ChromeWidget.h"
//#include "ContentViewDelegate.h"
#include "controllableviewimpl.h"

namespace GVA {

    CachedHandler::CachedHandler(const QString & elementId, const QString & script, const QRectF & rect, ChromeWidget * chrome, const QString &targetView)
    : m_id(elementId),
      m_script(script),
      m_rect(rect),  //NB: maybe this should be a QRect
      m_chrome(chrome),
      m_targetView(targetView)
  {
  }
  //NB: Return a QVariant?
  void CachedHandler::invoke() const
  {
    ControllableViewBase *view = m_chrome->getView(m_targetView.isEmpty() ? "WebView" : m_targetView);
    if (view)
      view->triggerAction(m_script);
  }

} // end of namespace GVA

