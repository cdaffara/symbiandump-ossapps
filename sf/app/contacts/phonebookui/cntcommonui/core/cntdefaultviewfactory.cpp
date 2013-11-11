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

#include "cntabstractviewfactory.h"
#include "cntdetaileditor.h"
#include "cntmainwindow.h"
#include <cntviewparams.h>
#include <cntviewsupplier.h>
#include <cntuiextensionfactory.h>
#include <hbstyleloader.h>
// views
#include "cntnamesview.h"
#include "cntmycardview.h"
#include "cntfavoritesview.h"
#include "cntimageeditorview.h"
#include "cntcollectionview.h"
#include "cntgroupmemberview.h"
#include "cntfavoritesmemberview.h"
#include "cntcontactcardview.h"
#include "cntgroupactionsview.h"
#include "cnthistoryview.h"
#include "cnteditview.h"
#include "cntimportsview.h"
#include "cntsettingsview.h"
#include "cntextensionmanager.h"

CntDefaultViewFactory::CntDefaultViewFactory( CntExtensionManager& aExt ):
    mExtensionManager( aExt )
{
    HbStyleLoader::registerFilePath( ":/style" );
}

CntDefaultViewFactory::~CntDefaultViewFactory()
{
    
}

CntAbstractView* CntDefaultViewFactory::createView( int aId )
{
    switch ( aId )
    {
    case namesView:
        return new CntNamesView();
    case contactCardView:
        return new CntContactCardView();
    case myCardView:
        return new CntMyCardView();
    case collectionView:
        return new CntCollectionView();
    case collectionFavoritesView:
        return new CntFavoritesView();
    case favoritesMemberView:
        return new CntFavoritesMemberView();
    case editView:
		return new CntEditView();
    case onlineAccountEditorView:
        break;
  
    case emailEditorView:
    case namesEditorView:
    case urlEditorView:
    case companyEditorView:
    case phoneNumberEditorView:
    case noteEditorView:
    case familyDetailEditorView:
    case addressEditorView:
    case dateEditorView:
    case groupEditorView:
        return new CntDetailEditor(aId);

    case imageEditorView:
        return new CntImageEditorView();
    case groupMemberView:
        return new CntGroupMemberView();
    case groupActionsView:
        return new CntGroupActionsView();
    case historyView:
        return new CntHistoryView();
    case importsView:
        return new CntImportsView();
    case settingsView:
        return new CntSettingsView();
    default:
       return createPluginView(aId);
    }
    return NULL;
}

CntAbstractView* CntDefaultViewFactory::createPluginView( int aId )
{
    for(int i = 0;i < mExtensionManager.pluginCount();i++)
    {
        CntViewSupplier* viewSupplier = mExtensionManager.pluginAt(i)->viewSupplier();
        if (viewSupplier)
        {
            for(int j = 0;j < viewSupplier->viewCount();j++)
            {
                if (viewSupplier->viewIdAt(j) == aId)
                {
                    return viewSupplier->viewAt(j);
                }
            }
        }
    }
    return NULL;
}

// End of File
