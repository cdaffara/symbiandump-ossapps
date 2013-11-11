/*
 * Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   Wlan Adapter DB handler
 *
 */

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include <WlanCdbCols.h>
#include "NSmlWlanSettings.h"
#include "NSmlWLanAdapter.h"
#include "nsmldebug.h"
#include <comms-infras/commdb/protection/protectdb.h>
#include <SettingEnforcementInfo.h> // vsettingenforcementinfo
#include <featmgr.h>
#include <EapExpandedType.h>
#include <EapGeneralSettings.h>

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

const TUint KEAPListBufferSize = 128;
const TUint KLengthOfOldStyleEAPListEntry = 5; // "+xxx,"

const TInt KBeginTransRetryDelay = 1000000; // Delay for comms db begintransaction retry (microseconds)
const TInt KBeginTransRetryCount = 7; // Maximum number of retries
//-----------------------------------------------------------------------------
// CWlanAdapter::CWlanSettings( CCommsDatabase& aDatabase )
//-----------------------------------------------------------------------------
CWlanSettings::CWlanSettings(CCommsDatabase& aDatabase) :
    iDatabase(aDatabase), iExpandedEAPTypeFieldsUsed(ETrue)
    {
    _DBG_FILE("CWlanSettings::CWlanSettings(): begin");

    iTableView = 0;
    iEapType = NULL;
    iSecondaryView = NULL;

    _DBG_FILE("CWlanSettings::CWlanSettings(): end");
    }

//-----------------------------------------------------------------------------
// CWlanAdapter::~CWlanSettings( )
//-----------------------------------------------------------------------------
CWlanSettings::~CWlanSettings()
    {

    delete iEapType;
    delete iSecondaryView;
    delete iTableView;
    }

//-----------------------------------------------------------------------------
// CWlanSettings* CWlanSettings::NewL( CCommsDatabase& aDatabase )
//-----------------------------------------------------------------------------
CWlanSettings* CWlanSettings::NewL(CCommsDatabase& aDatabase)
    {
    _DBG_FILE("CWlanSettings::NewL(): begin");

    CWlanSettings* self = new (ELeave) CWlanSettings(aDatabase);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    _DBG_FILE("CWlanSettings::NewL(): end");
    return self;
    }

//-----------------------------------------------------------------------------
// void CWlanSettings::ConstructL( )
//-----------------------------------------------------------------------------
void CWlanSettings::ConstructL()
    {
    _DBG_FILE("CWlanSettings::ConstructL(): begin");
    _DBG_FILE("CWlanSettings::ConstructL(): end");
    }

//-----------------------------------------------------------------------------
// TInt CWlanSettings::GetWlanSettings( TUint32 aLuid, TWlanSettings& aWlanSettings )
//-----------------------------------------------------------------------------
TInt CWlanSettings::GetWlanSettings(TUint32 aLuid,
        TWlanSettings& aWlanSettings)
    {
    _DBG_FILE("CWlanSettings::GetWlanSettings(): begin");

    TRAPD(err, ConnectToDatabaseL());
    if (err == KErrNone)
        {
        err = GoToRecord(aLuid);

        if (err == KErrNone)
            {
            TRAP(err, GetDataFromRecordL(&aWlanSettings));
            }
        }

    _DBG_FILE("CWlanSettings::GetWlanSettings(): end");
    return err;
    }

//-----------------------------------------------------------------------------
// TInt CWlanSettings::GetEapSettings( TInt aId, TWlanSettings& aWlanSettings )
//-----------------------------------------------------------------------------
TInt CWlanSettings::GetEAPSettings(const TInt aId,
        TEapExpandedType& aExpandedId, TEapExpandedType& aEncapsId,
        EAPSettings& aEapSettings)
    {
    _DBG_FILE("CWlanSettings::GetEapSettings(): begin");

    TRAPD(err, GetEAPInterfaceL(aId, aEncapsId, aExpandedId));
    DBG_ARGS(_S16("GetEAPInterfaceL leaved with %d"), err);
    if (err == KErrNone)
        {
        TRAP(err, iEapType->GetConfigurationL(aEapSettings));
        DBG_ARGS(_S16("iEapType->GetConfigurationL leaved with %d"), err);
        }

    _DBG_FILE("CWlanSettings::GetEAPSettings(): end");
    return err;
    }

//-----------------------------------------------------------------------------
// TInt CWlanSettings::DeleteWlanSettings( TUint32 aLuid )
//-----------------------------------------------------------------------------
TInt CWlanSettings::DeleteWlanSettings(TUint32 aLuid)
    {
    _DBG_FILE("CWlanSettings::DeleteWlanSettings(): begin");
    TBool wlanEnforce = EFalse;
    TInt tableLockError = KErrNone;

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        TInt enError = KErrNone;
        TRAP(enError, wlanEnforce = CheckEnforcementL());
        DBG_ARGS(
                _S16(
                        "CWlanSettings::DeleteWLANSettings, check enforcement failed. error: <%D>"),
                enError);
        if (wlanEnforce)
            {
            _DBG_FILE(
                    "CWlanSettings::WriteWlanSettings(): wlan enforce is ON ");
            TRAPD(lockError, tableLockError = PerformLockWLANTablesL(EFalse));
            if (lockError == KErrNone && tableLockError == KErrNone)
                {
                _DBG_FILE(
                        "CWlanSettings::WriteWlanSettings(): Table unlcoked successfully ");
                iWLANRelock = ETrue;
                }
            }
        }

    TRAPD(err, ConnectToDatabaseL());
    if (err == KErrNone)
        {
        err = GoToRecord(aLuid);
        if (err == KErrNone)
            {

            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                if (wlanEnforce)
                    {
                    ((CCommsDbProtectTableView*) iTableView)->UnprotectRecord();
                    }
                }

            TInt err = iTableView->UpdateRecord();
            if (err == KErrLocked)
                {
                _DBG_FILE("DeleteWlanSettings: UpdateRecord was locked.");
                TInt retry = KBeginTransRetryCount;
                while (retry > 0 && err == KErrLocked)
                    {
                    User::After(KBeginTransRetryDelay);
                    _DBG_FILE("DeleteWlanSettings: Slept 1 second. Try again");
                    err = iTableView->UpdateRecord();
                    retry--;
                    }
                if (err != KErrNone)
                    {
                    _DBG_FILE(
                            "DeleteWlanSettings: UpdateRecord was unsuccessful");
                    }
                else
                    {
                    _DBG_FILE(
                            "DeleteWlanSettings: UpdateRecord was successful");
                    }
                }

            if (err == KErrNone)
                {
                TRAP(err, iTableView->WriteUintL(TPtrC(WLAN_SERVICE_ID), 0));
                if (err != KErrNone)
                    {
                    if (FeatureManager::FeatureSupported(
                            KFeatureIdSapPolicyManagement))
                        {
                        TInt tableLockError = KErrNone;
                        if (iWLANRelock)
                            {
                            TInt lockError = KErrNone;
                            TRAP(lockError, tableLockError
                                    = PerformLockWLANTablesL(ETrue));
                            ((CCommsDbProtectTableView*) iTableView)->ProtectRecord();
                            DBG_ARGS(
                                    _S16(
                                            "CWlanSettings::DeleteWLANSettings, WLAN table Lock error. error: <%D>, <%D> "),
                                    lockError, tableLockError);
                            iWLANRelock = EFalse;
                            }
                        if (tableLockError != KErrNone) // to remove warnings
                            {
                            tableLockError = KErrNone;
                            }
                        }
                    return err;
                    }
                }
            err = iTableView->PutRecordChanges();
            if (err == KErrLocked)
                {
                _DBG_FILE("DeleteWlanSettings: PutRecordChanges was locked.");
                TInt retry = KBeginTransRetryCount;
                while (retry > 0 && err == KErrLocked)
                    {
                    User::After(KBeginTransRetryDelay);
                    _DBG_FILE("DeleteWlanSettings: Slept 1 second. Try again");
                    err = iTableView->PutRecordChanges();
                    retry--;
                    }
                if (err != KErrNone)
                    {
                    _DBG_FILE(
                            "DeleteWlanSettings: PutRecordChanges was unsuccessful");
                    }
                else
                    {
                    _DBG_FILE(
                            "DeleteWlanSettings: PutRecordChanges was successful");
                    }
                }

            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                TInt tableLockError = KErrNone;
                if (iWLANRelock)
                    {
                    TInt lockError = KErrNone;
                    TRAP(lockError, tableLockError = PerformLockWLANTablesL(
                            ETrue));
                    DBG_ARGS(
                            _S16(
                                    "CWlanSettings::DeleteWLANSettings, WLAN table Lock error. error: <%D>, <%D> "),
                            lockError, tableLockError);
                    ((CCommsDbProtectTableView*) iTableView)->ProtectRecord();
                    iWLANRelock = EFalse;
                    }
                if (tableLockError != KErrNone) // to remove warnings
                    {
                    tableLockError = KErrNone;
                    }
                }

            if (err == KErrNone)
                {
                // Ignore err on purpose
                TRAP(err, DeleteSecondarySSIDsL(aLuid));

                TRAP(err, DeleteWlanEapSettingsL(aLuid));
                }
            }
        }

    _DBG_FILE("CWlanSettings::DeleteWlanSettings(): end");
    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        if (iWLANRelock)
            {
            TInt lockError = KErrNone;
            TRAP(lockError, tableLockError = PerformLockWLANTablesL(ETrue));
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::DeleteWLANSettings, WLAN table Lock error. error: <%D>, <%D> "),
                    lockError, tableLockError);
            ((CCommsDbProtectTableView*) iTableView)->ProtectRecord();
            iWLANRelock = EFalse;
            }
        }

    if (wlanEnforce)
        wlanEnforce = EFalse; // to get rid of warnings
    return err;
    }

