/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <e32cmn.h>

#include "directprintmessage.h"
#include "clog.h"

TDpMessage::TDpMessage( const RMessage2& aMessage, 
							const TBool& aConnected ) :
							iConnected( aConnected ),
							iDisposable( EFalse )
																		
    {
    iMessage = aMessage;
    }

void TDpMessage::WriteL(TInt aParam, const TDesC8 &aDes, TInt aOffset) const
	{
	LOG("TDpMessage::WriteL begin");
	LOG1("TDpMessage::WriteL iConnected: %d", iConnected);
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TDpMessage::WriteL iMessage.Function(): %d", iMessage.Function());
		iMessage.WriteL( aParam, aDes, aOffset );	
		}
	LOG("TDpMessage::WriteL end");
	}
	
void TDpMessage::Complete(TInt aReason) const
	{
	LOG("TDpMessage::Complete begin");
	LOG1("TDpMessage::Complete iConnected: %d", iConnected);
	if( !iMessage.IsNull() && iConnected ) 
		{
		LOG1("TDpMessage::Complete iMessage.Function(): %d", iMessage.Function());
		iMessage.Complete( aReason );
		}
	LOG("TDpMessage::Complete end");
	}
	
TInt TDpMessage::Int0() const
	{
	LOG("TDpMessage::Int0 begin");
	LOG1("TDpMessage::Int0 iConnected: %d", iConnected);
	TInt value( 0 );
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TDpMessage::Int0 iMessage.Function(): %d", iMessage.Function());
		value = iMessage.Int0();
		}	
	LOG("TDpMessage::Int0 end");
	return value;
	}
	
TInt TDpMessage::Int1() const
	{
	LOG("TDpMessage::Int1 begin");
	LOG1("TDpMessage::Int1 iConnected: %d", iConnected);
	TInt value( 0 );
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TDpMessage::Int1 iMessage.Function(): %d", iMessage.Function());
		value = iMessage.Int1();
		}	
	LOG("TDpMessage::Int1 end");
	return value;
	}
	
TInt TDpMessage::GetDesLength(TInt aParam) const
	{
	LOG("TDpMessage::GetDesLength begin");
	LOG1("TDpMessage::GetDesLength iConnected: %d", iConnected);	
	TInt value( 0 );
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TDpMessage::GetDesLength iMessage.Function(): %d", iMessage.Function());
		value = iMessage.GetDesLength( aParam );
		}	
	LOG("TDpMessage::GetDesLength end");
	return value;
	}
	
void TDpMessage::ReadL(TInt aParam, TDes8 &aDes, TInt aOffset) const
	{
	LOG("TDpMessage::ReadL TDes8 begin");
	LOG1("TDpMessage::ReadL TDes8 iConnected: %d", iConnected);
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TDpMessage::ReadL TDes8 iMessage.Function(): %d", iMessage.Function());
		iMessage.ReadL( aParam, aDes, aOffset );
		}
	LOG("TDpMessage::ReadL TDes8 end");
	}
	
void TDpMessage::ReadL(TInt aParam, TDes16 &aDes, TInt aOffset) const
	{
	LOG("TDpMessage::ReadL TDes16 begin");
	LOG1("TDpMessage::ReadL TDes16 iConnected: %d", iConnected);
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TDpMessage::ReadL TDes16 iMessage.Function(): %d", iMessage.Function());
		iMessage.ReadL( aParam, aDes, aOffset );
		}
	LOG("TDpMessage::ReadL TDes16 end");
	}
	
TInt TDpMessage::Function() const
	{
	return iMessage.Function();
	}
	
void TDpMessage::SetDisposable( TBool aDisposable )
	{
	iDisposable = aDisposable;
	}
	
TBool TDpMessage::Disposable() const
	{
	return iDisposable;
	}		
	
void TDpMessage::Panic(const TDesC &aCategory, TInt aReason) const
	{
	iMessage.Panic( aCategory, aReason );
	}

//  End of File
