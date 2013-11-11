/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */


#include <centralrepository.h>
#include <s32mem.h>
#include <s32file.h>
#include <e32des8.h>
#ifndef __SERIES60_30__
#include "AMAppHideUtil.h"
#endif
#include "debug.h"
#include "amprivateCRKeys.h"
#include "amstorage.h"
#include "AMDeploymentComponent.h"

using namespace NApplicationManagement;

// CONSTANTS


/***************************************************
 // RComponentIdArray
 ***************************************************/

void RComponentIdArray::SetListL(const TDesC8& aChilds)
    {
#ifdef _DEBUG
    HBufC *b = HBufC::NewLC(aChilds.Length() );
    TPtr p(b->Des() );
    p.Copy(aChilds);
    RDEBUG_2( "RComponentIdArray::SetListL: Setting list from: '%S'", &p);
    CleanupStack::PopAndDestroy(b);
#endif

    RDesReadStream buf(aChilds);
    //TInt size( buf.Source()->TellL( MStreamBuf::ERead ).SizeL() );
    TInt left(aChilds.Length() );
    const TInt KSize(sizeof(TUint32));
    while (left > 0)
        {
        TUint32 val(buf.ReadUint32L() );
#ifdef _DEBUG
        RDEBUG_2( "RComponentIdArray::SetListL: Appending id: '%d'", val);
#endif
        Append(val);
        left -= KSize;
        }
    buf.Close();
    }

void RComponentIdArray::RemoveByValue(TUint32 aValue)
    {
    TInt ind(Find(aValue) );
    Remove(ind);
    }

void RComponentIdArray::GetListLC(HBufC8 *&aChilds, TInt &aLength)
    {
    ASSERT( aChilds == NULL );
    TInt c(Count() );
    if (c > 0)
        {
        aLength = c * sizeof(TUint32);
        aChilds = HBufC8::NewLC(aLength);
        TPtr8 p(aChilds->Des() );
        RDesWriteStream stream(p);
        CleanupClosePushL(stream);
        TInt i( 0);
        do
            {
            stream.WriteUint32L(operator[](i) );
            }
        while ( ++i < c);
        stream.CommitL();
        CleanupStack::PopAndDestroy( &stream);
        }
    else
        {
        aChilds = KNullDesC8().AllocLC();
        aLength = 0;
        }
    }

/*	
 void TCertInfoPckg::ExternalizeL(RWriteStream& aStream) const
 {
 aStream << iId;
 TPckgBuf<TCertInfo>::ExternalizeL( aStream );
 }

 void TCertInfoPckg::InternalizeL(RReadStream& aStream)
 {
 
 }
 */

void RCertArray::ExternalizeL(RWriteStream& aStream) const
    {
    TInt c(Count() );
    aStream.WriteInt32L(c);
    for (TInt i( 0); i < c; i++)
        {
        aStream.WriteInt32L(i);
        aStream << (*(operator[](i)));
        }
    }

void RCertArray::InternalizeL(RReadStream& aStream)
    {
    TInt c(aStream.ReadInt32L() );
    for (TInt i( 0); i < c; i++)
        {
        TCertInfoPckg *pkg = new ( ELeave ) TCertInfoPckg;
        RDEBUG_2( "RCertArray::InternalizeL - allocated TCertInfoPckg 0x%X", reinterpret_cast<TUint>( pkg ) );
        CleanupStack::PushL(pkg);
        TInt idx(aStream.ReadInt32L() );
        aStream >> (*pkg);
        Append(pkg);
        CleanupStack::Pop(pkg);
        }
    }