TInt CWlanSettings::DeleteOneSecondarySSIDL(TUint32 aWLANId, TUint32 asecId)
    {

    TInt retval = KErrNone;
    SetSecondaryViewToRecordL(aWLANId);
    retval = iSecondaryView->GotoFirstRecord();

    while (retval == KErrNone)
        {
        TSecondarySSID ssid;
        TRAP(retval, iSecondaryView->ReadUintL(TPtrC(WLAN_SEC_SSID_ID),
                ssid.Id));
        if (ssid.Id == asecId)
            {
            //retval = iSecondaryView->UpdateRecord();
            //if( retval==KErrNone )
            //  {
            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                TBool wlanEnforce = EFalse;
                TInt enError = KErrNone;
                TRAP(enError, wlanEnforce = CheckEnforcementL());
                DBG_ARGS(
                        _S16(
                                "CWlanSettings::DeleteWLANSettings, check enforcement failed. error: <%D>"),
                        enError);
                if (wlanEnforce)
                    {
                    ((CCommsDbProtectTableView*) iSecondaryView)->UnprotectRecord();
                    }
                }

            retval = iSecondaryView->DeleteRecord();
            if (retval == KErrLocked)
                {
                _DBG_FILE("DeleteOneSecondarySSIDL: DeleteRecord was locked.");
                TInt retry = KBeginTransRetryCount;
                while (retry > 0 && retval == KErrLocked)
                    {
                    User::After(KBeginTransRetryDelay);
                    _DBG_FILE(
                            "DeleteOneSecondarySSIDL: Slept 1 second. Try again");
                    retval = iSecondaryView->DeleteRecord();
                    retry--;
                    }
                if (retval != KErrNone)
                    {
                    _DBG_FILE(
                            "DeleteOneSecondarySSIDL: DeleteRecord was unsuccessful");
                    }
                else
                    {
                    _DBG_FILE(
                            "DeleteOneSecondarySSIDL: DeleteRecord was successful");
                    }
                }
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::DeleteOneSecondarySSIDL, Secondaryssid delete error: <%D> "),
                    retval);

            //    iTableView->PutRecordChanges( );
            //}

            break;
            }

        retval = iSecondaryView->GotoNextRecord();
        }

    return retval;
    }

TInt CWlanSettings::DeleteSecondarySSIDsL(TUint32 aWLANId)
    {
    TInt retVal(KErrNone);
    if (iSecondaryView == NULL)
        {
        SetSecondaryViewToRecordL(aWLANId);
        }
    else
        {
        retVal = iSecondaryView->GotoFirstRecord();
        TUint32 secId = 0;
        if (retVal == KErrNone)
            {
            iSecondaryView->ReadUintL(TPtrC(WLAN_SEC_SSID_SERVICE_ID), secId);
            }
        if (secId != aWLANId)
            {
            SetSecondaryViewToRecordL(aWLANId);
            }
        }

    TInt err = iSecondaryView->GotoFirstRecord();
    while (err == KErrNone)
        {
        if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
            {
            TBool wlanEnforce = EFalse;
            TInt enError = KErrNone;
            TRAP(enError, wlanEnforce = CheckEnforcementL());
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::DeleteSecondarySSIDsL, check enforcement failed. error: <%D>"),
                    enError);
            if (wlanEnforce)
                {
                ((CCommsDbProtectTableView*) iSecondaryView)->UnprotectRecord();
                }
            }
        retVal = iSecondaryView->DeleteRecord();
        if (retVal == KErrLocked)
            {
            _DBG_FILE("DeleteSecondarySSIDsL: DeleteRecord was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && retVal == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("DeleteSecondarySSIDsL: Slept 1 second. Try again");
                retVal = iSecondaryView->DeleteRecord();
                retry--;
                }
            if (retVal != KErrNone)
                {
                _DBG_FILE(
                        "DeleteSecondarySSIDsL: DeleteRecord was unsuccessful");
                }
            else
                {
                _DBG_FILE(
                        "DeleteSecondarySSIDsL: DeleteRecord was successful");
                }
            }
        DBG_ARGS(
                _S16(
                        "CWlanSettings::DeleteSecondarySSIDsL, Secondaryssid delete error: <%D> "),
                retVal);

        err = iSecondaryView->GotoNextRecord();
        }

    return retVal;
    }
