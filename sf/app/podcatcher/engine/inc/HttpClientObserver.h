/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef _MHTTPCLIENTOBSERVER_H_
#define _MHTTPCLIENTOBSERVER_H_

#include <e32cmn.h>

class CHttpClient;

class MHttpClientObserver
{
public:
  virtual void Connected(CHttpClient* aHttpClient) = 0;
  virtual void Disconnected(CHttpClient* aHttpClient) = 0;
  virtual void CompleteL(CHttpClient* aHttpClient, TInt aError) = 0;
  virtual void Progress(CHttpClient* aHttpClient, TInt aBytes, TInt aTotalBytes) = 0;
  virtual void DownloadInfo(CHttpClient* aHttpClient, TInt aTotalBytes) = 0;
  virtual void FileError(TUint aError) = 0;
};

#endif