TInt RCertArray::FindByValue(const TCertInfoPckg &aPckg)
    {
    TInt c(Count() );
    for (TInt i( 0); i < c; i++)
        {
        if (operator[](i)->Compare(aPckg) == 0)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

/***************************************************
 // CDeliveryComponentStorage
 ***************************************************/

CDeliveryComponentStorage::CDeliveryComponentStorage()
    {

    }

void CDeliveryComponentStorage::ConstructL()
    {
#ifndef __SERIES60_30__
    iHidder = CAMAppHideUtil::NewL();
#endif
    LoadCertsL();
    TRAPD( erx, iRepository = CRepository::NewL ( KCRUidPrivateApplicationManagementKeys ) )
    ;
    if (erx != KErrNone)
        {
        RDEBUG_2( "CDeliveryComponentStorage::ConstructL() ERROR CentRep not initialized: Check deployment! %d", erx );
        User::Leave(erx);
        }
    TInt err(iRepository->Get(KNextIdKey, iNextId) );
    if (err == KErrNotFound)
        {
        iNextId = KMinIdValue;
        User::LeaveIfError(iRepository->Create(KNextIdKey, iNextId) );
        }
    else
        {
        User::LeaveIfError(err);
        }
    LoadComponentsL();
    }

void CDeliveryComponentStorage::LoadCertsL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect() );
    CleanupClosePushL(fs);
    TInt e(fs.CreatePrivatePath(KDefaultDrive) );

    User::LeaveIfError(e);
    TBuf<64> privatePath;
    User::LeaveIfError(fs.PrivatePath(privatePath) );

    TInt privPathLength(privatePath.Length() );
    _LIT( KCertFile, "amcerts.dat");
    HBufC *afile = HBufC::NewLC(privPathLength + KCertFile().Length());
    *afile = privatePath;
    afile->Des().Append(KCertFile);
    RFileReadStream certFile;
    TInt err(certFile.Open(fs, *afile, EFileRead) );
    if (err == KErrNone)
        {
        CleanupClosePushL(certFile);
        iCertificates.InternalizeL(certFile);
        CleanupStack::PopAndDestroy( &certFile);
        }
    else
        if (err == KErrNotFound)
            {

            }
        else
            {
            User::Leave(err);
            }
    CleanupStack::PopAndDestroy(afile);
    CleanupStack::PopAndDestroy( &fs);
    }

CDeliveryComponentStorage* CDeliveryComponentStorage::NewLC()
    {
    CDeliveryComponentStorage *self = new ( ELeave ) CDeliveryComponentStorage( );
    CleanupStack::PushL(self) ;
    self->ConstructL() ;
    return self;
    }

CDeliveryComponentStorage* CDeliveryComponentStorage::NewL()
    {
    CDeliveryComponentStorage *self = NewLC();
    CleanupStack::Pop(self) ;
    return self;
    }

TInt CDeliveryComponentStorage::NextKey()
    {
    TInt oldNext(iNextId++);
    iRepository->Set(KNextIdKey, iNextId);
    return oldNext;
    }

CDeliveryComponentStorage::~CDeliveryComponentStorage()
    {
    RDEBUG_3( "CDeliveryComponentStorage::~CDeliveryComponentStorage 0x%X - 0x%X", reinterpret_cast<TUint>(this),
            reinterpret_cast<TUint>(this)+sizeof( CDeliveryComponentStorage ) );
    delete iRepository;
    iRepository = NULL;
    iComponents.ResetAndDestroy();
    iComponentIds.Close();
    TRAPD( err,CloseCertificatesL() )
    ;
    if (err != KErrNone)
        {
        RDEBUG_2( "CDeliveryComponentStorage::~CDeliveryComponentStorage: ERROR Failed to close certificate storage properly: %d", err);
        }
#ifndef __SERIES60_30__
    delete iHidder;
    iHidder = NULL;
#endif
    }

void CDeliveryComponentStorage::CloseCertificatesL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect() );
    CleanupClosePushL(fs);
    TInt e(fs.CreatePrivatePath(KDefaultDrive) );

    User::LeaveIfError(e);
    TBuf<64> privatePath;
    User::LeaveIfError(fs.PrivatePath(privatePath) );

    TInt privPathLength(privatePath.Length() );
    _LIT( KCertFile, "amcerts.dat");
    HBufC *afile = HBufC::NewLC(privPathLength + KCertFile().Length());
    *afile = privatePath;
    afile->Des().Append(KCertFile);
    RFileWriteStream certFile;

    TInt err(certFile.Replace(fs, *afile, EFileWrite) );
    if (err == KErrNone)
        {
        CleanupClosePushL(certFile);
        iCertificates.ExternalizeL(certFile);
        CleanupStack::PopAndDestroy( &certFile);
        }
    else
        if (err == KErrNotFound)
            {

            }
        else
            {
            User::Leave(err);
            }
    CleanupStack::PopAndDestroy(afile);
    CleanupStack::PopAndDestroy( &fs);
    iCertificates.ResetAndDestroy();
    }

