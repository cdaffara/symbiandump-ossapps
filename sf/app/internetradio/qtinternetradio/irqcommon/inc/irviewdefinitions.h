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
#ifndef IRVIEWDEFINITIONS_H
#define IRVIEWDEFINITIONS_H

enum TIRViewId
{
    EIRView_InvalidId = -1,
    EIRView_MainView,
    EIRView_CategoryView,
    EIRView_StationsView,
    EIRView_PlayingView,
    EIRView_FavoritesView,
    EIRView_HistoryView,
    EIRView_StationDetailsView,
    EIRView_SearchView,
    EIRView_SettingsView,
	EIRView_OpenWebAddressView,	 
	EIRView_SongHistoryView,
	EIRView_PlsView,
	EIRView_TermsConsView,
    EIRView_ViewCount
};

enum TIRViewParameter
{
    EIRViewPara_InvalidId = -1,
    EIRViewPara_Genre,
    EIRViewPara_Language,
    EIRViewPara_Country,
    EIRViewPara_ViewCounts
};

enum TIRHandleResult
{
    EIR_DoDefault,
    EIR_NoDefault
};

enum TIRViewCommand
{
    EIR_ViewCommand_TOBEACTIVATED,
    EIR_ViewCommand_ACTIVATED,
    EIR_ViewCommand_DEACTIVATE,
    EIR_ViewCommand_MAX
};

enum TIRViewCommandReason
{
    EIR_ViewCommandReason_Show,
    EIR_ViewCommandReason_Hide,  //a new view becomes current view, old view is deactivated and push into view stack
    EIR_ViewCommandReason_Back   //current view is deactivated, the top view in view stack becomes current view
};

enum TIRUseNetworkReason
{
    EIR_UseNetwork_NoReason,
    EIR_UseNetwork_SelectItem,
    EIR_UseNetwork_StartingView,
    EIR_UseNetwork_OpenWebAddress,
    EIR_UseNetwork_StartSearch,
    EIR_UseNetwork_LoadCategory,
    EIR_UseNetwork_PlayStation
};

#endif
