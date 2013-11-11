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

#include "nmuiheaders.h"

/*!
    Constructor
*/
NmUiDocumentLoader::NmUiDocumentLoader(const HbMainWindow *window) : HbDocumentLoader(window)
{
    NM_FUNCTION;
}

/*!
    Destructor
*/
NmUiDocumentLoader::~NmUiDocumentLoader()
{
    NM_FUNCTION;
}

/*!
    createObject. Function returns Nmail UI specific objects from DocML
*/
QObject *NmUiDocumentLoader::createObject(const QString& type, const QString &name)
{
    NM_FUNCTION;
    
    QObject *res(NULL);
    if( type == NmMailViewerWK::staticMetaObject.className() ) {
        res = new NmMailViewerWK();
        res->setObjectName(name);
    }
    if( type == NmBaseViewScrollArea::staticMetaObject.className() ) {
        res = new NmBaseViewScrollArea();
        res->setObjectName(name);
    }
    if( type == NmViewerHeader::staticMetaObject.className() ) {
        res = new NmViewerHeader();
        res->setObjectName(name);
    }
    if( type == NmAttachmentListWidget::staticMetaObject.className() ) {
        res = new NmAttachmentListWidget();
        res->setObjectName(name);
    }    
    if( type == NmRecipientLineEdit::staticMetaObject.className() ) {
        res = new NmRecipientLineEdit();
        res->setObjectName(name);
    }
    if( type == NmHtmlLineEdit::staticMetaObject.className() ) {
        res = new NmHtmlLineEdit();
        res->setObjectName(name);
    }
    if( type == NmEditorTextEdit::staticMetaObject.className() ) {
        res = new NmEditorTextEdit();
        res->setObjectName(name);
    }
    if (!res) {
        res = HbDocumentLoader::createObject(type, name);
    }
    return res;
}
