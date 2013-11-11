/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline methods for TVtEngDtmfTone
*
*/



inline TVtEngDtmfTone::TVtEngDtmfTone( const TChar& aTone )
: TVtEngtType( EVtEngTypeDtmf ) { iTone = aTone; }

// End of File