const RComponentIdArray &CDeliveryComponentStorage::GetComponentIds() const
    {
    return iComponentIds;
    }

void CDeliveryComponentStorage::GetStateChangeComponentIdsL(
        RComponentIdArray &aArray)
    {
    RDEBUG_2( "CDeliveryComponentStorage::GetStateChangeComponentIdsL %d dcs", iComponentIds.Count());
    aArray.Reset();
    TInt c(iComponentIds.Count() );
    for (TInt i( 0); i < c; i++)
        {
        CDeploymentComponent &compo = ComponentL(iComponentIds[i]);
        if (compo.OldState() != EDCSNone)
            {
            aArray.Append(iComponentIds[i]);
            }
        }
    RDEBUG( "CDeliveryComponentStorage::GetStateChangeComponentIdsL end");
    }

void CDeliveryComponentStorage::StateChangedL(TUint32 aInternalId)
    {
    RDEBUG_2( "CDeliveryComponentStorage::StateChangedL id %d", aInternalId);
    CDeploymentComponent &compo = ComponentL(aInternalId);
    compo.StateChangeComplete();
    UpdateL(compo);
    }

void CDeliveryComponentStorage::LoadComponentsL()
    {
    TInt length( 0);
    TInt err(iRepository->Get(KIdListLengthKey, length) );
    if (err == KErrNotFound)
        {
        length = 0;
        User::LeaveIfError(iRepository->Create(KIdListLengthKey, length) );
        }
    else
        {
        User::LeaveIfError(err);
        }
    HBufC8 *listbuf = HBufC8::NewLC(length) ;
    TPtr8 ptr(listbuf->Des() );
    err = iRepository->Get(KIdListKey, ptr) ;
    if (err == KErrNotFound)
        {
        err = iRepository->Create(KIdListKey, KNullDesC8);
        }
    else
        {

        }
    User::LeaveIfError(err);
    TRAPD(code, iComponentIds.SetListL( ptr ))
    ;
    if (code != KErrEof)
        {
        User::LeaveIfError(code);
        }

    CleanupStack::PopAndDestroy(listbuf);
#ifndef __SERIES60_30__
    TInt i(iComponentIds.Count() );
    //iHidder->Reset();
    while ( --i >= 0)
        {
        CDeploymentComponent &comp = ComponentL(iComponentIds[i]);
        if (comp.State() == EDCSInactive)
            {
            iHidder->AddUidL(comp.Uid());
            }
        else
            if (comp.State() == EDCSActive)
                {
                iHidder->RemoveUidL(comp.Uid() );
                }
        }
    iHidder->PersistUidsL();
#endif

    }