//-----------------------------------------------------------------------------
// TInt CWlanSettings::DeleteEapSettings( TInt aWlanId )
//-----------------------------------------------------------------------------
TInt CWlanSettings::DeleteEAPSettings(TInt aWlanId)
    {
    _DBG_FILE("CWlanSettings::DeleteEAPSettings(): begin");

    TInt err(KErrNone);

    TRAP(err, iEapType->DeleteConfigurationL());
    if (err != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::DeleteEAPSettings, delete failed, error: <%D> "),
                err);
        return err;
        }

    _DBG_FILE("CWlanSettings::DeleteEAPSettings(): end");
    return err;
    }

//-----------------------------------------------------------------------------
// TInt CWlanSettings::DeleteWlanEapSettings( TInt aWlanId )
// Deletes all EAPs from a Wlan
//-----------------------------------------------------------------------------
void CWlanSettings::DeleteWlanEapSettingsL(TInt aWlanId)
    {
    _DBG_FILE("CWlanSettings::DeleteWlanEapSettings(): begin");
    TInt err(KErrNone);
    err = DeleteEAPSettings(aWlanId);
    User::LeaveIfError(err);

    _DBG_FILE("CWlanSettings::DeleteWlanEapSettings(): end");
    }

//-----------------------------------------------------------------------------
// TInt CWlanSettings::WriteWlanSettings( TWlanSettings& aWlanSettings )
//-----------------------------------------------------------------------------
TInt CWlanSettings::WriteWlanSettings(TWlanSettings& aWlanSettings)
    {
    _DBG_FILE("CWlanSettings::WriteWlanSettings(): begin");

    TInt err = 0;
    TInt enError = KErrNone;
    iServiceID = aWlanSettings.ServiceID;
    TBool wlanEnforce = EFalse;

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        TRAP(enError, wlanEnforce = CheckEnforcementL());
        DBG_ARGS(
                _S16(
                        "CWlanSettings::WriteWlanSettings, check enforcement . error: <%D>"),
                enError);
        if (wlanEnforce)
            {
            TRAPD(lockError, enError = PerformLockWLANTablesL(EFalse));
            if (lockError == KErrNone && enError == KErrNone)
                {
                iWLANRelock = ETrue;
                }
            }
        }

    if (RecordExists(aWlanSettings.ServiceID) <= 0)
        {
        err = iTableView->InsertRecord(aWlanSettings.Id);
        if (err == KErrLocked)
            {
            _DBG_FILE("WriteWlanSettings InsertRecord: CommsDat was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && err == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("WriteWlanSettings: Slept 1 second. Try again");
                err = iTableView->InsertRecord(aWlanSettings.Id);
                retry--;
                }
            }

        if (err != KErrNone)
            {
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::WriteWlanSettings(): - InsertRecord failed. error: <%D>"),
                    err);

            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                TInt tableLockError = KErrNone;
                if (iWLANRelock)
                    {
                    TInt lockError = KErrNone;
                    TRAP(lockError, tableLockError = PerformLockWLANTablesL(
                            ETrue));
                    DBG_ARGS(
                            _S16(
                                    "CWlanSettings::WriteWlanSettings, WLAN table Lock error. error: <%D>, <%D> "),
                            lockError, tableLockError);
                    iWLANRelock = EFalse;
                    }
                if (tableLockError != KErrNone) // to remove warnings
                    {
                    tableLockError = KErrNone;
                    }
                }
            return err;
            }
        TRAP(err, InitialiseRecordL());
        if (err != KErrNone)
            {
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::WriteWlanSettings(): - InitialiseRecord failed. error: <%D>"),
                    err);

            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                TInt tableLockError = KErrNone;
                if (iWLANRelock)
                    {
                    TInt lockError = KErrNone;
                    TRAP(lockError, tableLockError = PerformLockWLANTablesL(
                            ETrue));
                    DBG_ARGS(
                            _S16(
                                    "CWlanSettings::WriteWlanSettings, WLAN table Lock error. error: <%D>, <%D> "),
                            lockError, tableLockError);
                    iWLANRelock = EFalse;
                    }
                if (tableLockError != KErrNone) // to remove warnings
                    {
                    tableLockError = KErrNone;
                    }
                }
            return err;
            }
        }
    else
        {
        err = GoToRecord(aWlanSettings.ServiceID);
        if (err == KErrNone)
            {

            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                if (wlanEnforce)
                    {
                    _DBG_FILE(
                            "CWlanSettings::WriteWlanSettings(): unprotect wlan");
                    ((CCommsDbProtectTableView*) iTableView)->UnprotectRecord();
                    }
                }

            err = iTableView->UpdateRecord();
            if (err == KErrLocked)
                {
                _DBG_FILE("WriteWlanSettings: UpdateRecord was locked.");
                TInt retry = KBeginTransRetryCount;
                while (retry > 0 && err == KErrLocked)
                    {
                    User::After(KBeginTransRetryDelay);
                    _DBG_FILE("WriteWlanSettings: Slept 1 second. Try again");
                    err = iTableView->UpdateRecord();
                    retry--;
                    }
                if (err != KErrNone)
                    {
                    _DBG_FILE(
                            "WriteWlanSettings: UpdateRecord was unsuccessful");
                    }
                else
                    {
                    _DBG_FILE(
                            "WriteWlanSettings: UpdateRecord was successful");
                    }
                }

            if (err != KErrNone)
                {
                DBG_ARGS(
                        _S16(
                                "CWlanSettings::WriteWlanSettings(): - UpdateRecord failed. error: <%D>"),
                        err);

                if (FeatureManager::FeatureSupported(
                        KFeatureIdSapPolicyManagement))
                    {
                    TInt tableLockError = KErrNone;
                    if (iWLANRelock)
                        {
                        TInt lockError = KErrNone;
                        TRAP(lockError, tableLockError
                                = PerformLockWLANTablesL(ETrue));
                        DBG_ARGS(
                                _S16(
                                        "CWlanSettings::WriteWlanSettings, WLAN table Lock error. error: <%D>, <%D> "),
                                lockError, tableLockError);
                        ((CCommsDbProtectTableView*) iTableView)->ProtectRecord();
                        iWLANRelock = EFalse;
                        }
                    if (tableLockError != KErrNone) // to remove warnings
                        {
                        tableLockError = KErrNone;
                        }
                    }
                return err;
                }
            }
        }

    TRAP(err, WriteDataToRecordL(&aWlanSettings));

    if (err == KErrNone)
        {
        _DBG_FILE("CWlanSettings::WriteWlanSettings(): PutRecordChanges");
        err = iTableView->PutRecordChanges();
        if (err == KErrLocked)
            {
            _DBG_FILE("WriteWlanSettings: PutRecordChanges was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && err == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("WriteWlanSettings: Slept 1 second. Try again");
                err = iTableView->PutRecordChanges();
                retry--;
                }
            }
        if (err != KErrNone)
            {
            _DBG_FILE("WriteWlanSettings: PutRecordChanges was unsuccessful");
            }
        else
            {
            _DBG_FILE("WriteWlanSettings: PutRecordChanges was successful");
            }
        }
    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        if (iWLANRelock)
            {
            TInt lockError = KErrNone;
            TRAP(lockError, enError = PerformLockWLANTablesL(ETrue));
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::WriteWlanSettings, WLAN table Lock error. error: <%D>, <%D> "),
                    lockError, enError);
            iWLANRelock = EFalse;
            }
        }
    DBG_ARGS(_S16("CWlanSettings::WriteWlanSettings(): end. err: <%D>"), err);
    return err;
    }

