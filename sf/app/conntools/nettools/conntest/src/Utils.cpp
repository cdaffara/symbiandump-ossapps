/*
 * Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Utils implements general tools for bandwidth and 
 * packet loss calculation and IAP selection etc
 *
 */

// INCLUDE FILES
#include <commdb.h>
#include <aknlistquerydialog.h>
#include <conntest.rsg>

#include <aknselectionlist.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <txtetext.h>

#include "Utils.h"
#include "conntest.hrh"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// Utils::Utils()
// Constructor
// ----------------------------------------------------------------------------
//
Utils::Utils()
    {
    }

// ----------------------------------------------------------------------------
// Utils::~Utils()
// Destructor
// ----------------------------------------------------------------------------
//
Utils::~Utils()
    {
    }

// ----------------------------------------------------------------------------
// Utils::CalculateThroughput
// Calculate throughput
// ----------------------------------------------------------------------------
//
void Utils::CalculateThroughput( TDes8& aBuffer, TTime aStartTime, TInt aSize )
    {
    TTime now;
    now.UniversalTime();
    TTimeIntervalMicroSeconds interval;
    interval = now.MicroSecondsFrom( aStartTime );
    TInt num = I64INT(interval.Int64());
    
    if (num != 0)
        {
        aBuffer.AppendFormat( _L8("Size: %d\n"), aSize );
        aBuffer.AppendFormat( _L8("Time: %4.1f s\n"), (TReal)num / 1000000 );
        aBuffer.AppendFormat( _L8(" -  %4.1f kB/s\n"), (TReal)((TReal)aSize / (1024.0
                * (TReal)num)) * 1000000 );
        aBuffer.AppendFormat( _L8(" -  %4.1f kbit/s\n"), (TReal)8 * ((TReal)aSize / (1024.0
                * (TReal)num)) * 1000000 );
        }
    else
        {
        aBuffer.AppendFormat( _L8("Size: %d\n"), aSize );
        aBuffer.AppendFormat( _L8("Time zero\n") );
        }
    
    }

// ----------------------------------------------------------------------------
// Utils::SetTimeoutsL(const TInt aTimeout)
// Adjust given Nifman's idle timeout for every bearer.
// ----------------------------------------------------------------------------
//
void Utils::SetTimeoutsL( const TInt aTimeout )
    {
    // GPRS and CSD bearer
    Utils::SetTableTimeoutL( aTimeout, TPtrC( MODEM_BEARER ) );
    
    // LAN bearer
    Utils::SetTableTimeoutL( aTimeout, TPtrC( LAN_BEARER ) );
    }

