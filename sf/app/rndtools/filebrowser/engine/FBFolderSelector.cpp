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

#include "FBFolderSelector.h"
#include "FBFileUtils.h"
#include "FB.hrh"
#include "FBStd.h"

#if (!defined __SERIES60_30__ && !defined __SERIES60_31__)
#include <driveinfo.h>
#endif // !defined __SERIES60_30__ && !defined __SERIES60_31__

// ===================================== MEMBER FUNCTIONS =====================================

CFileBrowserFolderSelector *CFileBrowserFolderSelector::NewL(
        TDes& aReturnPath,
        MFolderSelectorUI *aFolderSelectorUI)
    {
    CFileBrowserFolderSelector* self = new(ELeave) CFileBrowserFolderSelector(aReturnPath,
                                                                              aFolderSelectorUI);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CFileBrowserFolderSelector::~CFileBrowserFolderSelector()
    {
    delete iFileEntryList;
    delete iDriveEntryList;
    
    iFs.Close();

    //iEikonEnv->InfoMsgCancel();
    }

// --------------------------------------------------------------------------------------------

CFileBrowserFolderSelector::CFileBrowserFolderSelector(TDes& aReturnPath, MFolderSelectorUI *aFolderSelectorUI) :
    iReturnPath(aReturnPath),
    iFolderSelectorUI(aFolderSelectorUI)
    {
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFolderSelector::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    iDriveEntryList = new(ELeave) CDriveEntryList(8);
    iFileEntryList = new(ELeave) CFileEntryList(32);
    iCurrentPath = KNullDesC;
    
    //iIsDragging = EFalse;
    //EnableDragEvents();
    
    // get only writeable drives
    GetDriveListL();
    }

// --------------------------------------------------------------------------------------------

/*
void CFileBrowserFolderSelector::HandlePointerEventL(const TPointerEvent &aPointerEvent)
    {
    // detect direction of dragging by comparing the start and finish points
    
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
        iDragStartPoint = aPointerEvent.iPosition;
        iIsDragging = EFalse;
        }
    else if (aPointerEvent.iType == TPointerEvent::EDrag)
        {
        iIsDragging = ETrue;
        
        return;
        }
    else if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
        if (iIsDragging)
            {
            const TInt KDelta = iDragStartPoint.iX - aPointerEvent.iPosition.iX;
            const TInt KThreshold = 30;
            
            if (KDelta < -KThreshold) // dragging to right
                {
                // "emulate" right key press
                
                TKeyEvent keyEvent;
                keyEvent.iCode = EKeyRightArrow;
                keyEvent.iModifiers = 0;
                
                TEventCode type = EEventKey;
        
                OfferKeyEventL(keyEvent, type);
                
                return;
                }
            else if (KDelta > KThreshold) // dragging to left
                {
                // "emulate" left key press
                
                TKeyEvent keyEvent;
                keyEvent.iCode = EKeyLeftArrow;
                keyEvent.iModifiers = 0;
                
                TEventCode type = EEventKey;
        
                OfferKeyEventL(keyEvent, type);
                
                return;
                }            
            }
        iIsDragging = EFalse;
        }
    else
        {
        iIsDragging = EFalse;        
        }

    CAknListQueryDialog::HandlePointerEventL(aPointerEvent);    
    }
*/
       
// --------------------------------------------------------------------------------------------

TKeyResponse CFileBrowserFolderSelector::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
//    if(aType != EEventKey)
//        return EKeyWasNotConsumed;
    
//    if (aKeyEvent.iCode == EKeyLeftArrow && !IsDriveListViewActive())
//        {
//        MoveUpOneLevelL();
//        return EKeyWasConsumed;
//        }
//    else if (aKeyEvent.iCode == EKeyRightArrow && ((ListBox()->CurrentItemIndex() > 0 && !IsDriveListViewActive()) || IsDriveListViewActive()))
//        {
//        MoveDownToDirectoryL();
//        return EKeyWasConsumed;
//        }
//    else if (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
//        {
//        if (IsDriveListViewActive())
//            {
//            MoveDownToDirectoryL();
//            return EKeyWasConsumed;
//            }
//        else if (ListBox()->CurrentItemIndex() == 0)
//            {
//            TryExitL(EAknSoftkeyOk);
//            return EKeyWasConsumed;
//            }
//        else if (ListBox()->CurrentItemIndex() > 0)
//            {
//            MoveDownToDirectoryL();
//            return EKeyWasConsumed;
//            }
//        }
//
//    TKeyResponse result = CAknDialog::OfferKeyEventL(aKeyEvent, aType);
//
//    // update LSK label
//    if (!IsDriveListViewActive())
//        {
//        if (ListBox()->CurrentItemIndex() == 0)
//            {
//            ButtonGroupContainer().SetCommandL(0, EAknSoftkeyOk, iLSKActionText);
//            ButtonGroupContainer().DrawNow();
//            }
//        else
//            {
//            ButtonGroupContainer().SetCommandL(0, EAknSoftkeyOk, _L("Open dir"));
//            ButtonGroupContainer().DrawNow();
//            }
//        }
//
//    return result;
    return TKeyResponse();
    }    

