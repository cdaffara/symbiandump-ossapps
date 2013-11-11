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

#include "MemSpyViewMainMenu.h"

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyenginehelperprocess.h>

// User includes
#include "MemSpyContainerObserver.h"
#include "MemSpyViewProcesses.h"
#include "MemSpyViewRAMInfo.h"
#include "MemSpyViewROMInfo.h"
#include "MemSpyViewServerList.h"
#include "MemSpyViewOpenFiles.h"
#include "MemSpyViewFBServBitmaps.h"
#include "MemSpyViewSystemConfig.h"
#include "MemSpyViewCodeSegList.h"
#include "MemSpyViewChunkList.h"
#include "MemSpyViewKernel.h"
#include "MemSpyViewHeapTracking.h"
#include "MemSpyViewDriveInfo.h"
#include "MemSpyViewECom.h"
#include "MemSpyViewMemoryTrackingAutoStartConfig.h"
#include "MemSpyViewWindowGroups.h"

// System includes
#include <aknnotewrappers.h>

/*
CMemSpyViewMainMenu::CMemSpyViewMainMenu( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aEngine, aObserver )
    {
    }
*/
CMemSpyViewMainMenu::CMemSpyViewMainMenu( RMemSpySession& aSession, MMemSpyViewObserver& aObserver )
:   CMemSpyViewBase( aSession, aObserver )
    {
    }

void CMemSpyViewMainMenu::ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune )
    {
    _LIT( KTitle, "MemSpy" );
    SetTitleL( KTitle );
    //
    CMemSpyViewBase::ConstructL( aRect, aContainer, aSelectionRune );
    
    // Make sure the correct item is selected
    const TMemSpyViewType viewType = (TMemSpyViewType) ((TInt) aSelectionRune);
    const TInt index = IndexByViewType( viewType );
    iListBox->SetCurrentItemIndex( index );
    HandleListBoxItemSelectedL( index );
    }


void CMemSpyViewMainMenu::RefreshL()
    {
    SetListBoxModelL();
    CMemSpyViewBase::RefreshL();
    }


TMemSpyViewType CMemSpyViewMainMenu::ViewType() const
    {
    return EMemSpyViewTypeMainMenu;
    }


CMemSpyViewBase* CMemSpyViewMainMenu::PrepareChildViewL()
    {
    CMemSpyViewBase* child = NULL;
    const TInt index = iListBox->CurrentItemIndex();
    
#ifdef __WINS__
    // threads & process view is not supported in the emulator environment    
    if  ( index == 0 )
        {
        CAknInformationNote* note = new (ELeave) CAknInformationNote;
        note->ExecuteLD(_L("Not supported in the emulator, try Kernel -> Objects view"));
        return NULL;
        }
#endif

    if  ( index == 0 )
        {
        child = new(ELeave) CMemSpyViewProcesses( iMemSpySession, iObserver );		
        }
    else if ( index == 1 )
        {
        //child = new(ELeave) CMemSpyViewRAMInfo( iEngine, iObserver );
		child = new(ELeave) CMemSpyViewKernel( iMemSpySession, iObserver );
        }
    else if ( index == 2 )
        {
		child = new(ELeave) CMemSpyViewHeapTracking( iMemSpySession, iObserver );
        }
    else if ( index == 3 )
            {
            child = new(ELeave) CMemSpyViewServerList( iMemSpySession, iObserver );
            }
        
    /*
    else if ( index == 2 )
        {
        child = new(ELeave) CMemSpyViewROMInfo( iEngine, iObserver );
        }
    else if ( index == 3 )
        {
        child = new(ELeave) CMemSpyViewFBServBitmaps( iEngine, iObserver );
        }
    else if ( index == 4 )
        {
        child = new(ELeave) CMemSpyViewServerList( iEngine, iObserver );
        }
    else if ( index == 5 )
        {
        child = new(ELeave) CMemSpyViewChunkList( iEngine, iObserver );
        }
    else if ( index == 6 )
        {
        child = new(ELeave) CMemSpyViewCodeSegList( iEngine, iObserver );
        }
    else if ( index == 7 )
        {
        child = new(ELeave) CMemSpyViewOpenFiles( iEngine, iObserver );
        }
    else if ( index == 8 )
        {
        child = new(ELeave) CMemSpyViewSystemConfig( iEngine, iObserver );
        }    
    else if ( index == 9 )
        {
        child = new(ELeave) CMemSpyViewKernel( iEngine, iObserver );		       
        }       
    else if ( index == 10 )
        {
        child = new(ELeave) CMemSpyViewHeapTracking( iEngine, iObserver );
        }
     */    
    /*
    else if ( index == 11 )
        {
        child = new(ELeave) CMemSpyViewMemoryTrackingAutoStartConfig( iEngine, iObserver );
        }
    else if ( index == 12 )
        {
        child = new(ELeave) CMemSpyViewDriveList( iEngine, iObserver );
        }
    else if ( index == 13 )
        {
        child = new(ELeave) CMemSpyViewECom( iEngine, iObserver );
        }
    else if ( index == 14 )
        {
        child = new(ELeave) CMemSpyViewWindowGroups( iEngine, iObserver );
        }
        */
    CleanupStack::PushL( child );
    child->ConstructL( Rect(), *Parent() );
    CleanupStack::Pop( child );
    return child;
    }