CDeploymentComponent *CDeliveryComponentStorage::NewComponentL(
        const TDeploymentComponentState &aState, const TDCUserId &aUserId,
        const TCertInfo *aCertInfo/* = NULL */)
    {
    CDeploymentComponent *newc = CDeploymentComponent::NewLC(NextKey(),
            aUserId);
    if (aCertInfo != NULL)
        {
        TCertInfoPckg *p = new( ELeave ) TCertInfoPckg(*aCertInfo);
        RDEBUG_2( "CDeliveryComponentStorage::NewComponentL - allocated TCertInfoPckg 0x%X", reinterpret_cast<TUint>( p ) );

        TInt idx(iCertificates.FindByValue(*p) );
        if (idx == KErrNotFound)
            {
            RDEBUG_2( "CDeliveryComponentStorage::NewComponentL - TCertInfoPckg NOT found 0x%X", reinterpret_cast<TUint>( p ) );
            TRAPD(err,iCertificates.AppendL(p));
            if(err != KErrNone)
                {
                delete p;
                p = NULL;
				User::Leave( err );
                }
            else
                {
                idx = iCertificates.Count() - 1;
                }
            }
        else
            {
            RDEBUG_2( "CDeliveryComponentStorage::NewComponentL - TCertInfoPckg found at %d, deleting temporary", idx );
            delete p;
            p = NULL;
            }
        if (idx >= 0)
            {
            RDEBUG_2( "CDeliveryComponentStorage::NewComponentL - TCertInfoPckg found 0x%X", reinterpret_cast<TUint>( p ) );
            newc->SetOwner(idx);
            }
        else
            {
            RDEBUG_3( "CDeliveryComponentStorage::NewComponentL - WARNING Could not add certificate 0x%X: %d", reinterpret_cast<TUint>( p ), idx );

            }
        }
    newc->SetState(aState);
    newc->SetStatusNode(EIdle);
    UpdateL( *newc);
    iComponents.AppendL(newc);
    iComponentIds.AppendL(newc->InternalId() );
RDEBUG_2( "CDeliveryComponentStorage::NewComponentL -Internal ID is  %d, ", newc->InternalId() );
    PersistStateL();
    CleanupStack::Pop(newc);
    return newc;
    }

void CDeliveryComponentStorage::UpdateL(
        const CDeploymentComponent &aComponent)
    {
    aComponent.PersistL( *iRepository) ;

#ifndef __SERIES60_30__
    if (aComponent.State() == EDCSInactive)
        {
        iHidder->AddUidL(aComponent.Uid(), ETrue);
        }
    else
        {
        iHidder->RemoveUidL(aComponent.Uid(), ETrue);
        }
#endif
    }

void CDeliveryComponentStorage::PersistStateL()
    {
    TInt length( 0);
    HBufC8 *buf= NULL;
    iComponentIds.GetListLC(buf, length);
    User::LeaveIfError(iRepository->Set(KIdListLengthKey, length) );
    User::LeaveIfError(iRepository->Set(KIdListKey, *buf) );
    CleanupStack::PopAndDestroy(buf);
    }

void CDeliveryComponentStorage::RemoveL(TUint32 aInternalId)
    {
    RDEBUG_2("CDeliveryComponentStorage::RemoveL - Remove id: (%d)", aInternalId );
    TInt count(iComponents.Count() );
    for (TInt i( 0); i < count; i++)
        {
        CDeploymentComponent *el = iComponents[i];
        if (aInternalId == el->iInternalId)
            {
#ifndef __SERIES60_30__
            iHidder->RemoveUidL(el->Uid(), ETrue);
#endif
            iComponents.Remove(i);
            iComponentIds.RemoveByValue(aInternalId);
            el->DestroyL( *iRepository);
            delete el;
            RDEBUG_2("CDeliveryComponentStorage::RemoveL - Removed id: (%d)", aInternalId );
            PersistStateL();
            break;
            }
        }
    }

CDeploymentComponent &CDeliveryComponentStorage::ComponentL(
        TUint32 aInternalId)
    {
    CDeploymentComponent *resp= NULL;
    TInt count(iComponents.Count() );
    for (TInt i( 0); i < count; i++)
        {
        CDeploymentComponent *el = iComponents[i];
        if (aInternalId == el->iInternalId)
            {
            resp = el;
            break;
            }
        }
    if (resp == NULL)
        {
        TBuf8< NCentralRepositoryConstants::KMaxBinaryLength> buf;
        TInt err(iRepository->Get(aInternalId, buf) );
        User::LeaveIfError(err) ;
        resp = LoadComponentL(aInternalId, buf) ;
        iComponents.AppendL(resp);
        }
    if (resp == NULL)
        {
        User::Leave(KErrNotFound);
        }
    return *resp;
    }

CDeploymentComponent *CDeliveryComponentStorage::LoadComponentL(
        TInt aInternalId, const TDesC8 &aBuffer) const
    {
    CDeploymentComponent *res = CDeploymentComponent::LoadL(aInternalId,
            aBuffer);
#ifdef _DEBUG
    if (res)
        {
        RDEBUG8_4( "CDeliveryComponentStorage::LoadComponentL - loaded component state %d, internalid %d, and userid: %S", res->State(), res->InternalId(),&res->UserId() );
        }
    else
        {
        RDEBUG( "CDeliveryComponentStorage::LoadComponentL - WARNING failed to load component" );
        }
#endif
    return res;
    }