// --------------------------------------------------------------------------------------------

void CFileBrowserFolderSelector::PreLayoutDynInitL()
    {
//    CAknListQueryDialog::PreLayoutDynInitL();

//    static_cast<CEikFormattedCellListBox*>(ListBox())->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFolderSelector::PostLayoutDynInitL()
    {
//    CAknListQueryDialog::PostLayoutDynInitL();
    
    //w SetIconArrayL(static_cast<CArrayPtr<CGulIcon>*>(iIconArray));
    
    RefreshViewL();
    }

// --------------------------------------------------------------------------------------------

TBool CFileBrowserFolderSelector::OkToExitL(TInt /*aButtonId*/)
    {
//    if (aButtonId == GetLeftCBAShortKeyPress())
//        {
//        if (IsDriveListViewActive())
//            {
//            MoveDownToDirectoryL();
//            return EFalse;
//            }
//        else if (ListBox()->CurrentItemIndex() == 0)
//            {
//            // close the dialog
//            iReturnPath = iCurrentPath;
//            return ETrue;
//            }
//        else if (ListBox()->CurrentItemIndex() > 0)
//            {
//            MoveDownToDirectoryL();
//            return EFalse;
//            }
//        else
//            return EFalse;
//        }
//    else if (aButtonId == GetRightCBAShortKeyPress())
//        {
//        return ETrue;
//        }
//    else
//        {
//        return EFalse;
//        }
    return ETrue;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFolderSelector::MoveUpOneLevelL()
    {
    if (iCurrentPath.Length() <= 3)
        {
        // move to drive list view is the current path is already short enough
        iCurrentPath = KNullDesC;
        }
    else
        {
        // move one directory up
        TInt marker(iCurrentPath.Length());
        
        // find second last dir marker
        for (TInt i=iCurrentPath.Length()-2; i>=0; i--)
            {
            if (iCurrentPath[i] == '\\')
                {
                marker = i;
                break;
                }
                
            }
        iCurrentPath = iCurrentPath.LeftTPtr(marker+1);
        }    

    // update view
    RefreshViewL();
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFolderSelector::MoveDownToDirectoryL(TInt index)
    {
    if (index >= 0)
        {
        if (IsDriveListViewActive())
            {
            // currently in a drive list view, move to root of selected drive
            if (iDriveEntryList->Count() > index)
                {
                TDriveEntry driveEntry = iDriveEntryList->At(index);
                
                iCurrentPath.Append(driveEntry.iLetter);    
                iCurrentPath.Append(_L(":\\"));
                }
            }
        else
            {
            // this needed because we have an extra item in the listbox
            index--;

            // append the new directory
            if (iFileEntryList->Count() > index)
                {
                TFileEntry fileEntry = iFileEntryList->At(index);
                
                if (fileEntry.iEntry.IsDir())
                    {
                    iCurrentPath.Append(fileEntry.iEntry.iName);
                    iCurrentPath.Append(_L("\\"));
                    }
                }
            }
                   
        // update view
        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFolderSelector::RefreshViewL()
    {
    if (IsDriveListViewActive())
        {
        GetDriveListL();
        }
    else
        {
        GetFileListL();
        }
    iFolderSelectorUI->InformFolderSelectionChanged();

    // TODO
//    if (iCurrentPath == KNullDesC)
//        iEikonEnv->InfoMsgWithDuration(_L("<- up dir  down dir ->"), TTimeIntervalMicroSeconds32(KMaxTInt));
//    else
//        iEikonEnv->InfoMsgWithDuration(iCurrentPath, TTimeIntervalMicroSeconds32(KMaxTInt));
   
//    SetItemTextArray(textArray);
//    SetOwnershipType(ELbmOwnsItemArray);
//    ListBox()->HandleItemAdditionL();
//    Layout();
//    ListBox()->SetCurrentItemIndex(0);
//    DrawDeferred();
    }

// --------------------------------------------------------------------------------------------
    
//TBool CFileBrowserFolderSelector::RunCopyDlgLD()
//    {
//    iLSKActionText.Copy(_L("Copy"));
//    return ExecuteLD(1/*R_COPY_TO_FOLDER_SELECTION_QUERY*/);
//    }

//// --------------------------------------------------------------------------------------------

//TBool CFileBrowserFolderSelector::RunMoveDlgLD()
//    {
//    iLSKActionText.Copy(_L("Move"));
//    return ExecuteLD(1/*R_MOVE_TO_FOLDER_SELECTION_QUERY*/);
//    }

//// --------------------------------------------------------------------------------------------

//TInt CAknQueryDialog::GetLeftCBAShortKeyPress()
//    {
//    return TInt16(0xffff & TInt16(ButtonGroupContainer().ButtonGroup()->CommandId(0)));
//    }

//// --------------------------------------------------------------------------------------------
  
//TInt CAknQueryDialog::GetRightCBAShortKeyPress()
//    {
//    return TInt16(0xffff & TInt16(ButtonGroupContainer().ButtonGroup()->CommandId(2)));
//    }
       
// --------------------------------------------------------------------------------------------

void CFileBrowserFolderSelector::GetDriveListL()
    {
    TDriveList driveList;

    // get drive listing depending of the support for network drives
    if (true/*TODO iEngine->Settings().iSupportNetworkDrives*/)
        {
        #ifndef __SERIES60_30__
            #ifndef __SERIES60_31__
                User::LeaveIfError(iFs.DriveList(driveList, KDriveAttAll));
            #endif
        #else
            User::LeaveIfError(iFs.DriveList(driveList));
        #endif
        }
    else
        {
        User::LeaveIfError(iFs.DriveList(driveList));
        }

    iDriveEntryList->Reset();

    //TDriveEntry driveEntry;
    for (TInt i=0; i<KMaxDrives; i++)
        {
        if (driveList[i])
            {
            TDriveEntry driveEntry;

            // set default icon
            driveEntry.iIconId = EFixedIconPhoneMemory;

            // get drive letter and number
            driveEntry.iLetter = 'A' + i;
            iFs.CharToDrive(driveEntry.iLetter, driveEntry.iNumber);

            // get volume info and check errors
            if (iFs.Volume(driveEntry.iVolumeInfo, driveEntry.iNumber) == KErrNone)
                {
                // set media type descriptor
                TInt mediaType = driveEntry.iVolumeInfo.iDrive.iType;
                TBool extMountable( EFalse );

                if (mediaType == EMediaNotPresent)
                    driveEntry.iMediaTypeDesc = _L("Not present");
                else if (mediaType ==EMediaUnknown )
                    driveEntry.iMediaTypeDesc = _L("Unknown");
                else if (mediaType ==EMediaFloppy )
                    driveEntry.iMediaTypeDesc = _L("Floppy");
                else if (mediaType == EMediaHardDisk)
                    driveEntry.iMediaTypeDesc = _L("Mass storage");
                else if (mediaType == EMediaCdRom)
                    driveEntry.iMediaTypeDesc = _L("CD-ROM");
                else if (mediaType == EMediaRam)
                    driveEntry.iMediaTypeDesc = _L("RAM");
                else if (mediaType == EMediaFlash)
                    driveEntry.iMediaTypeDesc = _L("Flash");
                else if (mediaType == EMediaRom)
                    driveEntry.iMediaTypeDesc = _L("ROM");
                else if (mediaType == EMediaRemote)
                    driveEntry.iMediaTypeDesc = _L("Remote");
                else if (mediaType == EMediaNANDFlash)
                    driveEntry.iMediaTypeDesc = _L("NAND flash");

                // get real size of the ROM drive
                if (mediaType == EMediaRom)
                    {
                    TMemoryInfoV1Buf ramMemory;
                    UserHal::MemoryInfo(ramMemory);
                    driveEntry.iVolumeInfo.iSize = ramMemory().iTotalRomInBytes;
                    }

                // set attribute descripitions
                if (driveEntry.iVolumeInfo.iDrive.iBattery == EBatNotSupported)
                    driveEntry.iAttributesDesc.Append(_L("Battery not supported"));
                else if (driveEntry.iVolumeInfo.iDrive.iBattery == EBatGood)
                    driveEntry.iAttributesDesc.Append(_L("Battery good"));
                else if (driveEntry.iVolumeInfo.iDrive.iBattery == EBatLow)
                    driveEntry.iAttributesDesc.Append(_L("Battery low"));
                else
                    driveEntry.iAttributesDesc.Append(_L("Battery state unknown"));

                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttLocal)
                    driveEntry.iAttributesDesc.Append(_L(" + Local"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRom)
                    driveEntry.iAttributesDesc.Append(_L(" + ROM"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRedirected)
                    driveEntry.iAttributesDesc.Append(_L("+ Redirected"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttSubsted)
                    driveEntry.iAttributesDesc.Append(_L(" + Substed"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttInternal)
                    driveEntry.iAttributesDesc.Append(_L(" + Internal"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRemovable)
                    {
                    driveEntry.iAttributesDesc.Append(_L(" + Removable"));
#if (!defined __SERIES60_30__ && !defined __SERIES60_31__)
                    TUint drvStatus( 0 );
                    if ( !DriveInfo::GetDriveStatus( iFs, driveEntry.iNumber, drvStatus ) &&
                         drvStatus & DriveInfo::EDriveExternallyMountable &&
                         drvStatus & DriveInfo::EDriveInternal )
                        {
                        extMountable = ETrue;
                        // iMediaTypeDesc already set as "Mass storage"
                        }
                    else
                        {
                        driveEntry.iMediaTypeDesc = _L("Memory card");
                        }
#else
                    driveEntry.iMediaTypeDesc = _L("Memory card");
#endif
                    }
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRemote)
                    driveEntry.iAttributesDesc.Append(_L(" + Remote"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttTransaction)
                    driveEntry.iAttributesDesc.Append(_L(" + Transaction"));

                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttVariableSize)
                    driveEntry.iAttributesDesc.Append(_L(" + Variable size"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttDualDensity)
                    driveEntry.iAttributesDesc.Append(_L(" + Dual density"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttFormattable)
                    driveEntry.iAttributesDesc.Append(_L(" + Formattable"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttWriteProtected)
                    driveEntry.iAttributesDesc.Append(_L(" + Write protected"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttLockable)
                    driveEntry.iAttributesDesc.Append(_L(" + Lockable"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttLocked)
                    driveEntry.iAttributesDesc.Append(_L(" + Locked"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttHasPassword)
                    driveEntry.iAttributesDesc.Append(_L(" + Has password"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttReadWhileWrite)
                    driveEntry.iAttributesDesc.Append(_L(" + Read while write"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttDeleteNotify)
                    driveEntry.iAttributesDesc.Append(_L(" + Supports DeleteNotify"));


                // mark a removable media with memory card icon
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRemovable && !extMountable)
                    {
                    if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttLocked || driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAbsent)
                        {
                        driveEntry.iIconId = EFixedIconMemoryCardDisabled;
                        }
                    else
                        {
                        driveEntry.iIconId = EFixedIconMemoryCard;
                        }
                    }
                }

            // if this fails, likely it's a memory card which is not present
            else
                {
                TVolumeInfo volumeInfo;
                volumeInfo.iSize = 0;
                volumeInfo.iFree = 0;
                volumeInfo.iDrive.iDriveAtt = KDriveAttRemovable;
                volumeInfo.iDrive.iMediaAtt = KMediaAttWriteProtected;
                driveEntry.iVolumeInfo = volumeInfo;

                driveEntry.iMediaTypeDesc = _L("Not ready");
                driveEntry.iIconId = EFixedIconMemoryCardDisabled;
                }
// TODO What to do with iPrevFolderName and iPrevFolderIndex
//            if ( iPrevFolderName != KNullDesC && iPrevFolderName[0] == driveEntry.iLetter )
//                {
//                iPrevFolderIndex = iDriveEntryList->Count();
//                }

            if (driveEntry.iVolumeInfo.iDrive.iMediaAtt != KMediaAttWriteProtected
                && driveEntry.iVolumeInfo.iDrive.iMediaAtt != KMediaAttLocked
                && driveEntry.iVolumeInfo.iDrive.iDriveAtt != KDriveAbsent)
                {
                iDriveEntryList->AppendL(driveEntry);
                }
            }
        }
    }

void CFileBrowserFolderSelector::GetFileListL()
    {
    iFileEntryList->Reset();

    CDir* dir = NULL;
    if (iFs.GetDir(iCurrentPath, KEntryAttDir|KEntryAttMatchMask, ESortByName | EAscending | EDirsFirst, dir) == KErrNone)
        {
        CleanupStack::PushL(dir);

        for (TInt i=0; i<dir->Count(); i++)
            {
            TFileEntry fileEntry;
            fileEntry.iPath = iCurrentPath;
            fileEntry.iEntry = (*dir)[i];
            fileEntry.iDirEntries = KErrNotFound;
            fileEntry.iIconId = EFixedIconEmpty;

            // check for directory entries
            if (fileEntry.iEntry.IsDir())
                {
                fileEntry.iIconId = EFixedIconFolder;

                TFileName subPath = fileEntry.iPath;
                subPath.Append(fileEntry.iEntry.iName);
                subPath.Append(_L("\\"));

                // check if any sub directories
                CDir* subDir = NULL;
                if (iFs.GetDir(subPath, KEntryAttDir|KEntryAttMatchMask, ESortNone | EDirsFirst, subDir) == KErrNone)
                    {
                    fileEntry.iDirEntries = subDir->Count();

                    for (TInt j=0; j<subDir->Count(); j++)
                        {
                        TEntry entry = (*subDir)[j];

                        if (entry.IsDir())
                            {
                            fileEntry.iIconId = EFixedIconFolderSub;
                            break;
                            }
                        }

                    delete subDir;
                    }
                    iFileEntryList->AppendL(fileEntry);
                }
            }

        CleanupStack::PopAndDestroy(); //dir
        }
    }

// --------------------------------------------------------------------------------------------
      
// End of File

