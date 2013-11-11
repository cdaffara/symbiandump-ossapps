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
#ifndef __gva_snippets_h__
#define __gva_snippets_h__

#include <QObject>
#include <QMap>


namespace GVA{

  class ChromeSnippet;
  class ChromeWidget;

  /*! \ingroup JavascriptAPI
   * \brief This class maintains the list of all ChromeSnippets.
   *
   * Javascript object name: "snippets".
   *
   * Example javascript code to toggle the visibility of the snippet defined ablove:
   * \code
   * snippets.StatusBarChromeId.toggleVisibility();
   * \endcode
   * \sa ChromeSnippet
   */
  class Snippets : public QObject
  {
  public:
    Snippets(ChromeWidget * chrome, QObject * parent);
    virtual ~Snippets();
    ChromeSnippet * getSnippet(const QString & id);
    void addSnippet(ChromeSnippet *, const QString & id);
    QObjectList getList();
    void clear();
    void dump();
  private:
    QMap<QString, ChromeSnippet *> m_snippetMap;
    ChromeWidget * m_chrome;
  };
}

#endif
