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
#include "cntactionlauncher.h"

/*!
Action launcher. Caller should delete the instance.
*/
CntActionLauncher::CntActionLauncher( QContactManager& aContactManager, QString aAction ) : QObject(),
mAction( aAction ),
mContactAction( NULL ),
mContactManager( &aContactManager )
    {
    }

CntActionLauncher::~CntActionLauncher()
    {
    delete mContactAction;
    }
    
bool CntActionLauncher::execute( QContact aContact, QContactDetail aDetail )
    {
    bool executed = false;
    QList<QContactActionDescriptor> all = QContactAction::actionDescriptors(mAction, "symbian");
    mContactAction = QContactAction::action( all.first() );
    if ( mContactAction )
        {
        if (aContact.preferredDetail(mAction).isEmpty() && (mAction == "call" || mAction == "message" || mAction == "email"))
            {
            aContact.setPreferredDetail(mAction, aDetail);
            //return value will be ignored because we cannot do anything if it fails.
            mContactManager->saveContact(&aContact);
            }
        
        connect(mContactAction, SIGNAL(stateChanged(QContactAction::State)),
                this, SLOT(progress(QContactAction::State)));
        executed = mContactAction->invokeAction( aContact, aDetail );
        }
    else
        {
        progress(QContactAction::FinishedWithErrorState);
        }
    
    return executed;
    }

bool CntActionLauncher::execute( QContact aGrpContact, QContactDetail aDetail, QVariantMap aParameters )
    {
    bool executed = false;
    QList<QContactActionDescriptor> all = QContactAction::actionDescriptors(mAction, "symbian");
    mContactAction = QContactAction::action( all.first() );
    if ( mContactAction )
        {
        connect(mContactAction, SIGNAL(stateChanged(QContactAction::State)),
                this, SLOT(progress(QContactAction::State)));
        executed = mContactAction->invokeAction( aGrpContact, aDetail, aParameters );
        }
    else
        {
        progress(QContactAction::FinishedWithErrorState);
        }
    
    return executed;
    }
 
/*!
Launch dynamic action
*/
bool CntActionLauncher::execute( QContact aContact, QContactDetail aDetail, QContactActionDescriptor aActionDescriptor )
{
    bool executed = false;
    // detail might be empty -> in that case engine uses the preferred detail for the selected action
    mContactAction = QContactAction::action( aActionDescriptor );
    if ( mContactAction )
    {
        connect(mContactAction, SIGNAL(stateChanged(QContactAction::State)),
                this, SLOT(progress(QContactAction::State)));
        executed = mContactAction->invokeAction( aContact, aDetail );
    }
    else
    {
        progress(QContactAction::FinishedWithErrorState);
    }
    return executed;
}

void CntActionLauncher::progress( QContactAction::State status )
    {
    switch(status)
       {
       case QContactAction::FinishedState:
       case QContactAction::FinishedWithErrorState:
           emit actionExecuted( this );
           break;
       default:
           break;
       }
    }

// End of File
