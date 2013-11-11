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

#include <QWebHitTestResult>
#include "wrtBrowserUtils.h"

static const QString s_LinkTag = "link";
static const QString s_InputTag = "input";
static const QString s_ATag = "a";
static const QString s_HrefTag = "href";
static const QString s_AreaTag = "area";
static const QString s_TextAreaTag = "textarea";
static const QString s_ButtonTag = "button";
static const QString s_ObjectTag = "object";
static const QString s_EmbedTag = "embed";
static const QString s_ImgTag = "img";
static const QString s_SelectTag = "select";
static const QString s_LiTag = "li";
static const QString s_OlTag = "ol";
static const QString s_DlTag = "dl";
static const QString s_FormTag = "form";
static const QString s_HtmlTag = "html";

wrtBrowserDefs::BrowserElementType wrtBrowserUtils::getTypeFromElement(QWebHitTestResult htRes) 
{
	QWebElement node = htRes.element();
	wrtBrowserDefs::BrowserElementType elType = wrtBrowserDefs::EElementNone;

    if (node.isNull())
        return wrtBrowserDefs::EElementNone;

    if (node.localName().compare(s_HtmlTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementNone;
    } else if (node.tagName().isEmpty()) {
        // check whether hit test returns a navigatable element				
        if(!htRes.linkElement().isNull()) {
            elType = wrtBrowserDefs::EElementAnchor;
        }
    } else if (!node.tagName().compare(s_AreaTag, Qt::CaseInsensitive)) {
        if(!node.attribute(s_HrefTag).isNull()) {
            elType = wrtBrowserDefs::EElementAreaBox;
        } else {
            elType = wrtBrowserDefs::EElementAnchor;
        }
    } else if (!node.tagName().compare(s_ImgTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementImageBox;
        if(!htRes.linkElement().isNull()) {
            elType = wrtBrowserDefs::EElementAnchor;
        }
    } else if (!node.tagName().compare(s_LinkTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementAnchor;
	} else if (!node.tagName().compare(s_InputTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementInputBox;
    } else if (!node.tagName().compare(s_ATag, Qt::CaseInsensitive)) {
		elType = wrtBrowserDefs::EElementAnchor;        
    } else if (!node.tagName().compare(s_TextAreaTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementTextAreaBox;  
	} else if (!node.tagName().compare(s_SelectTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementSelectBox;  
	} else if (!node.tagName().compare(s_ButtonTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementButton; 
	} else if (!node.tagName().compare(s_FormTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementForm;  
	} else if (!node.tagName().compare(s_LiTag, Qt::CaseInsensitive) ||
	           !node.tagName().compare(s_OlTag, Qt::CaseInsensitive) ||
	           !node.tagName().compare(s_DlTag, Qt::CaseInsensitive)) {
        elType = wrtBrowserDefs::EElementListBox;
	} else if( !node.tagName().compare(s_ObjectTag, Qt::CaseInsensitive) ||
	           !node.tagName().compare(s_EmbedTag, Qt::CaseInsensitive)) {
		elType = wrtBrowserDefs::EElementObjectBox;        
    }

	return elType;
}