void CMemSpyViewMainMenu::SetListBoxModelL()
    {
    CDesCArrayFlat* model = new(ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL( model );

    TBuf<KMaxFullName + 1> item;

    
    // 1st item = Processes
    _LIT(KItem1Format, "\tProcesses & Threads");
    item.Format( KItem1Format );
    model->AppendL( item );

    //some main view items temporary removed
    /*
    // 2nd item = RAM info
    _LIT(KItem2Format, "\tRAM Info");
    model->AppendL( KItem2Format );

    // 3rd item = ROM info
    _LIT(KItem3Format, "\tROM Info");
    model->AppendL( KItem3Format );

    // 4th item = FBServ Bitmaps
    _LIT(KItem4Format, "\tFont & Bitmap\nServer");
    model->AppendL( KItem4Format );
	*/

    /*
    // 6th item = Chunks
    _LIT(KItem6Format, "\tChunks");
    model->AppendL( KItem6Format );

    // 7th item = Code Segments
    _LIT(KItem7Format, "\tCode Segments");
    model->AppendL( KItem7Format );

    // 8th item = Open Files
    _LIT(KItem8Format, "\tOpen Files");
    model->AppendL( KItem8Format );

    // 9th item = System Config
    _LIT(KItem9Format, "\tSystem\nConfiguration");
    model->AppendL( KItem9Format );
	*/
    
    // 10th item = Kernel
    _LIT(KItem2Format, "\tKernel");
    model->AppendL( KItem2Format );    

    // 11th item = Heap Tracking
    _LIT(KItem3Format, "\tSystem-Wide Memory Tracking");
    model->AppendL( KItem3Format );

    // 5th item = Servers
    _LIT(KItem4Format, "\tServers");    
    model->AppendL( KItem4Format );
    /*
    // 12th item = Process-specific Tracking
    _LIT(KItem12Format, "\tProcess-Specific Memory Tracking");
    model->AppendL( KItem12Format );

    // 13th item = Drive Info
    _LIT(KItem13Format, "\tDrive Summary");
    model->AppendL( KItem13Format );

    // 14th item = ECOM
    _LIT(KItem14Format, "\tECom");
    model->AppendL( KItem14Format );

    // 15th item = Window Groups
    _LIT(KItem15Format, "\tWindow Groups");
    model->AppendL( KItem15Format );
    */
    // Set up list box
    CAknSettingStyleListBox* listbox = static_cast< CAknSettingStyleListBox* >( iListBox );
    listbox->Model()->SetItemTextArray( model );
    listbox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( model );
    }


TInt CMemSpyViewMainMenu::IndexByViewType( TMemSpyViewType aType )
    {
    TInt index = 0;
    //
    switch( aType )
        {
    default:
    	
    case EMemSpyViewTypeProcesses:
        index = 0;
        break;
/*  case EMemSpyViewTypeRAMInfo:
        index = 1;
        break;
    case EMemSpyViewTypeROMInfo:
        index = 2;
        break;
    case EMemSpyViewTypeFBServBitmaps:
        index = 3;
        break;
        */
       /*
    case EMemSpyViewTypeChunkList:
        index = 5;
        break;
    case EMemSpyViewTypeCodeSegmentList:
        index = 6;
        break;
    case EMemSpyViewTypeOpenFiles:
        index = 7;
        break;
    case EMemSpyViewTypeSystemConfig:
        index = 8;
        break;*/  
        
    case EMemSpyViewTypeKernel:
        index = 1 ;
        break;    
    case EMemSpyViewTypeHeapTracking:
        index = 2;
        break;
    case EMemSpyViewTypeServerList:
    	index = 3;
    	break;
        
        
    /*case EMemSpyViewTypeThreadInfoItemMemoryTrackingAutoStartConfig:
        index = 11;
        break;
    case EMemSpyViewTypeDriveSummary:
        index = 12;
        break;
    case EMemSpyViewTypeECom:
        index = 13;
        break;
    case EMemSpyViewTypeWindowGroups:
        index = 14;
        break;*/        
        }
    //
    return index;
    }