// ----------------------------------------------------------------------------
// Utils::SetTableTimeoutL()
// Set Nifman's idle times for each record in given table.
// ----------------------------------------------------------------------------
//
void Utils::SetTableTimeoutL( const TInt aTimeout, const TDesC& aTableName )
    {
    TInt result;
    TUint32 value;
    TBuf<64> title;
    
    CCommsDatabase* TheDb;
    
    TheDb = CCommsDatabase::NewL();
    CleanupStack::PushL( TheDb );

    CCommsDbTableView* view2 = TheDb->OpenTableLC( aTableName );
    
    result = view2->GotoFirstRecord();
    User::LeaveIfError( result );

    while (result == KErrNone)
        {
        view2->ReadTextL( TPtrC( COMMDB_NAME ), title );

        switch (aTimeout)
            {
            case EConnTestCmdTimeoutLastSocketActivity:
                title.Append( _L("\nLastSocketActivity") );
                view2->ReadUintL( TPtrC( LAST_SOCKET_ACTIVITY_TIMEOUT ), value );
                break;
                
            case EConnTestCmdTimeoutLastSocketClosed:
                title.Append( _L("\nLastSocketClosed") );
                view2->ReadUintL( TPtrC( LAST_SOCKET_CLOSED_TIMEOUT ), value );
                break;
                
            case EConnTestCmdTimeoutLastSessionClosed:
                title.Append( _L("\nLastSessionClosed") );
                view2->ReadUintL( TPtrC( LAST_SESSION_CLOSED_TIMEOUT ), value );
                break;
                
            default:
                value = 0; // To get rid of compiler warning
                break;
            }

        TInt val2( value );
        CAknNumberQueryDialog* dlg;
        
        dlg = CAknNumberQueryDialog::NewL( val2, CAknQueryDialog::ENoTone );
        CleanupStack::PushL( dlg );
        dlg->SetPromptL( title );
        TInt changed = dlg->ExecuteLD( R_CONNTEST_NUMBER_LAYOUT );
        CleanupStack::Pop( dlg );

        if (changed)
            {
            result = view2->UpdateRecord();
            User::LeaveIfError( result );

            if (val2 == -1)
                {
                val2 = KMaxTInt;
                }

            switch (aTimeout)
                {
                case EConnTestCmdTimeoutLastSocketActivity:
                    view2->WriteUintL( TPtrC( LAST_SOCKET_ACTIVITY_TIMEOUT ),
                            val2 );
                    break;
                    
                case EConnTestCmdTimeoutLastSocketClosed:
                    view2->WriteUintL( TPtrC( LAST_SOCKET_CLOSED_TIMEOUT ),
                            val2 );
                    break;
                    
                case EConnTestCmdTimeoutLastSessionClosed:
                    view2->WriteUintL( TPtrC( LAST_SESSION_CLOSED_TIMEOUT ),
                            val2 );
                    break;
                    
                default:
                    break;
                }

            result = view2->PutRecordChanges();
            User::LeaveIfError( result );
            }
        result = view2->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy( view2 ); // view2
    CleanupStack::PopAndDestroy( TheDb ); // TheDb
    }

// ----------------------------------------------------------------------------
// Utils::SetConnectionPrefsIap()
// Set IapId value into connection preferences table
// ----------------------------------------------------------------------------
//
void Utils::SetConnectionPrefsIapL()
    {
    
    CCommsDatabase* TheDb;
    
    TheDb = CCommsDatabase::NewL();
    CleanupStack::PushL( TheDb );

    CCommsDbConnectionPrefTableView* view = TheDb->OpenConnectionPrefTableLC();
    
    TInt ret = view->GotoFirstRecord();
    
    CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref tablePref;
    
    if (ret == KErrNone)
        {
        view->ReadConnectionPreferenceL( tablePref );
        }

    User::LeaveIfError( ret );

    TInt value( tablePref.iBearer.iIapId );
    CAknNumberQueryDialog* dlg;
    
    dlg = CAknNumberQueryDialog::NewL( value, CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dlg );
    dlg->SetPromptL( _L("Iap Id:") );
    TInt changed = dlg->ExecuteLD( R_CONNTEST_NUMBER_LAYOUT );
    CleanupStack::Pop( dlg );

    if (changed)
        {
        tablePref.iBearer.iIapId = value;
        view->UpdateBearerL( tablePref.iBearer );
        }

    CleanupStack::PopAndDestroy( view ); // view
    CleanupStack::PopAndDestroy( TheDb ); // TheDb
    }

// ----------------------------------------------------------------------------
// Utils::AskIapIdL(TUint32& aId)
// Ask IAP id from the user
// ----------------------------------------------------------------------------
//
TInt Utils::AskIapIdL( TUint32& aId )
    {
    return Utils::SelectIdFromTableL( aId, TPtrC( IAP ) );
    }

// ----------------------------------------------------------------------------
// Utils::AskNetworkIdL(TUint32& aId)
// Ask Network id from the user
// ----------------------------------------------------------------------------
//
TInt Utils::AskNetworkIdL( TUint32& aId )
    {
    return Utils::SelectIdFromTableL( aId, TPtrC( NETWORK ) );
    }

// ----------------------------------------------------------------------------
// Utils::SelectIdFromTableL()
// Does the real work for selecting IAP or Network id
// ----------------------------------------------------------------------------
//
TInt Utils::SelectIdFromTableL( TUint32& aId, const TDesC& aTableName )
    {
    CCommsDatabase* TheDb;
    TInt res;
    TBuf<40> name;
    TBuf<20> title;
    TUint32 id;
    RArray<TUint32> idArray;
    CDesCArrayFlat* items = new (ELeave) CDesCArrayFlat( 16 );
    
    // Get IAP names and ids from the database
    TheDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( TheDb );

    TheDb->ShowHiddenRecords();

    CCommsDbTableView* view;
    
    view = TheDb->OpenTableLC( aTableName );
    title.Format( _L("Select %S:"), &aTableName );

    res = view->GotoFirstRecord();

    while (res == KErrNone)
        {
        view->ReadTextL( TPtrC( COMMDB_NAME ), name );
        view->ReadUintL( TPtrC( COMMDB_ID ), id );

        idArray.Insert( id, 0 );
        items->InsertL( 0, name );

        res = view->GotoNextRecord();
        RDebug::Print( _L("ConnTest: IAP name, id: %S, %d"), &name, id );
        }

    CleanupStack::PopAndDestroy( view ); // view
    CleanupStack::PopAndDestroy( TheDb ); // TheDb

    // Create listbox and PUSH it.
    CEikTextListBox* list = new (ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( list );

    // Create popup list and PUSH it.
    CAknPopupList* popupList = CAknPopupList::NewL( list,
            R_AVKON_SOFTKEYS_OK_CANCEL, AknPopupLayouts::EMenuWindow );
    CleanupStack::PushL( popupList );

    // initialize listbox.
    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto );

    // Set listitems.
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );

    // Set title
    popupList->SetTitleL( title );

    // Show popup list.
    TInt popupOk = popupList->ExecuteLD();
    CleanupStack::Pop( popupList ); // popuplist
    if (popupOk)
        {
        TInt index = list->CurrentItemIndex();
        aId = idArray[index];
        }

    CleanupStack::PopAndDestroy( list ); // list

    idArray.Close();
    //    delete items; (list deletes)
    items = NULL;
    return popupOk;
    }

// ----------------------------------------------------------------------------
// Utils::CreateDataChunk()
// Create data chunk for POST performance tests
// ----------------------------------------------------------------------------
//
void Utils::CreateDataChunk( TDes8& aBuffer, TInt aLength )
    {
    for (TInt i = 0; i < aLength && i < aBuffer.MaxLength(); i++)
        {
        if (i == 0)
            {
            aBuffer.Copy( _L8("a") );
            }
        else
            {
            aBuffer.Append( _L8("a") );
            }
        }
    }

