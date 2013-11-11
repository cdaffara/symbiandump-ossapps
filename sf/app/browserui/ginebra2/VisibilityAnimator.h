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

#ifndef __GINEBRA_VISIBILITYANIMATOR_H__
#define __GINEBRA_VISIBILITYANIMATOR_H__

#include <QObject>

class QTimeLine;

namespace GVA {
class ChromeSnippet;

/*!  \ingroup JavascriptAPI
 * \brief Base class for visibility animation delegates
 *
 * Example javascript code to set a fade animation with the default 500ms duration on a snippet.
 * This will cause the snippet to fade-in when it is shown and fade-out when it is hidden.
 * \code
 * snippets.SnippetId.setVisibilityAnimator("G_VISIBILITY_FADE_ANIMATOR");
 * \endcode
 * Example javascript code to set a fade animation with a 200ms duration on a snippet.
 * \code
 * snippets.SnippetId.setVisibilityAnimator("G_VISIBILITY_FADE_ANIMATOR").duration = 200;
 * \endcode
 * \sa ChromeSnippet::setVisibilityAnimator()
 */

class VisibilityAnimator : public QObject
{
   Q_OBJECT

 public:
  VisibilityAnimator(ChromeSnippet* snippet) : m_snippet(snippet), m_timeLine(0), m_visible(true), m_duration(500){}
  virtual ~VisibilityAnimator();
  static VisibilityAnimator * create(const QString & name, ChromeSnippet* snippet);
  void setVisible(bool visible, bool animate = true);
  void setTimeline(QTimeLine * timeline){m_timeLine = timeline;}
  void setDuration(uint value) { m_duration = value; }
  uint duration() const { return m_duration; }
  Q_PROPERTY(uint duration READ duration WRITE setDuration)
 public slots:
  virtual void updateVisibility(qreal step) = 0;
 private slots:
  void onFinished();
 signals:
  void started(bool visible);
  void finished(bool visible);
  void moving(qreal delta);
 protected:
  ChromeSnippet* m_snippet;
  QTimeLine *m_timeLine;
  bool m_visible;
  uint m_duration;
};

}
#endif