//-----------------------------------------------------------------------------
// TInt CWlanSettings::WriteWlanSettings( TWlanSettings& aWlanSettings )
//-----------------------------------------------------------------------------
TInt CWlanSettings::WriteSecondarySSIDL(TUint32 aWlanID,
        TSecondarySSID& aSettings, TBool aNew)
    {

    _DBG_FILE("CWlanSettings::WriteSecondarySSIDL(): begin");

    TUint32 id = 0;
    delete iSecondaryView;
    iSecondaryView = NULL;

    TInt retval = KErrNone;
    TInt reLock = EFalse;
    TBool apEnforce = EFalse;

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        TRAPD(eError,apEnforce=CheckEnforcementL())
        DBG_ARGS8(_S8("CNSmlInternetAdapter::WriteSecondarySSIDL(): EError %d , APEnforcement is %d"),eError,apEnforce);
        if (eError == KErrNone && apEnforce)
            {
            _DBG_FILE("CWlanSettings::WriteSecondarySSIDL(): set db protect");
            TInt enError = KErrNone;
            TInt lockError = KErrNone;
            TRAP(lockError, enError = PerformLockWLANTablesL(EFalse));
            reLock = ETrue;
            if (enError != KErrNone) // to remove warnings
                {
                enError = KErrNone;
                }
            }
        }

    if (aNew)
        {
        _DBG_FILE("CWlanSettings::WriteSecondarySSIDL(): new table inserted");
        iSecondaryView = iDatabase.OpenTableLC(TPtrC(WLAN_SECONDARY_SSID));
        CleanupStack::Pop(iSecondaryView); // iSecondaryView
        retval = iSecondaryView->InsertRecord(id);

        if (retval == KErrLocked)
            {
            _DBG_FILE("WriteSecondarySSIDL: InsertRecord was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && retval == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("WriteSecondarySSIDL: Slept 1 second. Try again");
                retval = iSecondaryView->InsertRecord(id);
                retry--;
                }
            }
        if (retval != KErrNone)
            {
            _DBG_FILE("WriteSecondarySSIDL: InsertRecord was unsuccessful");
            }
        else
            {
            _DBG_FILE("WriteSecondarySSIDL: InsertRecord was successful");
            }

        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::WriteSecondarySSIDL(): Add record, error %d"),
                retval);
        if (retval == KErrNone)
            {
            _DBG_FILE(
                    "CWlanSettings::WriteSecondarySSIDL(): table insertion went ok");
            iSecondaryView->WriteUintL(TPtrC(WLAN_SEC_SSID_SERVICE_ID),
                    aWlanID);
            iSecondaryView->WriteUintL(TPtrC(WLAN_SEC_SSID_ID), id);
            }
        }
    else
        {
        _DBG_FILE(
                "CWlanSettings::WriteSecondarySSIDL(): table exists, find it");
        iSecondaryView = iDatabase.OpenViewMatchingUintLC(TPtrC(
                WLAN_SECONDARY_SSID), TPtrC(WLAN_SEC_SSID_ID), aSettings.Id);
        CleanupStack::Pop(iSecondaryView); // iSecondaryView
        TInt err = iSecondaryView->GotoFirstRecord();
        if (err != KErrNone)
            {
            _DBG_FILE(
                    "CWlanSettings::WriteSecondarySSIDL(): go to first record failed");
            // most likely KErrNotFound, something wrong with the URI parsing probably
            return err;
            }
        if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
            {
            if (apEnforce)
                {
                ((CCommsDbProtectTableView*) iSecondaryView)->UnprotectRecord();
                }
            }
        retval = iSecondaryView->UpdateRecord();
        if (retval == KErrLocked)
            {
            _DBG_FILE("WriteSecondarySSIDL: UpdateRecord was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && retval == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("WriteSecondarySSIDL: Slept 1 second. Try again");
                retval = iSecondaryView->UpdateRecord();
                retry--;
                }
            }
        if (retval != KErrNone)
            {
            _DBG_FILE("WriteSecondarySSIDL: UpdateRecord was unsuccessful");
            }
        else
            {
            _DBG_FILE("WriteSecondarySSIDL: UpdateRecord was successful");
            }
        }
    if (retval == KErrNone)
        {
        DBG_ARGS8(
                _S8(
                        "CWlanSettings::writesecondaryssids - WLAN_SEC_SSID_SCANNED_SSID uri: <%S> ad  WLAN_SEC_SSID_USED_SSID : <%S>"),
                &aSettings.ScannedId, &aSettings.UsedId);
        iSecondaryView->WriteTextL(TPtrC(WLAN_SEC_SSID_SCANNED_SSID),
                aSettings.ScannedId);
        iSecondaryView->WriteTextL(TPtrC(WLAN_SEC_SSID_USED_SSID),
                aSettings.UsedId);

        retval = iSecondaryView->PutRecordChanges();
        if (retval == KErrLocked)
            {
            _DBG_FILE("WriteSecondarySSIDL: PutRecordChanges was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && retval == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("WriteSecondarySSIDL: Slept 1 second. Try again");
                retval = iSecondaryView->PutRecordChanges();
                retry--;
                }
            }
        if (retval != KErrNone)
            {
            _DBG_FILE(
                    "WriteSecondarySSIDL: PutRecordChanges was unsuccessful");
            }
        else
            {
            _DBG_FILE("WriteSecondarySSIDL: PutRecordChanges was successful");
            }
        }

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        if (reLock)
            {
            TInt enError = KErrNone;
            TInt lockError = KErrNone;
            TRAP(lockError, enError = PerformLockWLANTablesL(ETrue));
            if (iSecondaryView != NULL)
                {
                ((CCommsDbProtectTableView*) iSecondaryView)->ProtectRecord();
                }
            if (enError != KErrNone) // to remove warnings
                {
                enError = KErrNone;
                }
            }
        }

    // This makes sure that there will be no calling to GotoFirstRecord() 
    // before secondaryview is reinitialized, (causes a panic if that would happen)
    delete iSecondaryView;
    iSecondaryView = NULL;

    if (reLock && apEnforce)
        {
        reLock = EFalse; // to get rid of warnings
        apEnforce = EFalse;
        }

    return retval;

    }

TInt CWlanSettings::WriteEAPSettings(EAPSettings& aEapSettings)
    {
    TRAPD(err, iEapType->SetConfigurationL(aEapSettings));
    return err;
    }

