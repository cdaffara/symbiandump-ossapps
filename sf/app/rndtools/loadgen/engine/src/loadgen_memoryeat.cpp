/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "loadgen_memoryeat.h"
#include "loadgen_utils.h"
#include "loadgen.hrh"
#include "loadgen_traces.h"
#include <loadgen.rsg>

#include <e32math.h>

_LIT(KThreadName, "MemEat %d");
_LIT(KChunkName, "LoadGen %d");
_LIT(KFilePath, "%c:\\system\\temp\\LoadGen-%d_%d.$$$");

// currently can only handle 2GB
const TInt64 KMaxEatSize = 2147483647;

const TInt KDefaultStart = 50;
const TInt KDefaultPeriod = 5000000;
    
// ===================================== MEMBER FUNCTIONS =====================================

CMemoryEat* CMemoryEat::NewL(TMemoryEatAttributes& aAttributes, TInt aReferenceNumber)
    {
    CMemoryEat* self = new(ELeave) CMemoryEat(aAttributes, aReferenceNumber);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// --------------------------------------------------------------------------------------------

CMemoryEat::~CMemoryEat()
    {
    Close();
    }

// --------------------------------------------------------------------------------------------

CMemoryEat::CMemoryEat(TMemoryEatAttributes& aAttributes, TInt aReferenceNumber) : iAttributes(aAttributes)
    {
    iAttributes.iId = aReferenceNumber;
    }

// --------------------------------------------------------------------------------------------

void CMemoryEat::ConstructL()
    {
    CLoadBase::ConstructL();
    
    iType = ELoadGenCmdNewLoadEatMemory;
    
    TBuf<64> threadName;
    threadName.Format(KThreadName, iAttributes.iId);
    
    // create a thread
    User::LeaveIfError(iThread.Create(threadName, ThreadFunction, KDefaultStackSize*2, KMinHeapSize, 32*1024*KMinHeapSize, (TAny*) &iAttributes ));
    
    // set priority of the thread
    SetPriority();
    }

// --------------------------------------------------------------------------------------------

TInt CMemoryEat::ThreadFunction(TAny* aThreadArg)
    {
    CTrapCleanup* pC = CTrapCleanup::New();
    CActiveScheduler* pS = new CActiveScheduler;
    CActiveScheduler::Install(pS);

    // start memory eat, pass pointer to arguments
    GenerateLoad(*((TMemoryEatAttributes*) aThreadArg));

    delete pS;
    delete pC;

    return KErrNone;
    }

// --------------------------------------------------------------------------------------------

void CMemoryEat::GenerateLoad(TMemoryEatAttributes& aAttributes)
    {
    CMemoryEatManager* memoryEatManager = NULL;
    TRAPD(err, memoryEatManager = CMemoryEatManager::NewL(aAttributes));
    if (err == KErrNone) CActiveScheduler::Start();
    delete memoryEatManager;
    }

// --------------------------------------------------------------------------------------------

void CMemoryEat::Resume()
    {
    CLoadBase::Resume();
    
    iThread.Resume();
    }

// --------------------------------------------------------------------------------------------

void CMemoryEat::Suspend()
    {
    CLoadBase::Suspend();
    
    iThread.Suspend();
    }

// --------------------------------------------------------------------------------------------

void CMemoryEat::SetPriority()
    {
    CLoadBase::SetPriority();
    
    iThread.SetPriority(CLoadGenUtils::SettingItemToThreadPriority(iAttributes.iPriority));
    }
    
// --------------------------------------------------------------------------------------------

void CMemoryEat::Close()
    {
    CLoadBase::Close();
    
    if (iThread.ExitReason() == 0) // check if the thread is still alive
        {
        // signal the thread that it needs to close
        iThread.RequestComplete(iAttributes.iDeathStatus, KErrCancel);

        // wait the thread to die
        TRequestStatus waiter;
        iThread.Logon(waiter);
        User::WaitForRequest(waiter);
        iThread.Close();
        }
    }
    
// --------------------------------------------------------------------------------------------

TPtrC CMemoryEat::Description()
    {
    TBuf<256> buf;
    TBuf<16> prioBuf;
    CLoadGenUtils::SettingItemToThreadDescription(iAttributes.iPriority, prioBuf);
    TBuf<16> srcBuf;
    CLoadGenUtils::SettingItemToSourceDescription(iAttributes.iSource, srcBuf);
    
    if (iAttributes.iType == EMemoryEatTypeMemoryToEat)
        {
        _LIT(KMemoryEatEntryMemoryToEat, "[%d] MemEat src=%S prio=%S type=MemToEat amount=%LDB buf=%dB idle=%dms");
        
        buf.Format(KMemoryEatEntryMemoryToEat, iAttributes.iId, &srcBuf, &prioBuf, iAttributes.iAmount, iAttributes.iBuffer, iAttributes.iIdle);
        }
    else if (iAttributes.iType == EMemoryEatTypeMemoryToBeLeft)
        {
        _LIT(KMemoryEatEntryMemoryToBeLeft, "[%d] MemEat src=%S prio=%S type=MemToBeLeft amount=%LDB buf=%dB idle=%dms");
        
        buf.Format(KMemoryEatEntryMemoryToBeLeft, iAttributes.iId, &srcBuf, &prioBuf, iAttributes.iAmount, iAttributes.iBuffer, iAttributes.iIdle);
        }        

    else if (iAttributes.iType == EMemoryEatTypeWavy)
        {
        _LIT(KMemoryEatEntryRandom, "[%d] MemEat src=%S prio=%S type=Random min=%LDB max=%LDB buf=%dB idle=%dms");
        
        buf.Format(KMemoryEatEntryRandom, iAttributes.iId, &srcBuf, &prioBuf, iAttributes.iRandomMin, iAttributes.iRandomMax, iAttributes.iBuffer, iAttributes.iIdle);
        } 
            
    return TPtrC(buf);
    }               

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CMemoryEatManager* CMemoryEatManager::NewL(TMemoryEatAttributes& aAttributes)
    {
    CMemoryEatManager* self = new(ELeave) CMemoryEatManager(aAttributes);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CMemoryEatManager::CMemoryEatManager(TMemoryEatAttributes& aAttributes) :
    CActive(EPriorityStandard), iAttributes(aAttributes)
    {
    }

// --------------------------------------------------------------------------------------------
    
CMemoryEatManager::~CMemoryEatManager()
    {
    Cancel();
    DestroyMemoryEat();
    }

// --------------------------------------------------------------------------------------------
 
void CMemoryEatManager::ConstructL()
    {
    CActiveScheduler::Add(this);
    
    // set the status as pending
    iStatus = KRequestPending;
    SetActive();
    
    // set the death status pointer point to the request status of this ao
    iAttributes.iDeathStatus = &iStatus;
    
    // init
    InitMemoryEatL();
    
    // start timer    
    iPeriodicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iPeriodicTimer->Start(KDefaultStart, KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }
    
// --------------------------------------------------------------------------------------------
 
void CMemoryEatManager::RunL()
    {
    // request status has completed by the main thread meaning that we need to stop now
    CActiveScheduler::Stop();
    }

// --------------------------------------------------------------------------------------------
 
void CMemoryEatManager::DoCancel()
    {
    }

// --------------------------------------------------------------------------------------------

TInt CMemoryEatManager::PeriodicTimerCallBack(TAny* aAny)
    {
    TInt result = KErrNone;
    CMemoryEatManager* self = static_cast<CMemoryEatManager*>( aAny );

    self->iPeriodicTimer->Cancel();
    TRAP( result, self->EatMemoryL() );

    return result;
    }
    
// --------------------------------------------------------------------------------------------
 
void CMemoryEatManager::InitMemoryEatL()
    {
    // create a chunk for RAM
    if (iAttributes.iSource == EMemoryEatSourceTypeRAM)
        {
        TBuf<64> chunkName;
        chunkName.Format(KChunkName, iAttributes.iId);
        
        // set max size of the chunk to be size of the RAM memory
        TMemoryInfoV1Buf ramMemory;
        UserHal::MemoryInfo(ramMemory);
    
        User::LeaveIfError( iEatChunk.CreateGlobal(chunkName, 0, ramMemory().iMaxFreeRamInBytes-1, EOwnerThread) );
        }
    
    // create a temporary file to disk
    else
        {
        // connect to RFs
        User::LeaveIfError( iFs.Connect() );
        
        iFilesCounter = 0;
        
        TFileName eatFileName;
        eatFileName.Format( KFilePath, 'B'+iAttributes.iSource, iAttributes.iId, 1 );

        iFs.MkDirAll( eatFileName );
        User::LeaveIfError( iEatFile.Replace( iFs, eatFileName, EFileWrite ) );
        iFilesCounter++;        
        } 
    
    iWavyEatMoreMemory = ETrue;
    }

// --------------------------------------------------------------------------------------------

void CMemoryEatManager::DestroyMemoryEat()
    {
    if (iAttributes.iSource == EMemoryEatSourceTypeRAM)
        {
        iEatChunk.Adjust(0);
        iEatChunk.Close();
        }
    else
        {
        // close current file
        
        iEatFile.Flush();
        iEatFile.Close();

        // delete created files
        for ( TInt k = 1; k <= iFilesCounter; k++ )
            {            
            TFileName eatFileName;
            eatFileName.Format( KFilePath, 'B'+iAttributes.iSource, iAttributes.iId, k );
            iFs.Delete( eatFileName );
            }
        iFs.Close();
        } 
    }
    
// --------------------------------------------------------------------------------------------
 
void CMemoryEatManager::EatMemoryL()
    {
    // get current sizes
    TInt64 eatSizeNow(0);
    TInt sizeNow(0);
    TInt64 freeSystemMemory(0);

    if (iAttributes.iSource == EMemoryEatSourceTypeRAM)
        {
        eatSizeNow = iEatChunk.Size(); // get current chunk size
        }
    else
        {
        User::LeaveIfError( iEatFile.Size( sizeNow ) );
        if ( iFilesCounter > 1)
            {
            eatSizeNow += sizeNow + ( ( iFilesCounter - 1 ) * KMaxEatSize );
        	}
        else
            { 
            eatSizeNow = sizeNow ; 
            }
        }
    LOGSTRING2("Loadgen:EatMemoryL eatSizeNow = %d", eatSizeNow );
    
    freeSystemMemory = ReadFreeMemory();
    
    // eat until specific amount of memory at most amount of buffer at a time  
    if (iAttributes.iType == EMemoryEatTypeMemoryToEat)
        {
        // grow partial buffer to match exact amount 
        if ( ( iAttributes.iAmount - eatSizeNow >= 0 && 
               iAttributes.iAmount - eatSizeNow < iAttributes.iBuffer ) )
            {
               DoEatMemoryL( eatSizeNow + ( iAttributes.iAmount - eatSizeNow ) );
            }
        
        // grow full buffer
        else if ( eatSizeNow < iAttributes.iAmount )
            {
            DoEatMemoryL( eatSizeNow + iAttributes.iBuffer );
            }
        // shrink partial buffer to match exact amount
        else if (eatSizeNow - iAttributes.iAmount >= 0 && eatSizeNow - iAttributes.iAmount < iAttributes.iBuffer)
            {
            DoEatMemoryL(eatSizeNow - (eatSizeNow - iAttributes.iAmount));
            }                
        
        // shrink full buffer
        else if (eatSizeNow > iAttributes.iAmount)
            {
            DoEatMemoryL(eatSizeNow - iAttributes.iBuffer);
            }                
        }

    
    // eat until specific amount of free memory has been reached at most amount of buffer at a time
    else if (iAttributes.iType == EMemoryEatTypeMemoryToBeLeft)
        {
        // grow partial buffer to match exact amount
        if (freeSystemMemory - iAttributes.iAmount >= 0 && freeSystemMemory - iAttributes.iAmount < iAttributes.iBuffer)
            {
            DoEatMemoryL(eatSizeNow + (freeSystemMemory - iAttributes.iAmount));
            }

        // grow full buffer
        else if (freeSystemMemory > iAttributes.iAmount)
            {
            DoEatMemoryL(eatSizeNow + iAttributes.iBuffer);
            }

        // shrink partial buffer to match exact amount
        if (iAttributes.iAmount - freeSystemMemory >= 0 && iAttributes.iAmount - freeSystemMemory < iAttributes.iBuffer)
            {
            DoEatMemoryL(eatSizeNow - (iAttributes.iAmount - freeSystemMemory));
            }

        // shrink full buffer
        else if (freeSystemMemory < iAttributes.iAmount)
            {
            DoEatMemoryL(eatSizeNow - iAttributes.iBuffer);
            }
        }        
        
    // wavy memory eat
    else if (iAttributes.iType == EMemoryEatTypeWavy)
        {
        // grow to max value if not under it
        if (freeSystemMemory > iAttributes.iRandomMax)
            {
            DoEatMemoryL(eatSizeNow + (freeSystemMemory - iAttributes.iRandomMax) + iAttributes.iBuffer);
            iWavyEatMoreMemory = ETrue; // change direction
            }
        
        // shrink to min value if not below it
        else if (freeSystemMemory < iAttributes.iRandomMin)
            {
            DoEatMemoryL(eatSizeNow - (iAttributes.iRandomMin - freeSystemMemory) - iAttributes.iBuffer);
            iWavyEatMoreMemory = EFalse; // change direction
            }                
        
        // grow or shrink
        else
            {
            if (iWavyEatMoreMemory)
                {
                // grow partial buffer to match exact amount
                if (freeSystemMemory - iAttributes.iRandomMin >= 0 && freeSystemMemory - iAttributes.iRandomMin < iAttributes.iBuffer)
                    {
                    DoEatMemoryL(eatSizeNow + (freeSystemMemory - iAttributes.iRandomMin));
                    iWavyEatMoreMemory = EFalse; // change direction since limit reached
                    }
                
                // grow full buffer
                else
                    {
                    DoEatMemoryL(eatSizeNow + iAttributes.iBuffer);
                    }
                }

            else // shrink
                {
                // shrink partial buffer to match exact amount
                if (iAttributes.iRandomMax - freeSystemMemory >= 0 && iAttributes.iRandomMax - freeSystemMemory < iAttributes.iBuffer)
                    {
                    DoEatMemoryL(eatSizeNow - (iAttributes.iRandomMax - freeSystemMemory));
                    iWavyEatMoreMemory = ETrue; // change direction since limit reached
                    }                
                
                // shrink full buffer
                else
                    {
                    DoEatMemoryL(eatSizeNow - iAttributes.iBuffer);
                    }                 
                }
            }
        }
        

    // call timer
    iPeriodicTimer->Start(CLoadGenUtils::MilliSecondsToMicroSeconds(iAttributes.iIdle, iAttributes.iRandomVariance), KDefaultPeriod, TCallBack(PeriodicTimerCallBack, this));
    }
    
// --------------------------------------------------------------------------------------------
 
void CMemoryEatManager::DoEatMemoryL( TInt64 aNewSize )
    {
    // check that new size is in valid range
    if (aNewSize < 0)
        {
        aNewSize = 0;
        }
    LOGSTRING2("Loadgen::DoEatMemoryL aNewSize = %ld", aNewSize );
    // set new size
    if (iAttributes.iSource == EMemoryEatSourceTypeRAM)
        {
        TInt err = iEatChunk.Adjust( aNewSize );
        }
    else
        {
        HandleEatMemoryL( aNewSize );
        }
    }

// --------------------------------------------------------------------------------------------

void CMemoryEatManager::HandleEatMemoryL( TInt64 aNewSize )
    {
    // current size = open file size + full files sizes
    TInt currentFileSize = 0;        
    User::LeaveIfError( iEatFile.Size( currentFileSize ) );    
    TInt64 currentSize = currentFileSize + ( ( iFilesCounter - 1 ) * KMaxEatSize );
    
    if ( aNewSize > currentSize ) // increasing
        {
        AllocMemoryL( aNewSize );
        }
    else if ( aNewSize < currentSize )//decreasing
        {
        FreeMemoryL( aNewSize );
        }
    }

// --------------------------------------------------------------------------------------------
void CMemoryEatManager::AllocMemoryL( TInt64 aNewSize )
    {
    LOGSTRING("Loadgen: CMemoryEatManager::AllocMemoryL =>");
    // size of the file that remains open
    TInt64 sizeLeft = aNewSize % KMaxEatSize;
    // amount of the files that is needed
    TInt64 filesNeeded = aNewSize / KMaxEatSize; 
    if ( sizeLeft > 0 )
        {
        filesNeeded++;
        }

    if ( filesNeeded > iFilesCounter )
        {
        // update current file size to 2 GB
        User::LeaveIfError( iEatFile.SetSize( KMaxEatSize ) );
        // close current file
        iEatFile.Flush();
        iEatFile.Close();
        
        // open new file
        TFileName eatFileName;
        eatFileName.Format( KFilePath, 'B'+iAttributes.iSource, iAttributes.iId, ++iFilesCounter );
        
        iFs.MkDirAll( eatFileName );
        User::LeaveIfError( iEatFile.Replace( iFs, eatFileName, EFileWrite ) );
        
        }
    User::LeaveIfError( iEatFile.SetSize( sizeLeft ) );
    LOGSTRING("Loadgen: CMemoryEatManager::AllocMemoryL <=");
    }

// --------------------------------------------------------------------------------------------
void CMemoryEatManager::FreeMemoryL( TInt64 aNewSize )    
    {
    LOGSTRING("Loadgen: CMemoryEatManager::FreeMemoryL =>");
    // size of the file that remains open
    TInt sizeLeft = aNewSize % KMaxEatSize;
        
    LOGSTRING2("Loadgen: sizeLeft = %d ",  sizeLeft );
    // amount of the files that is needed
    TInt filesNeeded = aNewSize / KMaxEatSize; 
  
    if ( filesNeeded == 0 )
            {
            filesNeeded = 1;
            }

    if ( iFilesCounter > 1 && filesNeeded < iFilesCounter )
        {
        TFileName eatFileName;

        // close current open file 
        iEatFile.Flush();
        iEatFile.Close();
        // remove file
        eatFileName.Format( KFilePath, 'B'+iAttributes.iSource, iAttributes.iId, iFilesCounter-- );
        iFs.Delete( eatFileName );
        
        // remove files not needed
        while ( iFilesCounter > filesNeeded )
            {
            eatFileName.Format( KFilePath, 'B'+iAttributes.iSource, iAttributes.iId, iFilesCounter-- );
            iFs.Delete( eatFileName );
            }

        // open new file
        eatFileName.Format( KFilePath, 'B'+iAttributes.iSource, iAttributes.iId, iFilesCounter );
        User::LeaveIfError( iEatFile.Open( iFs, eatFileName, EFileWrite ) );
        }
    User::LeaveIfError( iEatFile.SetSize( sizeLeft ) );
    LOGSTRING("Loadgen: CMemoryEatManager::FreeMemoryL <=");
    }

// --------------------------------------------------------------------------------------------
TInt64 CMemoryEatManager::ReadFreeMemory()    
    {
    if ( iAttributes.iSource == EMemoryEatSourceTypeRAM )
        {   
        TMemoryInfoV1Buf ramMemory;
        UserHal::MemoryInfo( ramMemory ); // get available RAM
        return (TInt64)ramMemory().iFreeRamInBytes;
        }
    else
        {   
        TVolumeInfo volumeInfo;
        iFs.Volume( volumeInfo, iAttributes.iSource + 1 ); // get available disk      
        return volumeInfo.iFree;
        }
    }
// --------------------------------------------------------------------------------------------

// End of File
