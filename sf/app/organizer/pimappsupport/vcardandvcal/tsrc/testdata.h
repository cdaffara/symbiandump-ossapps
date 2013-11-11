// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Versit test vCard module
// 
//

// INPUT 1.
// vcard with binary data - but no agents
_LIT8(vcardbinary,"BEGIN:VCARD\r\n"
"VERSION:2.1\r\n"
"REV:20020913T091955Z\r\n"
"UID:02e6f080309ad3ac-00e08837032e0b78-2\r\n"
"N:example;a;;;\r\n"
"NOTE:Test vCard for Versit work\r\n"
"PHOTO;ENCODING=BASE64:/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkS\r\n"
"Ew8UHRofHh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJ\r\n"
"CQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIy\r\n"
"MjIyMjIyMjIyMjIyMjL/wAARCACbANADAREAAhEBAxEB/8QAHwAAAQUBAQEBAQEA\r\n"
"AAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIh\r\n"
"MUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6\r\n"
"Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZ\r\n"
"mqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx\r\n"
"8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREA\r\n"
"AgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAV\r\n"
"YnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hp\r\n"
"anN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPE\r\n"
"xcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwDh\r\n"
"5dJQ8bF3DpxW7prqY8xWOnTxHLzDHfHAFQ4NbDTXUmtbV2P7kq3PzE9hRyPqFyxc\r\n"
"WUMav5ki+oHSq9mluLmMmWwspRudSzkfwkmpcU+o09CpF4aW8UvH5iYPRxU8l9mV\r\n"
"cqt4duVfAQk9uKl02Mh/sS43DDgsc5ABGKXI1uFhtxo97DEHLAqxwAOpNLlt0AZH\r\n"
"p97KnlbG3HgbuKlK70RViR9FurRQZFf6LyKtwl1QioI5rUlgwViODUBYJNQuGUKz\r\n"
"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\r\n"
"gP/Z\r\n\r\n"
"END:VCARD\r\n");

// INPUT 2
// vcard with binary data and agents
_LIT8(vcardagent,"BEGIN:VCARD\r\n"
"VERSION:2.1\r\n"
"AAA.ADR:;;AThe Street;London;;;UK\r\n"
"AAA.DDD.EE.FFFF.GHIJKL.TITLE:AMonster\r\n"
"AAA.N:AX;Example\r\n"
"BBBB.N:BX;Example\r\n"
"BBBB.TITLE:BMonster\r\n"
"BBBB.ADR:;;BThe Street;London;;;UK\r\n"
"CC.ADR:;;CThe Street;London;;;UK\r\n"
"CC.TITLE:CMonster\r\n"
"N:X;Example\r\n"
"ADR:;;The Street;London;;;UK\r\n"
"TITLE;X-DEF=abc;def;X-DEFa=ghi;X-DEFb=jkl;mno:Monster\r\n"
"TEL;PREF;WORK;VOICE:1234567890\r\n"
"TEL;HOME;VOICE:1234567890\r\n"
"LOGO;GIF;ENCODING=BASE64:R0lGODlhogBNAPEAAP//+AgP8AAAAAACH5BAEAAAAALAAAAACiAE0AAAL/BISp\r\n"
    "y+1iopy02ouz3rzTB4aMR5bmiXriCqbuC8cVSy/yjeddzev+/+PRgMRiTLgyKpck\r\n"
    "ZIgJjWKcD6n1eqCOsFyo1tYNG7+J0+B8FqszZEgJDV/LP18T/D7PG+qku18/p2Xn\r\n"
    "FwcoRzVImGa45mSmuMiohvQIKXlYk2J52TjkQsgZyAKDF4opYpp6kqTauvPkGrsB\r\n"
    "K1tr0WKbO+Og2yvB6xucBfYT+TZQobihHLOJspXz11coAWlMYY2smX2toiCTrRFe\r\n"
    "za1Nzl1ZfhKgEPBZbo6tbgB/XR+/fI/fEcCO0L8N3QV49PRF0LdvIEIT/fw1DDjO\r\n"
    "AkGEaSjmK3UOo4eG/+w4NmMmbt7BehMiKtRYktpGjiw/guIgMGXMjC8lSjuZUAPL\r\n"
    "naRAhvQpzxrOm0FVniyxk+cLZz+J2gQqs2ZUo0+RJm35TmoGpk+dJoNa0OvXnBeu\r\n"
    "KoWIEgPXsWKnUgUAdiwJs2fTpR361i2apnfjBl1J1yPavGzv6u1WtK/WG4HrHluM\r\n"
    "d+9WvyMXr43R2PE0yF0N0yRc2etlGJk1dxhdGHTYtp8lt0b8onRSu6pfkw3tGTdV\r\n"
    "1Chkz07EOnFt3qvTEmfo2zTf2rpdR4ZtW7hz0smVT+acenp26M27CZVR/beH491v\r\n"
    "Fx8O9Tv18FjHUz5sHu57+ZbJc2AvHub88su1S//3bp9O+FnXGXPn+Qdff4ipl8KA\r\n"
    "+SkYH3/XBXfgggGW5SCB/0VYYXzHXWYSchm2B+Fp830YV4iAjSjYRRRGN2FuHRb4\r\n"
    "ogUsPvgch/S9iCJ2MyVDwY040shdgjGqxiB8iu0j5JBKRqiibdxF2aFIETTppEVS\r\n"
    "bqLllrtRxCSWpnVZJZcLPZnXmRKIOSSZO5L0JkFoIhjnhcLokeSd8ugJVy/m6AgF\r\n"
    "oBJt5YOgyxykRjyGvlEoEIs2Co42Ff3pmmSTyofpOat9lak9kXyq1qeWImMMNYVc\r\n"
    "GsciqkpKKqsljUQPogXNmimtrnYaqq2yghorp32KqmufsQJbq7C9GktpsMeL/imr\r\n"
    "scLiw6yzkqrVbELROlvNq9kie2y3z2q7bLXijsvted5GK81t11JaabPgnhtuWPBi\r\n"
    "qyi5625bbr74xmsttfzie+279MabLbTzEnxvtwkjiy64t3470LirkpsSObCWyyyx\r\n"
    "Bku8MLv00drpqB6HXKq1E5OM6V4InlrpxKa2DG2ryU56a5F8ykIACgA7\r\n\r\n"

"AGENT:BEGIN:VCARD\r\n"
"VERSION:2.1\r\n"
"N:Example,AN\r\n"
"ADR:;;10 Example Street;London;;;UK\r\n"
"TITLE:Sir\r\n"
"TEL:007\r\n"
"END:VCARD\r\n"
"AGENT:BEGIN:VCARD\r\n"
"VERSION:2.1\r\n"
"N:Example;;A\r\n"
"ADR:;;Space City;London;;;UK\r\n"
"TITLE:StillAlive\r\n"
"TEL:009\r\n"
"END:VCARD\r\n"
"END:VCARD\r\n");

// INPUT 3
// vcard with binary data field which is empty
_LIT8(vcardbinaryempty,"BEGIN:VCARD\r\n"
"VERSION:2.1\r\n"
"REV:20020913T091955Z\r\n"
"UID:02e6f080309ad3ac-00e08837032e0b78-2\r\n"
"N:example;a;;;\r\n"
"NOTE:Test vCard for Versit work\r\n"
"PHOTO;ENCODING=BASE64:\r\n\r\n"
"END:VCARD\r\n");