//-----------------------------------------------------------------------------
// void CWlanSettings::ConnectToDatabaseL( )
//-----------------------------------------------------------------------------
void CWlanSettings::ConnectToDatabaseL()
    {
    _DBG_FILE("CWlanSettings::ConnectToDatabaseL(): begin");

    if (!iTableView)
        {
        iTableView = iDatabase.OpenTableLC(TPtrC(WLAN_SERVICE));

        if (iTableView == NULL)
            {
            _DBG_FILE(
                    "CWlanSettings::ConnectToDatabaseL(): could not open table");
            User::Leave(KErrCouldNotConnect);
            }

        CleanupStack::Pop(iTableView); //iTableView
        }

    _DBG_FILE("CWlanSettings::ConnectToDatabaseL(): end");
    }

void CWlanSettings::GetEAPInterfaceL(const TInt aId,
        TEapExpandedType& aEncapsId, TEapExpandedType& aExpandedId)
    {
    if (aId < 0)
        {
        // Illegal input
        User::Leave(KErrArgument);
        }

    if (iEapType)
        {
        delete iEapType;
        iEapType = NULL;
        }
    TInt err = KErrNone;
    TRAP(err, iEapType = CEapType::NewL(ELan, aId, aExpandedId));

    if (err == KErrNone)
        {
        if (aEncapsId != (*EapExpandedTypeNone.GetType()))
            {
            iEapType->SetTunnelingType(aEncapsId);
            }
        }
    }

//-----------------------------------------------------------------------------
// TInt CWlanSettings::GoToRecord( TUint32 aId )
// Moves iTableView to point record whose id-number equals to aId
//-----------------------------------------------------------------------------
TInt CWlanSettings::GoToRecord(TUint32 aId)
    {
    _DBG_FILE("CWlanSettings::GoToRecord(): begin");
    DBG_ARGS(
            _S16("CWlanSettings::GoToRecord(): begin Search record id: <%D>"),
            aId);

    TInt err = iTableView->GotoFirstRecord();
    if (err != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GoToRecord(): end, GotoFirstRecord. err: <%D>"),
                err);
        return err;
        }

    TUint32 id = 0;
    TRAP(err, iTableView->ReadUintL(TPtrC(WLAN_SERVICE_ID), id));
    if (err != KErrNone)
        {
        DBG_ARGS(_S16(
                "CWlanSettings::GoToRecord(): end, ReadUintL. err: <%D>"),
                err);
        return err;
        }

    while (aId != id)
        {
        err = iTableView->GotoNextRecord();
        if (err != KErrNone)
            {
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::GoToRecord(): end, GotoNextRecord. err: <%D>"),
                    err);
            return err;
            }

        TRAP(err, iTableView->ReadUintL(TPtrC(WLAN_SERVICE_ID), id));

        if (err == KErrNone)
            {
            DBG_ARGS(_S16(
                    "CWlanSettings::GoToRecord(): Found service id: <%D>"),
                    id);
            }
        else
            {
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::GoToRecord(): end, ReadUintL next. err: <%D>"),
                    err);
            return err;
            }
        }

    TRAP(err, SetSecondaryViewToRecordL(aId));

    DBG_ARGS(_S16("CWlanSettings::GoToRecord(): end. Final record id: <%D>"),
            id);

    return err;
    }

void CWlanSettings::SetSecondaryViewToRecordL(TUint32 aId)
    {
    // optional secondarySSID
    if (iSecondaryView)
        {
        delete iSecondaryView;
        iSecondaryView = NULL;
        }

    // first open a view to secondary ssid table, with all the rows with service id
    // matching the wlan service id (count can be zero)        
    iSecondaryView = iDatabase.OpenViewMatchingUintLC(TPtrC(
            WLAN_SECONDARY_SSID), TPtrC(WLAN_SEC_SSID_SERVICE_ID), aId);
    if (iSecondaryView == NULL)
        {
        // this should not happen in a normal situation
        User::Leave(KErrCouldNotConnect);
        }
    CleanupStack::Pop(iSecondaryView);
    }

//-----------------------------------------------------------------------------
// void CWlanSettings::GetDataFromRecordL( TWlanSettings* aWlanSettings )
// Read settings from database to aWlanSettings structure
//-----------------------------------------------------------------------------
void CWlanSettings::GetDataFromRecordL(TWlanSettings* aWlanSettings)
    {
    _DBG_FILE("CWlanSettings::GetDataFromRecordL(): begin");

    TInt leavecode = 0;

    aWlanSettings->ServiceID = 0;
    aWlanSettings->ConnectionMode = 0;
    aWlanSettings->SSID = _L("");
    aWlanSettings->UsedSSID = _L("");
    aWlanSettings->ScanSSID = 0;
    aWlanSettings->WepKey1 = _L8("");
    aWlanSettings->WepKey2 = _L8("");
    aWlanSettings->WepKey3 = _L8("");
    aWlanSettings->WepKey4 = _L8("");
    aWlanSettings->WepIndex = 0;
    aWlanSettings->SecurityMode = EAllowUnsecure;
    aWlanSettings->WPAPreSharedKey = _L8("");
    aWlanSettings->UseWPAPSK = 0;
    aWlanSettings->EapList = _L("");
    aWlanSettings->AuthMode = 0;

    TRAP(leavecode, iTableView->ReadUintL(TPtrC(WLAN_SERVICE_ID),
            aWlanSettings->ServiceID));
    if (leavecode != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): WLAN_SERVICE_ID read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadUintL(TPtrC(WLAN_CONNECTION_MODE),
            aWlanSettings->ConnectionMode));
    if (leavecode != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): WLAN_CONNECTION_MODE read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadUintL(TPtrC(NU_WLAN_AUTHENTICATION_MODE),
            aWlanSettings->AuthMode));
    if (leavecode != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): NU_WLAN_AUTHENTICATION_MODE read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadUintL(TPtrC(WLAN_WEP_INDEX),
            aWlanSettings->WepIndex));
    if (leavecode != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): WLAN_WEP_INDEX read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadUintL(TPtrC(WLAN_SECURITY_MODE),
            aWlanSettings->SecurityMode));
    if (leavecode != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): WLAN_SECURITY_MODE read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadUintL(TPtrC(WLAN_ENABLE_WPA_PSK),
            aWlanSettings->UseWPAPSK));
    if (leavecode != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): WLAN_ENABLE_WPA_PSK read error: <%D>"),
                leavecode);
        }

    TBuf8<KCommsDbSvrMaxFieldLength> columnValue8;
    TRAP(leavecode, iTableView->ReadTextL(TPtrC(NU_WLAN_WEP_KEY1),
            columnValue8));
    if ((leavecode == KErrNone) && (columnValue8.Length() > 0))
        {
        aWlanSettings->WepKey1 = columnValue8;
        }
    else
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): NU_WLAN_WEP_KEY1 read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadTextL(TPtrC(NU_WLAN_WEP_KEY2),
            columnValue8));
    if ((leavecode == KErrNone) && (columnValue8.Length() > 0))
        {
        aWlanSettings->WepKey2 = columnValue8;
        }
    else
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): NU_WLAN_WEP_KEY2 read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadTextL(TPtrC(NU_WLAN_WEP_KEY3),
            columnValue8));
    if ((leavecode == KErrNone) && (columnValue8.Length() > 0))
        {
        aWlanSettings->WepKey3 = columnValue8;
        }
    else
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): NU_WLAN_WEP_KEY3 read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadTextL(TPtrC(NU_WLAN_WEP_KEY4),
            columnValue8));
    if ((leavecode == KErrNone) && (columnValue8.Length() > 0))
        {
        aWlanSettings->WepKey4 = columnValue8;
        }
    else
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): NU_WLAN_WEP_KEY4 read error: <%D>"),
                leavecode);
        }

    TBuf8<KMaxPSKLength> precolumnValue8;
    TBuf<KCommsDbSvrMaxFieldLength> columnValue;

    _DBG_FILE(
            "CWlanSettings::GetDataFromRecordL(): Before getting WLAN_WPA_PRE_SHARED_KEY ");
    TRAP(leavecode, iTableView->ReadTextL(TPtrC(WLAN_WPA_PRE_SHARED_KEY),
            precolumnValue8));
    DBG_ARGS(
            _S16(
                    "CWlanSettings::ReadTextL( TPtrC(WLAN_WPA_PRE_SHARED_KEY ),length of key: <%D>"),
            precolumnValue8.Length());
    if ((leavecode == KErrNone) && (precolumnValue8.Length() > 0))
        {
        aWlanSettings->WPAPreSharedKey = precolumnValue8;
        }
    else
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): WLAN_WPA_PRE_SHARED_KEY read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadTextL(TPtrC(NU_WLAN_SSID), columnValue));
    if ((leavecode == KErrNone) && (columnValue.Length() > 0))
        {
        aWlanSettings->SSID = columnValue;
        }
    else
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): NU_WLAN_SSID read error: <%D>"),
                leavecode);
        }

    TRAP(leavecode, iTableView->ReadTextL(TPtrC(WLAN_USED_SSID), columnValue));
    if ((leavecode == KErrNone) && (columnValue.Length() > 0))
        {
        aWlanSettings->UsedSSID = columnValue;
        _DBG_FILE(
                "CWlanSettings::GetDataFromRecordL(): WLAN_USED_SSID reading ok");
        }
    else
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): WLAN_USED_SSID read error: <%D>"),
                leavecode);
        }

    iTableView->ReadTextL(TPtrC(WLAN_USED_SSID), aWlanSettings->UsedSSID);
    _DBG_FILE("CWlanSettings::GetDataFromRecordL(): WLAN_USED_SSID");

    TRAP(leavecode, iTableView->ReadUintL(TPtrC(WLAN_SCAN_SSID),
            aWlanSettings->ScanSSID));
    if (leavecode != KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::GetDataFromRecordL(): WLAN_SCAN_SSID read error: <%D>"),
                leavecode);
        }

    TInt retval = iSecondaryView->GotoFirstRecord();
    if (retval == KErrNone)
        {
        aWlanSettings->SecondarySSIDsExisting = ETrue;
        }
    else
        {
        aWlanSettings->SecondarySSIDsExisting = EFalse;
        }

    _DBG_FILE("CWlanSettings::GetDataFromRecordL(): WLAN_EAPS");

    _DBG_FILE("CWlanSettings::GetDataFromRecordL(): end");
    }

