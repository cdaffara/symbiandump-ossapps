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

#ifndef __GINEBRA_SLIDEANIMATOR_H__
#define __GINEBRA_SLIDEANIMATOR_H__

#include <QObject>
#include <QTransform>

#include "../VisibilityAnimator.h"

namespace GVA {
class ChromeSnippet;

class SlideAnimator : public VisibilityAnimator
{
   Q_OBJECT

 public:
  SlideAnimator(ChromeSnippet* snippet);
  virtual ~SlideAnimator();
  void setDirection(const QString& direction);
 public slots:
  virtual void updateVisibility(qreal step);
 private:
  QString m_direction;
  //QPoint m_visiblePos;
  QTransform m_originalTransform;
  qreal m_xRange;
  qreal m_yRange;
};
}
#endif
