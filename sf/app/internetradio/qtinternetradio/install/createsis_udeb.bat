@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@echo off
echo step 1 : create cenrep.sis
cd ..\..\internetradio2.0\install
makesis  cenrep.pkg
signsis  cenrep.sis cenrep.sis ..\..\qtinternetradio\install\rd.cer ..\..\qtinternetradio\install\rd-key.pem

echo.
echo.
echo step 2 : create ir_package_deb.sis
makesis ir_package_deb.pkg
signsis ir_package_deb.sis ir_package_deb.sis ..\..\qtinternetradio\install\rd.cer ..\..\qtinternetradio\install\rd-key.pem
move ir_package_deb.sis ..\..\qtinternetradio\install

echo.
echo.
echo step 3 : create package_Inc6.sis
cd ..\..\presetserver\sis\presetserver_default
makesis package_Inc6.pkg package_Inc6.sis
signsis package_Inc6.sis package_Inc6.sis ..\..\..\qtinternetradio\install\rd.cer ..\..\..\qtinternetradio\install\rd-key.pem
move package_Inc6.sis ..\..\..\qtinternetradio\install

echo.
echo.
echo step 4 : create qtinternetradio_udeb.sis
cd ..\..\..\qtinternetradio\install
makesis ir3_qtinternetradio_armv5_udeb.pkg qtinternetradio_udeb.sis
signsis qtinternetradio_udeb.sis qtinternetradio_udeb.sis rd.cer rd-key.pem

echo.