//-----------------------------------------------------------------------------
// void CWlanSettings::GetSecondarySSIDListL(TUint32 aId, RArray<TSecondarySSID>& aSecondarySSIDs)
// Fills the array with secondarySSIDs, which are associated with given WLANID
//-----------------------------------------------------------------------------
void CWlanSettings::GetSecondarySSIDListL(TUint32 aWlanId, RArray<
        TSecondarySSID>& aSecondarySSIDs)
    {
    if (iSecondaryView == NULL)
        {
        // views haven't been initialized, in case of incorrect usage
        User::Leave(KErrNotReady);
        }

    // set the view pointer into the first record
    TInt retval = iSecondaryView->GotoFirstRecord();
    if (retval != KErrNone)
        {
        return;
        }

    // check that given id matches the current rowset. If not, then reinitialize
    TUint32 id;
    TInt err(KErrNone);
    TRAP(err, iSecondaryView->ReadUintL(TPtrC(WLAN_SEC_SSID_SERVICE_ID), id));
    if (id != aWlanId)
        {
        SetSecondaryViewToRecordL(aWlanId);
        retval = iSecondaryView->GotoFirstRecord();
        }

    TBuf<KMaxTextLength> columnValue;

    while (retval == KErrNone)
        {
        TSecondarySSID ssid;
        TRAP(retval, iSecondaryView->ReadUintL(TPtrC(WLAN_SEC_SSID_ID),
                ssid.Id));
        TRAP(retval, iSecondaryView->ReadTextL(TPtrC(
                WLAN_SEC_SSID_SCANNED_SSID), ssid.ScannedId));
        TRAP(retval, iSecondaryView->ReadTextL(
                TPtrC(WLAN_SEC_SSID_USED_SSID), ssid.UsedId));
        aSecondarySSIDs.Append(ssid);
        retval = iSecondaryView->GotoNextRecord();
        }
    }

//-----------------------------------------------------------------------------
// void CWlanSettings::WriteDataToRecordL( TWlanSettings* aWlanSettings )
// Writes settings to database from aWlanSettings structure
//-----------------------------------------------------------------------------
void CWlanSettings::WriteDataToRecordL(TWlanSettings* aWlanSettings)
    {
    _DBG_FILE("CWlanSettings::WriteDataToRecordL(): begin");

    iTableView->WriteUintL(TPtrC(WLAN_SERVICE_ID), aWlanSettings->ServiceID);
    iTableView->WriteUintL(TPtrC(WLAN_CONNECTION_MODE),
            aWlanSettings->ConnectionMode);
    iTableView->WriteUintL(TPtrC(WLAN_WEP_INDEX), aWlanSettings->WepIndex);
    iTableView->WriteUintL(TPtrC(WLAN_SECURITY_MODE),
            aWlanSettings->SecurityMode);
    iTableView->WriteUintL(TPtrC(WLAN_ENABLE_WPA_PSK),
            aWlanSettings->UseWPAPSK);
    iTableView->WriteUintL(TPtrC(NU_WLAN_AUTHENTICATION_MODE),
            aWlanSettings->AuthMode);
    iTableView->WriteTextL(TPtrC(NU_WLAN_SSID), aWlanSettings->SSID);
    iTableView->WriteTextL(TPtrC(WLAN_USED_SSID), aWlanSettings->UsedSSID);
    iTableView->WriteUintL(TPtrC(WLAN_SCAN_SSID), aWlanSettings->ScanSSID);

    iTableView->WriteTextL(TPtrC(NU_WLAN_WEP_KEY1), aWlanSettings->WepKey1);
    iTableView->WriteTextL(TPtrC(NU_WLAN_WEP_KEY2), aWlanSettings->WepKey2);
    iTableView->WriteTextL(TPtrC(NU_WLAN_WEP_KEY3), aWlanSettings->WepKey3);
    iTableView->WriteTextL(TPtrC(NU_WLAN_WEP_KEY4), aWlanSettings->WepKey4);
    iTableView->WriteUintL(TPtrC(WLAN_WEP_KEY1_FORMAT), 1); // 0 = Eascii, 1 = EHexadecimal
    iTableView->WriteUintL(TPtrC(WLAN_WEP_KEY2_FORMAT), 1);
    iTableView->WriteUintL(TPtrC(WLAN_WEP_KEY3_FORMAT), 1);
    iTableView->WriteUintL(TPtrC(WLAN_WEP_KEY4_FORMAT), 1);

    iTableView->WriteTextL(TPtrC(WLAN_WPA_PRE_SHARED_KEY),
            aWlanSettings->WPAPreSharedKey);
    iTableView->WriteUintL(TPtrC(WLAN_WPA_KEY_LENGTH),
            aWlanSettings->WPAPreSharedKey.Length());

    _DBG_FILE("CWlanSettings::WriteDataToRecordL(): end");
    }

