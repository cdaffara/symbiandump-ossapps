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

#ifndef __GINEBRA_CACHEDHANDLER_H__
#define __GINEBRA_CACHEDHANDLER_H__

#include <QString>
#include <QRect>

namespace GVA {

  class ChromeWidget;

  class CachedHandler
  {
  public:
    CachedHandler(const QString & elementId, const QString & script, const QRectF & rect, ChromeWidget * m_chrome, const QString &targetView);
    QString elementId() const {return m_id;}
    QString script() const {return m_script;}
    inline const QRectF & rect() const {return m_rect;}
    void invoke() const;
  private:
    QString m_id;
    QString m_script;
    QRectF m_rect;
    ChromeWidget * m_chrome;
    QString m_targetView;
  };

} // end of namespace GVA

#endif // __GINEBRA_CACHEDHANDLER_H__
