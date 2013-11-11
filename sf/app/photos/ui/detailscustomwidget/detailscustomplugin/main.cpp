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
 * Description:   Details custom widgets plugin
 *
 */

#include "glxdetailstextedit.h"
#include "glxdetailsicon.h"

#include <QtPlugin>
#include <hbdocumentloaderplugin.h>

class GlxDetailsCustomPlugin : public HbDocumentLoaderPlugin
{
public:
    QObject *createObject(const QString& type, const QString &name);
    QList<const QMetaObject *> supportedObjects();  
};
Q_EXPORT_PLUGIN(GlxDetailsCustomPlugin)

QObject *GlxDetailsCustomPlugin::createObject(const QString& type, const QString &name )
{ 
 
    if( type == GlxDetailsTextEdit::staticMetaObject.className() ) {
        QObject *object = new GlxDetailsTextEdit();
        object->setObjectName(name);
        return object;    
    }
    
    if( type == GlxDetailsIcon::staticMetaObject.className() ) {
        QObject *object = new GlxDetailsIcon();
        object->setObjectName(name);
        return object;    
    }
    
    return 0;
}

QList<const QMetaObject *> GlxDetailsCustomPlugin::supportedObjects()
{
    QList<const QMetaObject *> result;
    result.append( &GlxDetailsTextEdit::staticMetaObject );
    result.append( &GlxDetailsIcon::staticMetaObject );
    return result;
}

// end of file 

