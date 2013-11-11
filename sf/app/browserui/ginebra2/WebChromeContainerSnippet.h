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
#ifndef __GINEBRA_WEBCHROMECONTAINERSNIPPET_H__
#define __GINEBRA_WEBCHROMECONTAINERSNIPPET_H__

#include <QtGui>
#include "ChromeSnippet.h"

namespace GVA {

  class ChromeWidget;

  class WebChromeContainerSnippet : public ChromeSnippet
  {
    Q_OBJECT
  public:
    WebChromeContainerSnippet(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
    virtual ~WebChromeContainerSnippet();
    virtual void setChromeWidget(QGraphicsWidget * widget);
    virtual void addChild(ChromeSnippet * child);
    QGraphicsLinearLayout* layout() { return m_layout;}

  Q_SIGNALS:
    void childAdded(ChromeSnippet * );
  public slots:
    void setLayoutHeight(int height);
    void setLayoutWidth(qreal width, bool update = false);
    virtual void updateSize(QSize );
    void updateSizes();
    QRectF ownerArea() { return QRectF(m_element.geometry());}
    virtual void updateOwnerArea();

  protected slots:
    void positionChildren();

  private:
    qreal m_layoutHeight;
    qreal m_layoutWidth;
    QGraphicsLinearLayout * m_layout;
  };

} // end of namespace GVA

#endif // __GINEBRA_WEBCHROMECONTAINERSNIPPET_H__
