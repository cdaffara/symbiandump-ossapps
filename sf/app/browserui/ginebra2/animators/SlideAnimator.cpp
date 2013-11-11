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

#include "SlideAnimator.h"
#include "../ChromeSnippet.h"
#include <QList>
#include <QDebug>

namespace GVA {

SlideAnimator::SlideAnimator(ChromeSnippet* m_snippet)
  : VisibilityAnimator(m_snippet)
{
  //  m_visiblePosition = m_snippet->widget()->pos();
  setDirection("up");
  m_originalTransform = m_snippet->widget()->transform();
}

SlideAnimator::~SlideAnimator()
{
}

void SlideAnimator::setDirection(const QString & direction)
{
  m_direction = direction;

  if (m_direction == "up"){
    m_xRange = 0;
    m_yRange = -m_snippet->widget()->size().height();
  }
  else if (m_direction == "down"){
    m_xRange = 0;
    m_yRange = m_snippet->widget()->size().height();
  }
  else if (m_direction == "left"){
    m_xRange = -m_snippet->widget()->size().width();
    m_yRange = 0;
  }
  else if (m_direction == "right"){
    m_xRange = m_snippet->widget()->size().width();
    m_yRange = 0;
  }
}

void SlideAnimator::updateVisibility(qreal step)
{
  QTransform transform = m_originalTransform;
  transform.translate(step*m_xRange, step*m_yRange);
  m_snippet->widget()->setTransform(transform);
  emit moving(step);
}

}
