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

#ifndef __GINEBRA_PROGRESSBARSNIPPET_H__
#define __GINEBRA_PROGRESSBARSNIPPET_H__

#include "ChromeSnippet.h"
#include "ChromeWidget.h"
#include "ProgressBarItem.h"

namespace GVA {

  class ProgressBarSnippet : public ChromeSnippet
  {
    Q_OBJECT
    public:
       ProgressBarSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element)
        : ChromeSnippet( elementId, chrome, widget, element )
        {}
       virtual ~ProgressBarSnippet() {;}

       Q_PROPERTY(qreal progress READ getProgress WRITE setProgress)

    protected:
       qreal getProgress() const { return static_cast<ProgressBarItem*>(m_widget)->getProgress(); }
       void setProgress(qreal value) { static_cast<ProgressBarItem*>(m_widget)->setProgress(value); }
  };

}

#endif // __GINEBRA_PROGRESSBARSNIPPET_H__
