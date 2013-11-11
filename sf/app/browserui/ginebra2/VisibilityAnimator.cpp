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

#include "VisibilityAnimator.h"
#include <QTimeLine>

// NB: These includes go away when plugins are implemented

#include "animators/FadeAnimator.h"
#include "animators/SlideAnimator.h"

namespace GVA {

VisibilityAnimator::~VisibilityAnimator(){
  delete m_timeLine;
}

// NB: Replace factory implementation with hash table populated by plugin discovery

VisibilityAnimator * VisibilityAnimator:: create(const QString & name, ChromeSnippet* snippet){
  if (name.compare("G_VISIBILITY_SLIDE_ANIMATOR") == 0){
    return new SlideAnimator(snippet);
  }
  if (name.compare("G_VISIBILITY_FADE_ANIMATOR") == 0){
    return new FadeAnimator(snippet);
  }
  return 0;
}

void VisibilityAnimator::setVisible(bool visible, bool animate){
  m_visible = visible;
  if (!animate) {
    updateVisibility((m_visible)?0.0:1.0);
    return;
  }

  if (!m_timeLine) {
    m_timeLine = new QTimeLine(m_duration); //Default to .5 sec
    connect(m_timeLine, SIGNAL(valueChanged(qreal)),
        this, SLOT(updateVisibility(qreal)));
    connect(m_timeLine, SIGNAL(finished()),
        this, SLOT(onFinished()));
  }
  else {
    m_timeLine->stop();
  }

  if (!m_visible) {
    m_timeLine->setDirection(QTimeLine::Forward);
  }
  else {
    m_timeLine->setDirection(QTimeLine::Backward);
    m_timeLine->setStartFrame(m_timeLine->endFrame());
  }

  emit started(m_visible);
  m_timeLine->start();
}

void VisibilityAnimator::onFinished()
{
  emit finished(m_visible);
}
}