//-----------------------------------------------------------------------------
// TInt CWlanSettings::RecordExists( TUint32 aLuid )
// Returns ETrue if record exists in database
//-----------------------------------------------------------------------------
TInt CWlanSettings::RecordExists(TUint32 aLuid)
    {
    _DBG_FILE("CWlanSettings::RecordExists(): begin");

    TInt err = KErrNone;
    TRAP(err, ConnectToDatabaseL());

    // If the connection went ok, then we can go to the record
    if (err == KErrNone)
        {
        err = GoToRecord(aLuid);
        }

    if (err == KErrNone)
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::RecordExists(): end. GoToRecord OK err: <%D>"),
                err);
        return ETrue;
        }
    else
        {
        DBG_ARGS(
                _S16(
                        "CWlanSettings::RecordExists(): end. GoToRecord error err: <%D>"),
                err);
        return err;
        }
    }

//-----------------------------------------------------------------------------
// void CWlanSettings::InitialiseRecordL( )
// Inserts mandatory values for commsdb
//-----------------------------------------------------------------------------
void CWlanSettings::InitialiseRecordL()
    {
    _DBG_FILE("CWlanSettings::InitialiseRecordL(): begin");

    iTableView->WriteUintL(TPtrC(WLAN_WPA_KEY_LENGTH), 0);
    iTableView->WriteUintL(TPtrC(NU_WLAN_AUTHENTICATION_MODE), 0);

    _DBG_FILE("CWlanSettings::InitialiseRecordL(): end");
    }

TInt CWlanSettings::InstalledEAPsL(CBufBase& aEAPList)
    {
    _DBG_FILE("CWlanSettings::InstalledEAPsL(): begin");

    RImplInfoPtrArray ecomInfoArray;
    REComSession::ListImplementationsL(KEapTypeInterfaceUid, ecomInfoArray);

    TUint eapId(0);

    // First sort the EAPIds
    for (TInt i = 0; i < ecomInfoArray.Count(); i++)
        {
        if (ecomInfoArray[i]->DataType().Length() == KExpandedEAPIdLength)
            {
            // The EAP ID is in expanded type format
            // Only handle the vendor type of 0
            if ((ecomInfoArray[i]->DataType()[1] != 0
                    || ecomInfoArray[i]->DataType()[2] != 0
                    || ecomInfoArray[i]->DataType()[3] != 0)
                    && ((ecomInfoArray[i]->DataType()[7]
                            != KMschapv2TypeId[7])
                            && (ecomInfoArray[i]->DataType()[7]
                                    != KTtlspapTypeId[7])))
                {
                // This is some other vendor type than IETF
                continue;
                }
            eapId = ecomInfoArray[i]->DataType()[7];
            _DBG_FILE(
                    "CWlanSettings::InstalledEAPsL(): Expanded EAP id found.");
            }
        else
            {
            // The EAP ID is in old 8-bit format (as string)
            TLex8 lex(ecomInfoArray[i]->DataType());
            if (lex.Val(eapId) != KErrNone)
                {
                // Unsupported format. Ignore this.
                _DBG_FILE(
                        "CWlanSettings::InstalledEAPsL(): Unsupported EAP id found.");
                continue;
                }
            _DBG_FILE(
                    "CWlanSettings::InstalledEAPsL(): Old style EAP id found.");
            }

        if (!IsDisallowedOutsidePEAP(*ecomInfoArray[i]))
            {
            aEAPList.InsertL(aEAPList.Size(), KNSmlWLanEapId); // EAPId
            TBuf8<3> cue; // Max EAPId == 255
            cue.AppendNumFixedWidth(eapId, EDecimal, 3);
            aEAPList.InsertL(aEAPList.Size(), cue);
            aEAPList.InsertL(aEAPList.Size(), KSlash);
            }

        // Check if the method is allowed inside PEAP
        if (!IsDisallowedInsidePEAP(*ecomInfoArray[i]))
            {
            aEAPList.InsertL(aEAPList.Size(), KNSmlWLanEapId); // EAPId
            TBuf8<3> cue; // Max EAPId == 255
            cue.AppendNumFixedWidth(eapId, EDecimal, 3);
            aEAPList.InsertL(aEAPList.Size(), cue);
            aEAPList.InsertL(aEAPList.Size(), KDash);
            cue.Zero();
            cue.AppendNumFixedWidth(KEapPeapTypeId[7], EDecimal, 3);
            aEAPList.InsertL(aEAPList.Size(), cue);
            aEAPList.InsertL(aEAPList.Size(), KSlash);

#ifdef FF_WLAN_EXTENSIONS
            // same additions for EAP-FAST
            aEAPList.InsertL(aEAPList.Size(), KNSmlWLanEapId); // EAPId
            cue.Zero(); // Max EAPId == 255
            cue.AppendNumFixedWidth( eapId, EDecimal, 3 );
            aEAPList.InsertL(aEAPList.Size(), cue);
            aEAPList.InsertL(aEAPList.Size(), KDash);
            cue.Zero();
            cue.AppendNumFixedWidth( KEapFastTypeId[7], EDecimal, 3 );
            aEAPList.InsertL(aEAPList.Size(), cue);
            aEAPList.InsertL(aEAPList.Size(), KSlash);
#endif              
            }
        // Check if the method is allowed inside TTLS
        if (!IsDisallowedInsideTTLS(*ecomInfoArray[i]))
            {
            aEAPList.InsertL(aEAPList.Size(), KNSmlWLanEapId); // EAPId
            TBuf8<3> cue; // Max EAPId == 255
            cue.AppendNumFixedWidth(eapId, EDecimal, 3);
            aEAPList.InsertL(aEAPList.Size(), cue);
            aEAPList.InsertL(aEAPList.Size(), KDash);
            cue.Zero();
            cue.AppendNumFixedWidth(KEapTtlsTypeId[7], EDecimal, 3);
            aEAPList.InsertL(aEAPList.Size(), cue);
            aEAPList.InsertL(aEAPList.Size(), KSlash);
            }
        }
    // ECOM array is no longer needed.
    ecomInfoArray.ResetAndDestroy();

    // Cut the last slash
    aEAPList.ResizeL(aEAPList.Size() - 1);

    _DBG_FILE("CWlanSettings::InstalledEAPsL(): end");
    return KErrNone;
    }

