/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32cmn.h>

#include "tmessagewrp2.h"
#include "clog.h"

TMessageWrp2::TMessageWrp2( const RMessage2& aMessage, 
							const TBool& aConnected ) :
							iConnected( aConnected ),
							iDisposable( EFalse )
																		
    {
    iMessage = aMessage;
    }

void TMessageWrp2::WriteL(TInt aParam, const TDesC8 &aDes, TInt aOffset) const
	{
	LOG("TMessageWrp2::WriteL begin");
	LOG1("TMessageWrp2::WriteL iConnected: %d", iConnected);
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TMessageWrp2::WriteL iMessage.Function(): %d", iMessage.Function());
		iMessage.WriteL( aParam, aDes, aOffset );	
		}
	LOG("TMessageWrp2::WriteL end");
	}
	
void TMessageWrp2::Complete(TInt aReason) const
	{
	LOG("TMessageWrp2::Complete begin");
	LOG1("TMessageWrp2::Complete iConnected: %d", iConnected);
	if( !iMessage.IsNull() && iConnected ) 
		{
		LOG1("TMessageWrp2::Complete iMessage.Function(): %d", iMessage.Function());
		iMessage.Complete( aReason );
		}
	LOG("TMessageWrp2::Complete end");
	}
	
TInt TMessageWrp2::Int0() const
	{
	LOG("TMessageWrp2::Int0 begin");
	LOG1("TMessageWrp2::Int0 iConnected: %d", iConnected);
	TInt value( 0 );
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TMessageWrp2::Int0 iMessage.Function(): %d", iMessage.Function());
		value = iMessage.Int0();
		}	
	LOG("TMessageWrp2::Int0 end");
	return value;
	}
	
TInt TMessageWrp2::Int1() const
	{
	LOG("TMessageWrp2::Int1 begin");
	LOG1("TMessageWrp2::Int1 iConnected: %d", iConnected);
	TInt value( 0 );
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TMessageWrp2::Int1 iMessage.Function(): %d", iMessage.Function());
		value = iMessage.Int1();
		}	
	LOG("TMessageWrp2::Int1 end");
	return value;
	}
	
TInt TMessageWrp2::GetDesLength(TInt aParam) const
	{
	LOG("TMessageWrp2::GetDesLength begin");
	LOG1("TMessageWrp2::GetDesLength iConnected: %d", iConnected);	
	TInt value( 0 );
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TMessageWrp2::GetDesLength iMessage.Function(): %d", iMessage.Function());
		value = iMessage.GetDesLength( aParam );
		}	
	LOG("TMessageWrp2::GetDesLength end");
	return value;
	}
	
void TMessageWrp2::ReadL(TInt aParam, TDes8 &aDes, TInt aOffset) const
	{
	LOG("TMessageWrp2::ReadL TDes8 begin");
	LOG1("TMessageWrp2::ReadL TDes8 iConnected: %d", iConnected);
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TMessageWrp2::ReadL TDes8 iMessage.Function(): %d", iMessage.Function());
		iMessage.ReadL( aParam, aDes, aOffset );
		}
	LOG("TMessageWrp2::ReadL TDes8 end");
	}
	
void TMessageWrp2::ReadL(TInt aParam, TDes16 &aDes, TInt aOffset) const
	{
	LOG("TMessageWrp2::ReadL TDes16 begin");
	LOG1("TMessageWrp2::ReadL TDes16 iConnected: %d", iConnected);
	if( !iMessage.IsNull() && iConnected )
		{
		LOG1("TMessageWrp2::ReadL TDes16 iMessage.Function(): %d", iMessage.Function());
		iMessage.ReadL( aParam, aDes, aOffset );
		}
	LOG("TMessageWrp2::ReadL TDes16 end");
	}
	
TInt TMessageWrp2::Function() const
	{
	return iMessage.Function();
	}
	
void TMessageWrp2::SetDisposable( TBool aDisposable )
	{
	iDisposable = aDisposable;
	}
	
TBool TMessageWrp2::Disposable() const
	{
	return iDisposable;
	}		
	
void TMessageWrp2::Panic(const TDesC &aCategory, TInt aReason) const
	{
	iMessage.Panic( aCategory, aReason );
	}

//  End of File
