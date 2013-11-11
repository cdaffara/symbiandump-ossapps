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

#include "MemSpyUiUtils.h"

// System includes
#include <coemain.h>
#include <memspyui.rsg>




void MemSpyUiUtils::Format( TDes& aBuf, TInt aResourceId, ...)
    {
	VA_LIST list;
    VA_START(list,aResourceId);
	TBuf<128> format;
	CCoeEnv::Static()->ReadResource( format, aResourceId );
	aBuf.FormatList( format, list );
    }


void MemSpyUiUtils::GetErrorText( TDes& aBuf, TInt aError )
    {
    if  ( aError == KErrNotSupported )
        {
        _LIT( KMemSpyErrorText, "Not Supported" );
        aBuf = KMemSpyErrorText;
        }
    else if ( aError == KErrNotReady )
        {
        _LIT( KMemSpyErrorText, "Not Ready" );
        aBuf = KMemSpyErrorText;
        }
    else if ( aError == KErrNotFound )
        {
        _LIT( KMemSpyErrorText, "Missing" );
        aBuf = KMemSpyErrorText;
        }
    else if ( aError == KErrGeneral )
        {
        _LIT( KMemSpyErrorText, "General Error" );
        aBuf = KMemSpyErrorText;
        }
    else
        {
        _LIT( KMemSpyItemValueError, "Error: %d" );
        aBuf.Format( KMemSpyItemValueError, aError );
        }
    }

HBufC* MemSpyUiUtils::FormatItemLC( const TDesC& aCaption )
	{
	HBufC* retBuf = HBufC::NewLC( 32 );
	TPtr pRetBuf( retBuf->Des() );
	pRetBuf.Zero();
	pRetBuf.Append( _L("\t") );
	pRetBuf.Append( aCaption );
	return retBuf;
	}

TDesC& MemSpyUiUtils::ThreadInfoItemNameByType( TMemSpyThreadInfoItemType aType )
	{	
	TPtrC pType( KTypeUnknown );
	
	switch( aType )
		{
		case EMemSpyThreadInfoItemTypeFirst:			
			pType.Set(KGeneral);
			break;			
		case EMemSpyThreadInfoItemTypeHeap:			
			pType.Set(KHeap);
			break;				
		case EMemSpyThreadInfoItemTypeStack:			
			pType.Set(KStack);
			break;			
		case EMemSpyThreadInfoItemTypeChunk:			
			pType.Set(KChunks);
			break;
		case EMemSpyThreadInfoItemTypeCodeSeg:
			pType.Set(KCodeSegs);
			break;
		case EMemSpyThreadInfoItemTypeOpenFiles:
			pType.Set(KOpenFiles);
			break;	
		case EMemSpyThreadInfoItemTypeActiveObject:
			pType.Set(KActiveObjects);
			break;
		case EMemSpyThreadInfoItemTypeOwnedThreadHandles:
			pType.Set(KThreadHandlers);
			break;
		case EMemSpyThreadInfoItemTypeOwnedProcessHandles:
			pType.Set(KProcessHandlers);
			break;
		case EMemSpyThreadInfoItemTypeServer:
			pType.Set(KServers);
			break;
		case EMemSpyThreadInfoItemTypeSession:
			pType.Set(KConnections);
			break;
		case EMemSpyThreadInfoItemTypeSemaphore:
			pType.Set(KSemaphores);
			break;
		case EMemSpyThreadInfoItemTypeOtherThreads:
			pType.Set(KThreadReferences);
			break;
		case EMemSpyThreadInfoItemTypeOtherProcesses:
			pType.Set(KProcessReferences);
			break;
		case EMemSpyThreadInfoItemTypeMutex:
			pType.Set(KMutexes);
			break;
		case EMemSpyThreadInfoItemTypeTimer:
			pType.Set(KTimers);					
			break;
		case EMemSpyThreadInfoItemTypeLogicalChannel:
			pType.Set(KDD);
			break;
		case EMemSpyThreadInfoItemTypeChangeNotifier:
			pType.Set(KChangeNotif);
			break;
		case EMemSpyThreadInfoItemTypeUndertaker:
			pType.Set(KUndertakers);
			break;				
		case EMemSpyThreadInfoItemTypeLDD:
			pType.Set(KLogicalDrivers);
			break;
		case EMemSpyThreadInfoItemTypePDD:
			pType.Set(KPhysicalDrivers);
			break;
			
	    default:	       
	    	break;  
		}
	return pType;
	}


void MemSpyUiUtils::AppendPriority( TDes& aDes, TProcessPriority aPriority )
	{
	switch( aPriority )
		{
	    case EPriorityLow:
	        aDes += _L("[L]");
	        break;
	    case EPriorityBackground:
	        aDes += _L("[B]");
	        break;
	    case EPriorityForeground:
	        aDes += _L("[F]");
	        break;
	    case EPriorityHigh:
	        aDes += _L("[H]");
	        break;
	    case EPriorityWindowServer:
	        aDes += _L("[WS]");
	        break;
	    case EPriorityFileServer:
	        aDes += _L("[FS]");
	        break;
	    case EPriorityRealTimeServer:
	        aDes += _L("[RTS]");
	        break;
	    case EPrioritySupervisor:
	        aDes += _L("[SUP]");
	        break;
	    default:
	        aDes += _L("[?]");
	        break;
		}
	}

