/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <qwebsettings.h>

#include <QtGui/QIcon>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeView>
#include <QDebug>

#include "bookmarks.h"
#include "HistoryManager.h"
#include "xbel.h"

namespace WRT {

/*!
 * \class BookmarkNode
 * \brief class that represents a bookmark node 
 */

/*!
 * Bookmarks node constructor
 * @param Node type : type of the node (root , folder , leaf)
 * @param parent : parent node under which this need node is to be added
 * @see Type
 */

BookmarkNode::BookmarkNode(BookmarkNode::Type type, BookmarkNode *parent) :
     expanded(false)
   , m_parent(parent)
   , m_type(type)
{
    if (parent && m_type != Root)
        parent->add(this);
}

/*!
 * Bookmarks destructor
 */
BookmarkNode::~BookmarkNode()
{
    if (m_parent)
        m_parent->remove(this);
    qDeleteAll(m_children);
	m_children.clear();
    m_parent = 0;
    m_type = BookmarkNode::Root;
}

/*!
 * Compares give contents node with this node 
 * @return  true if the contents match else false
 */
bool BookmarkNode::operator==(const BookmarkNode &other)
{
    if (url != other.url
        || title != other.title
        || desc != other.desc
        || expanded != other.expanded
        || m_type != other.m_type
        || m_children.count() != other.m_children.count())
        return false;

    for (int i = 0; i < m_children.count(); ++i)
        if (!((*(m_children.at(i))) == (*(other.m_children.at(i)))))
            return false;
    return true;
}

/*!
 * returns  node type of this node (folder, Root etc.,)
 * @return Type : type of the node
 * @see Type
 */
BookmarkNode::Type BookmarkNode::type() const
{
    return m_type;
}

/*!
 * sets the node type of this node
 * @param type : type to be set (Root , folder etc.,)
 * @see Type
 */ 
void BookmarkNode::setType(Type type)
{
    m_type = type;
}

/*!
 * returns list of children under this node
 * @return children under this node
 */
QList<BookmarkNode *> BookmarkNode::children() const
{
    return m_children;
}

/*!
 * returns parent of this node
 * @return parent node of this node
 */
BookmarkNode *BookmarkNode::parent() const
{
    return m_parent;
}

/*!
 * Tests if this node is bookmarked
 * @return true if node is bookmarked else false
 * @see Type
 */
bool BookmarkNode::isBookmark() const
{
    return (m_type == Bookmark);
}

/*!
 * Tests if this node is of type folder
 * @return  True if this node is of type folder else  false
 * @see Type
 */
bool BookmarkNode::isFolder() const
{
    return (m_type == Folder);
}

/*!
 * adds need child node under this node 
 * Note this  node  should be of type Root
 * @param child : handle to the node which needs to be added as child
 */
void BookmarkNode::add(BookmarkNode *child, int offset)
{
    Q_ASSERT(child->m_type != Root);
    if (child->m_parent)
        child->m_parent->remove(child);
    child->m_parent = this;
    if (-1 == offset)
        offset = m_children.size();


  
    m_children.insert(offset, child);
}

/*!
 * Removes the child node from this node
 */
void BookmarkNode::remove(BookmarkNode *child)
{
    child->m_parent = 0;
    m_children.removeAll(child);
}


}