//-----------------------------------------------------------------------------
// TBool CWlanSettings::CheckEnforcementL( )
// Checks if wlan enforcement is On or Off
//-----------------------------------------------------------------------------
TBool CWlanSettings::CheckEnforcementL()
    {
    CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
    CleanupStack::PushL(info);
    TBool enforceWLANActive(EFalse);
    User::LeaveIfError(info->EnforcementActive(EWLANEnforcement,
            enforceWLANActive));
    CleanupStack::PopAndDestroy(info);
    return (enforceWLANActive);
    }

//-----------------------------------------------------------------------------
// TBool CWlanSettings::CheckAPEnforcementL( )
// Checks if AP enforcement is On or Off
//-----------------------------------------------------------------------------

TBool CWlanSettings::CheckAPEnforcementL()
    {
    CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
    CleanupStack::PushL(info);
    TBool enforceAPActive(EFalse);
    User::LeaveIfError(info->EnforcementActive(EAPEnforcement,
            enforceAPActive));
    CleanupStack::PopAndDestroy(info);
    return (enforceAPActive);
    }
//-----------------------------------------------------------------------------
// TInt CWlanSettings::PerformLockWLANTablesL( )
// Locks or unlocks the wlan tables depending on the 
// aProtect value
//-----------------------------------------------------------------------------
TInt CWlanSettings::PerformLockWLANTablesL(TBool aProtect)
    {

    _DBG_FILE("CWlanSettings::PerformLockWLANTablesL(): BEGIN ");
    CCommsDatabaseProtect* dbprotect = CCommsDatabaseProtect::NewL();
    CleanupStack::PushL(dbprotect);
    RArray<TPtrC> wlanTableList;
    CleanupClosePushL(wlanTableList);
    TBuf<KCommsDbSvrMaxFieldLength> serviceType;
    TBool apEnforce = EFalse;
    TInt err = KErrNone;

    TRAP_IGNORE(apEnforce = CheckAPEnforcementL());
    if (apEnforce)
        {
        _DBG_FILE(
                "CWlanSettings::PerformLockWLANTablesL(): AP enforcement is TRUE");
        wlanTableList.AppendL(TPtrC(WAP_ACCESS_POINT));
        wlanTableList.AppendL(TPtrC(WAP_IP_BEARER));
        wlanTableList.AppendL(TPtrC(IAP));
        //condition when only ap is enforced and not wlan then its not 
        //necessary to add wlanservice table
        RDbRowSet::TAccess checkAccessType;
        checkAccessType = dbprotect->GetTableAccessL(TPtrC(
                WLAN_DEVICE_SETTINGS));
        switch (checkAccessType)
            {
            case RDbRowSet::EReadOnly:
                {
                wlanTableList.AppendL(TPtrC(WLAN_SERVICE));
                wlanTableList.AppendL(TPtrC(WLAN_SECONDARY_SSID));
                }
            default:
                {
                break;
                }
            }

        }
    else
        {
        _DBG_FILE(
                "CWlanSettings::PerformLockWLANTablesL() Only wlan enforcement is ON");
        wlanTableList.AppendL(TPtrC(WLAN_SERVICE));
        wlanTableList.AppendL(TPtrC(WLAN_SECONDARY_SSID));
        }

    iDatabase.CommitTransaction();

    for (TInt i(0); i < wlanTableList.Count(); i++)
        {
        if (!aProtect)
            {
            err = dbprotect->UnProtectTable(wlanTableList[i]);
            DBG_ARGS(
                    _S16(
                            "CWlanSettings::PerformLockWLANTablesL(): Unprotect error  %D"),
                    err);
            //Check if current AP is locked , if yes , unprotect 
            // lock = EFalse;
            }
        else
            {
            err = dbprotect->ProtectTable(wlanTableList[i]);
            //Check if current ServiceID is not locked , if yes , protect
            // lock = ETrue;
            }
        } //for loop

    CleanupStack::PopAndDestroy(&wlanTableList);
    CleanupStack::PopAndDestroy(dbprotect);
    CCommsDatabase* commsDataBase = CCommsDatabase::NewL();
    CleanupStack::PushL(commsDataBase);

    // Open the IAP from IAP Table and protect 

    TUint32 apIAPID = 0;
    CCommsDbTableView* iapViews = commsDataBase->OpenViewMatchingUintLC(
            TPtrC(IAP), TPtrC(IAP_SERVICE), iServiceID);
    err = iapViews->GotoFirstRecord();
    // Service ID can be same for GPRS or WLAN, hence check service type and get the iAPID
    while (err == KErrNone)
        {
        iapViews->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
        if (serviceType == TPtrC(LAN_SERVICE))
            {
            iapViews->ReadUintL(TPtrC(COMMDB_ID), apIAPID);
            DBG_ARGS(_S16(
                    "CWlanSettings::PerformLockWLANTablesL():  IAPId   %D"),
                    apIAPID);
            break;
            }
        err = iapViews->GotoNextRecord();
        }

    //iapViews->ReadUintL(TPtrC(COMMDB_ID), apIAPID);
    DBG_ARGS(
            _S16(
                    "CWlanSettings::PerformLockWLANTablesL(): end. IAP ID being locked %d "),
            apIAPID);
    if (err == KErrNone)
        {
        CCommsDbTableView* iapUpdate = commsDataBase->OpenViewMatchingUintLC(
                TPtrC(IAP), TPtrC(COMMDB_ID), apIAPID);
        TInt iapExists = iapUpdate->GotoFirstRecord();

        if (!aProtect)
            {
            ((CCommsDbProtectTableView*) iapViews)->UnprotectRecord();
            if (iapExists)
                {
                ((CCommsDbProtectTableView*) iapUpdate)->UnprotectRecord();
                }
            }
        else
            {
            ((CCommsDbProtectTableView*) iapViews)->ProtectRecord();
            if (iapExists)
                {
                ((CCommsDbProtectTableView*) iapUpdate)->ProtectRecord();
                }
            }
        CleanupStack::PopAndDestroy(); // iapUpdate
        }

    CleanupStack::PopAndDestroy(iapViews);
    CleanupStack::PopAndDestroy(commsDataBase);

    err = iDatabase.BeginTransaction();
    return err;
    }

TBool CWlanSettings::IsDisallowedOutsidePEAP(
        const CImplementationInformation& aImplInfo)
    {

    const TUint8 pluginOpaqueData = *(aImplInfo.OpaqueData().Ptr());

    if (pluginOpaqueData & KNotOutsidePEAP)
        {
        return ETrue;
        }
    return EFalse;

    }

TBool CWlanSettings::IsDisallowedInsidePEAP(
        const CImplementationInformation& aImplInfo)
    {
    const TUint8 pluginOpaqueData = *(aImplInfo.OpaqueData().Ptr());

    if (pluginOpaqueData & KNotInsidePEAP)
        {
        return ETrue;
        }
    return EFalse;

    }

TBool CWlanSettings::IsDisallowedInsideTTLS(
        const CImplementationInformation& aImplInfo)
    {
    const TUint8 pluginOpaqueData = *(aImplInfo.OpaqueData().Ptr());

    if (pluginOpaqueData & KNotInsideTTLS)
        {
        return ETrue;
        }
    return EFalse;
    }

