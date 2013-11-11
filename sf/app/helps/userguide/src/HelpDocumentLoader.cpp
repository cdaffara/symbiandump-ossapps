/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include "HelpCategoryView.h"
#include "HelpContentsView.h"
#include "BrowserWrapper.h"

#include "HelpDocumentLoader.h"


////////////////////////////////////////////////////////////////////////////////////////////

QObject* HelpDocumentLoader::createObject(const QString& type, const QString& name)
{
    if(type == BrowserWrapper::staticMetaObject.className())    
    {
        QObject* object = new BrowserWrapper;
        object->setObjectName(name);
        return object;
    }
 
    return HbDocumentLoader::createObject(type, name);
}


////////////////////////////////////////////////////////////////////////////////////////////
void HelpUIBuilder::setObjectTree(QObjectList roots)
{
	mDocLoader.setObjectTree(roots);
}

QObjectList HelpUIBuilder::load(const QString& fileName)
{ 
    bool ok = false;
    QObjectList list = mDocLoader.load(fileName, &ok);
    Q_ASSERT_X(ok, "Help", "HelpUIBuilder load file");
    return list;
}

QObjectList HelpUIBuilder::load(const QString& fileName, const QString& section)
{
    bool ok = false;
    QObjectList list = mDocLoader.load(fileName, section, &ok);
    Q_ASSERT_X(ok, "Help", "HelpUIBuilder load section");
    return list;
}

QGraphicsWidget* HelpUIBuilder::findWidget(const QString& name)
{
    return mDocLoader.findWidget(name);
}

QObject* HelpUIBuilder::findObject(const QString& name)
{
    return mDocLoader.findObject(name);
}

// end of file

