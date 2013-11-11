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
#include <QDebug>
#include <smcmockclassincludes.h>
#include <hbinstance.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbInstance::instance
// -----------------------------------------------------------------------------
//
HbInstance * HbInstance::instance(  )
    {
    SMC_MOCK_METHOD0( HbInstance * )
    }


// -----------------------------------------------------------------------------
// HbInstance::allMainWindows
// -----------------------------------------------------------------------------
//
QList <HbMainWindow * > HbInstance::allMainWindows(  ) const
    {
    SMC_MOCK_METHOD0( QList <HbMainWindow * > )
    }


// -----------------------------------------------------------------------------
// HbInstance::style
// -----------------------------------------------------------------------------
//
HbStyle * HbInstance::style(  ) const
    {
    SMC_MOCK_METHOD0( HbStyle * )
    }


// -----------------------------------------------------------------------------
// HbInstance::theme
// -----------------------------------------------------------------------------
//
HbTheme * HbInstance::theme(  ) const
    {
    SMC_MOCK_METHOD0( HbTheme * )
    }


// -----------------------------------------------------------------------------
// HbInstance::addLibraryPath
// -----------------------------------------------------------------------------
//
void HbInstance::addLibraryPath( 
        const QString & path )
    {
    SMC_MOCK_METHOD1( void, const QString &, path )
    }


// -----------------------------------------------------------------------------
// HbInstance::removeLibraryPath
// -----------------------------------------------------------------------------
//
void HbInstance::removeLibraryPath( 
        const QString & path )
    {
    SMC_MOCK_METHOD1( void, const QString &, path )
    }


// -----------------------------------------------------------------------------
// HbInstance::libraryPaths
// -----------------------------------------------------------------------------
//
QStringList HbInstance::libraryPaths(  ) const
    {
    SMC_MOCK_METHOD0( QStringList )
    }


// -----------------------------------------------------------------------------
// HbInstance::setLibraryPaths
// -----------------------------------------------------------------------------
//
void HbInstance::setLibraryPaths( 
        const QStringList & paths )
    {
    SMC_MOCK_METHOD1( void, const QStringList &, paths )
    }


