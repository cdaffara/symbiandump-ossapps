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
* Description:  Homescreen document loader class.
*
*/

#include "hsdocumentloader.h"
#include "hspageindicator.h"
#include "hstrashbinwidget.h"
#include "hsconfiguration.h"

/*!
    \class HsDocumentLoader
    \ingroup group_hshomescreenstateplugin
    \brief Homescreen document loader class.

    Loads own widgets from docml.
*/

/*!
    Constructor.
*/
HsDocumentLoader::HsDocumentLoader()
    : HbDocumentLoader()
{
}

/*!
    Destructor.
*/
HsDocumentLoader::~HsDocumentLoader()
{
}

/*!
    Looks up an object which inherits class type and of which object name equals to name.
    \a type type of the desired object.
    \a name object name of the desired object.
    */
QObject *HsDocumentLoader::createObject(const QString &type, const QString &name)
{
    if (type == HsPageIndicator::staticMetaObject.className()) {
        QObject *object = new HsPageIndicator(HSCONFIGURATION_GET(pageIndicatorSpacing));
        object->setObjectName(name);
        return object;
    }
    else if (type == HsTrashBinWidget::staticMetaObject.className()) {
        QObject *object = new HsTrashBinWidget;
        object->setObjectName(name);
        return object;
    }
        
    return HbDocumentLoader::createObject(type, name);
}
