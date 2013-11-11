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

#ifndef __GINEBRA_PROGRESSBARITEM_H__
#define __GINEBRA_PROGRESSBARITEM_H__

#include <QtGui>
#include "NativeChromeItem.h"

namespace GVA {

  /*!
   * Draws a simple progress bar that is driven by loadProgress signals from
   * the page controller.
   */
  class ProgressBarItem : public NativeChromeItem
  {
    Q_OBJECT
  public:
    ProgressBarItem(ChromeSnippet * snippet, QGraphicsItem* parent = 0);
    virtual ~ProgressBarItem();
    qreal getProgress() const { return m_progress; }
    void setProgress(qreal value);
  public slots:
    void onProgress(int progress);
    void onStart();
    void onFinished(bool ok);
  protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget);
    void connectAll();
  private:
    qreal m_progress;
    QColor m_color;
  };

} // end of namespace GVA

#endif // __GINEBRA_PROGRESSBARITEM_H__
