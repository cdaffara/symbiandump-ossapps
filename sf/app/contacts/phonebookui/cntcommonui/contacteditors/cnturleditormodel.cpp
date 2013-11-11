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
#include "cnturleditormodel.h"
#include "cntdetailmodelitem.h"
#include "cntdetailorderinghelper.h"
#include <qcontacturl.h>

CntUrlEditorModel::CntUrlEditorModel(QContact* aContact) :
    CntDetailEditorModel(aContact)
{
    mUrlList = CntDetailOrderingHelper::getOrderedUrls(*mContact);
    if (mUrlList.isEmpty()) {
        QContactUrl url;
        url.setSubType(QContactUrl::SubTypeHomePage);
        mUrlList.append(url);
    }

    foreach( QContactUrl contactUrl, mUrlList ) {
        CntDetailModelItem* item = new CntDetailModelItem(contactUrl);
        appendDataFormItem(item, invisibleRootItem());
        connect( item, SIGNAL(itemDataChanged(CntDetailModelItem*)), this, SLOT(emitDataChanged(CntDetailModelItem*)) );
    }
}

CntUrlEditorModel::~CntUrlEditorModel()
{
}

void CntUrlEditorModel::insertDetailField()
{
    QContactUrl url;
    url.setSubType(QContactUrl::SubTypeHomePage);
    mUrlList.append( url );
    CntDetailModelItem* newUrl = new CntDetailModelItem(url);
    appendDataFormItem( newUrl, invisibleRootItem() );
    emitDataChanged(newUrl);
}

void CntUrlEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();

    int count(root->childCount());
    for (int i(0); i < count; i++) {
        CntDetailModelItem* detail = static_cast<CntDetailModelItem*> (root->childAt(i));
        QContactDetail url = detail->detail();
        
        if ( !mUrlList.contains(url) )
        {
            mContact->saveDetail( &url );
        }
        
        if ( url.value(QContactUrl::FieldUrl).isEmpty() )
        {
            mContact->removeDetail( &url );
        }
    }
}

QContactDetail CntUrlEditorModel::detail() const 
{
    QListIterator<QContactUrl> urlList(mContact->details<QContactUrl>());
    urlList.toBack(); // go through backwards, so the newest item will be returned
    while ( urlList.hasPrevious() )
    {
        QContactUrl url = urlList.previous();
        if ( !url.value(QContactUrl::FieldUrl).isEmpty() )
        {
            return url;
        }
    }
    return QContactEmailAddress(); // return empty address if none found
}

// End of File

