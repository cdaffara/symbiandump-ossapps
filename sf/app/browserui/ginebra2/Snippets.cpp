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

#include "Snippets.h"
#include "ChromeSnippet.h"
#include "ChromeWidget.h"

namespace GVA {
  Snippets::Snippets(ChromeWidget * chrome, QObject * parent)
    : QObject(parent),
      m_chrome(chrome)
  {
    setObjectName("snippets");
  }

  Snippets::~Snippets()
  {
    // clear();
  }

  ChromeSnippet * Snippets::getSnippet(const QString & id)
  {
    return m_snippetMap.value(id);
  }

  void Snippets::addSnippet(ChromeSnippet * snippet, const QString & id)
  {
    m_snippetMap[id] = snippet;
  }

  QObjectList Snippets::getList()
  {
    QObjectList result;
    foreach(GVA::ChromeSnippet *snippet, m_snippetMap) {
      result.append(snippet);
    }
    return result;
  }

  void Snippets::clear()
  {

    foreach(ChromeSnippet *snippet, m_snippetMap){
      if (snippet){
        //If anchored, shrink the anchor bar
        m_chrome->layout()->snippetHiding(snippet);
        //Remove about-to-be-deleted snippet from scene
        m_chrome->layout()->scene()->removeItem(snippet->widget());
        delete snippet;
      }
    }

    m_snippetMap.clear();
  }

  void Snippets::dump() {
    qDebug() << "Snippets::dump: count=" << m_snippetMap.count();
    foreach(ChromeSnippet *snippet, m_snippetMap){
      snippet->dump();
    }
  }
}