void MemSpyUiUtils::AppendExitInfo( TDes& aDes, TExitType aType, TInt aExitReason, const TDesC& aExitCategory  )
	{
    aDes.Append( '[' );
    const TInt length = aDes.Length();
    AppendExitType( aDes, aType );
    aDes.SetLength( length + 1 ); // Remove all but the first letter
    aDes.Append( ']' );
    
    if  ( aType == EExitKill || aType == EExitPending )
        {
        // Kill implies "clean" exit. Pending implies not yet dead.
        }
    else
        {
        TMemSpyTruncateOverflow overflow;

        // Terminate or Panic implies abnormal exit condition, so
        // show full exit info.
        _LIT( KAbnormalFormatSpec, " %S-%d" );
        aDes.AppendFormat( KAbnormalFormatSpec, &overflow, &aExitCategory, aExitReason );
        }
	}


void MemSpyUiUtils::AppendExitType( TDes& aDes, TExitType aType )
    {
    _LIT( KExitTypeKilled, "Killed" );
    _LIT( KExitTypeTerminated, "Terminated" );
    _LIT( KExitTypePanicked, "Panicked" );
    _LIT( KExitTypePending, "Pending" );
    
    // Panic and Terminate are exceptional exit conditions.
    // Kill, is ironically, not an exceptional condition.
    switch( aType )
        {
    case EExitKill:
        aDes += KExitTypeKilled;
        break;
    case EExitTerminate:
        aDes += KExitTypeTerminated;
        break;
    case EExitPanic:
        aDes += KExitTypePanicked;
        break;
    default:
    case EExitPending:
        aDes += KExitTypePending;
        break;
        }
    }


TPtrC MemSpyUiUtils::TypeAsString( TMemSpyDriverContainerType aType )
    {     
    TPtrC pType( KTypeUnknown );
    //
    switch( aType )
        {
    case EMemSpyDriverContainerTypeThread:
        pType.Set( KTypeThread );
        break;
    case EMemSpyDriverContainerTypeProcess:
        pType.Set( KTypeProcess );
        break;
    case EMemSpyDriverContainerTypeChunk:
        pType.Set( KTypeChunk );
        break;
    case EMemSpyDriverContainerTypeLibrary:
        pType.Set( KTypeLibrary );
        break;
    case EMemSpyDriverContainerTypeSemaphore:
        pType.Set( KTypeSemaphore );
        break;
    case EMemSpyDriverContainerTypeMutex:
        pType.Set( KTypeMutex );
        break;
    case EMemSpyDriverContainerTypeTimer:
        pType.Set( KTypeTimer );
        break;
    case EMemSpyDriverContainerTypeServer:
        pType.Set( KTypeServer );
        break;
    case EMemSpyDriverContainerTypeSession:
        pType.Set( KTypeSession );
        break;
    case EMemSpyDriverContainerTypeLogicalDevice:
        pType.Set( KTypeLogicalDevice );
        break;
    case EMemSpyDriverContainerTypePhysicalDevice:
        pType.Set( KTypePhysicalDevice );
        break;
    case EMemSpyDriverContainerTypeLogicalChannel:
        pType.Set( KTypeLogicalChannel );
        break;
    case EMemSpyDriverContainerTypeChangeNotifier:
        pType.Set( KTypeChangeNotifier );
        break;
    case EMemSpyDriverContainerTypeUndertaker:
        pType.Set( KTypeUndertaker );
        break;
    case EMemSpyDriverContainerTypeMsgQueue:
        pType.Set( KTypeMsgQueue );
        break;
    case EMemSpyDriverContainerTypePropertyRef:
        pType.Set( KTypePropertyRef );
        break;
    case EMemSpyDriverContainerTypeCondVar:
        pType.Set( KTypeCondVar );
        break;
        
    default:
        break;
        }
    //
    return pType;
    }

TMemSpySizeText MemSpyUiUtils::FormatSizeText( const TInt64& aValue, TInt aDecimalPlaces, TBool aExtraRounding )
    {
    _LIT(KFormatKilo, "%dK");
    _LIT(KFormatMega, "%SM");
    _LIT(KFormatGiga, "%SG");

	TMemSpySizeText buf;
	if  ( aValue < 1024000 )					// If < 1000K
		{
		TInt sizeInK = 0;

        if  ( aValue != 0 )
			{
			sizeInK = I64INT( (aValue + 512) >> 10 );
			if  (sizeInK < 1)
                {
				sizeInK = 1;
                }
			if  (sizeInK > 999)
                {
				sizeInK = 999;
                }
			}

        buf.Format( KFormatKilo, sizeInK );
		}
	else
		{
		TReal sizeInM = I64INT( aValue );
		sizeInM /= 1048576;
		if  ( sizeInM < 1 )
            {
			sizeInM = 1;
            }

        TPtrC pFormat( KFormatMega );
		if  ( sizeInM >= 1000 )
			{
			sizeInM /= 1024;				// Size in G
			if  (sizeInM < 1)
                {
				sizeInM = 1;
                }
			
            pFormat.Set( KFormatGiga );
			}

        if  ( sizeInM > 999.9)
            {
            sizeInM = 999.9;
            }

        if  ( aExtraRounding )
            {
			sizeInM += 0.499999;
            }

		TBuf<16> size;
		size.Num( sizeInM, TRealFormat( 14, aDecimalPlaces ) );	// Allow for "17179869184.0"G which is 2^64
        buf.Format( pFormat, &size );
		}

    return buf;
    }
