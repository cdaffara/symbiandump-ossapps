@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

copy \epoc32\release\winscw\udeb\ut_tphonecmdparamcallheaderdata.dll \epoc32\release\winscw\udeb\z\sys\bin

\epoc32\release\winscw\udeb\eunitexerunner -dtextshell --ut_tphonecmdparamcallheaderdata

copy \epoc32\winscw\c\shared\Eunit\Logs\Eunit_log.xml .\
