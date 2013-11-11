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

#include "FadeAnimator.h"
#include "../ChromeSnippet.h"
#include <QList>
#include <QDebug>

namespace GVA {

FadeAnimator::FadeAnimator(ChromeSnippet* m_snippet)
  : VisibilityAnimator(m_snippet),
    m_minOpacity(0.1)
{
  // If the snippet is currently hidden, it will be shown
  // before the animator starts. It should therefore show
  // with minimum opacity.

  if (m_snippet->widget() && !m_snippet->widget()->isVisible())
    m_snippet->widget()->setOpacity(m_minOpacity);
}

FadeAnimator::~FadeAnimator()
{
}

void FadeAnimator::updateVisibility(qreal step)
{
  qreal o = 1.0 - step;
  m_snippet->widget()->setOpacity((o > m_minOpacity)?o:m_minOpacity);
}

}