void CDeliveryComponentStorage::CheckForDuplicateNodesInDeployedL(
        CDeploymentComponent &aComponent)
    {
    TInt CompIdsCount(iComponentIds.Count() );
    RDEBUG8_2("CheckForDuplicateNodesInDeployedL: CompIdsCount-> (%d)", CompIdsCount );
    RDEBUG8_2("CheckForDuplicateNodesInDeployedL: aComponent UserID is %S", &(aComponent.UserId()));
    for (TInt i( 0); i < CompIdsCount; i++)
        {
        CDeploymentComponent &compo = ComponentL(iComponentIds[i]);
        RDEBUG8_2("CheckForDuplicateNodesInDeployedL: compo UserID is %S", &(compo.UserId()));
        if (&aComponent != &compo)
        {
        if (((compo.State()== EDCSActive) || (compo.State()== EDCSInactive))
                &&(compo.UserId() == aComponent.UserId()))
            {
            (aComponent.iUserId).Append(*IntToDes8LC(aComponent.iInternalId));
            CleanupStack::PopAndDestroy();
            break;
            }
          }
        }
    }

HBufC8* CDeliveryComponentStorage::IntToDes8LC(const TInt aLuid)
    {
    HBufC8* buf = HBufC8::NewLC( 10); //10 = max length of 32bit integer
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num(aLuid);
    return buf;
    }

void CDeliveryComponentStorage::InstalledL(CDeploymentComponent &aComponent)
    {
    CheckForDuplicateNodesInDeployedL(aComponent);
    aComponent.SetState(EDCSActive);
    // set state to IDLE 
    aComponent.SetStatusNode(EIdle);
    UpdateL(aComponent);
    }

void CDeliveryComponentStorage::DeactivateL(CDeploymentComponent &aComponent)
    {
    /* :
     * set state to InActivate in progress
     * use TRAP instead of leaving.
     */
    aComponent.SetStatusNode(EDeployed_DeactivateProgress);
    //User::LeaveIfError(aComponent.SetState(EDCSInactive) );
    TInt err = aComponent.SetState(EDCSInactive);
    if (err == KErrNone)
        {
        TRAP(err,UpdateL(aComponent));
        }
    //UpdateL(aComponent);
    /*:
     * if error set state to InActivate failed or else set to Idle
     * call userleaveiferror()
     */
    if (err == KErrNone)
        {
        aComponent.SetStatusNode(EIdle);
        }
    else
        {
        aComponent.SetStatusNode(EDeployed_DeactivateFailed);
        }
    User::LeaveIfError(err);
    }

void CDeliveryComponentStorage::SetDeploymentComponentState(CDeploymentComponent &aComponent,TDeploymentComponentState aState)
    {
    aComponent.SetState( aState );
    TRAP_IGNORE(UpdateL( aComponent ));    
    }

void CDeliveryComponentStorage::ActivateL(CDeploymentComponent &aComponent)
    {
    /* :
     * set state to Activate in progress
     * use TRAP instead of leaving.
     */
    aComponent.SetStatusNode(EDeployed_ActivateProgress);
    TInt err = aComponent.SetState(EDCSActive);
    //UpdateL( aComponent );
    if (err == KErrNone)
        {
        TRAP(err,UpdateL(aComponent));
        }
    /*:
     * if error set state to InActivate failed or else set to Idle
     * call userleaveiferror()
     */
    if (err == KErrNone)
        {
        aComponent.SetStatusNode(EIdle);
        }
    else
        {
        aComponent.SetStatusNode(EDeployed_ActivateFailed);
        }
    User::LeaveIfError(err);
    }

/*void CDeliveryComponentStorage::UninstalledL( CDeploymentComponent &aComponent )
 {
 aComponent.SetState( EDCSInactive );
 UpdateL( aComponent );
 }
 
 void CDeliveryComponentStorage::ResetL()
 {
 }
 */
//  End of File
