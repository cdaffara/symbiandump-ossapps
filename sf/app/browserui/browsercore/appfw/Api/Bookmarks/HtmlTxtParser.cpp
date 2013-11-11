/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include <QtGui/QApplication>
#include <QString>
#include <QFile>
#include <QTextStream>
 #include <QByteArray>

namespace WRT {
static int parseHtmlFile(QTextStream &out); 
static int parseTxtFile(QTextStream &out);
static QString getLinkTitle(QString line); // for html file parsing
static QString getLinkUrl(QString line);   // for html file parsing
static QString getFolderTitle(QString line); // for html file parsing
static QString getHeaderTitle(QString line); // for html file parsing

static QString getTxtLinkTitle(QString line);
static QString getTxtLinkUrl(QString line);

QString getFolderTitle(QString line)
{
    QString line2, title="";
    int ii = 0;

    ii = line.lastIndexOf("</H3>", -1, Qt::CaseInsensitive);
    if(ii > 3){
      line2 = line.left(ii);
      ii = line2.lastIndexOf('>');
      title = line2.mid(ii+1);
      title.replace("&", "&amp;");    
    }
    return title;
}

QString getHeaderTitle(QString line)
{
    QString line2, title="";
    int ii = 0;

    ii = line.lastIndexOf("</H1>", -1, Qt::CaseInsensitive);
    if(ii > 3){
      line2 = line.left(ii);
      ii = line2.lastIndexOf('>');
      title = line2.mid(ii+1);
      title.replace("&", "&amp;");
    }
    return title;
}

QString getLinkTitle(QString line)
{
    QString line2, title;
    int ii = 0;

    ii = line.lastIndexOf("</A>", -1, Qt::CaseInsensitive);
    if(ii > 3){
       line2 = line.left(ii);
       ii = line2.lastIndexOf('>');
       title = line2.mid(ii+1);
       title.replace("&", "&amp;");;
    }
    return title;
}

QString getLinkUrl(QString line)
{
    QString line2, url="";
    int ii = 0;
    
    ii = line.lastIndexOf("HREF", -1, Qt::CaseInsensitive);
    line2 = line.mid(ii+4);
    ii = line2.indexOf("\"");
    line2 = line2.mid(ii+1);
    ii = line2.indexOf("\"");
    url = line2.mid(0, ii);
    url.replace("&", "&amp;");

    return url;
}

QString getTxtLinkTitle(QString line)
{
    QString line2, title="";
    int start = 0, end = 0;

    start = line.lastIndexOf("<", -1, Qt::CaseInsensitive);
    end = line.lastIndexOf(">", -1, Qt::CaseInsensitive);
    if((start==5) && (end >0)){
       if (start+1 == end)
           return "";

       title = line.mid(start+1, end-start-1);
       title.replace("&", "&amp;");
    }
    return title;
}

QString getTxtLinkUrl(QString line)
{
    QString line2, url="";
    int start = 0, end = 0;

    start = line.lastIndexOf("<", -1, Qt::CaseInsensitive);
    end = line.lastIndexOf(">", -1, Qt::CaseInsensitive);
    if((start==4) && (end >0)){
       if (start+1 == end)
           return "";

       url = line.mid(start+1, end-start-1);
       url.replace("&", "&amp;");
    }

    return url;
}


int parseHtmlFile(QTextStream &out)
{
  QString fileName("iebookmark.html");
  QFile file(fileName);
  bool res = file.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream rs( &file );
  QString line, line2, url, title;
  bool folderStarted = false;

  if(!res)
      return -1;

  out << "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?> \n";
  out << "<!DOCTYPE xbel> \n";
  out << "<xbel version=\"1.0\"> \n";

  while ( !rs.atEnd() ) {
      line = rs.readLine();         // line of text excluding '\n'
      if ((line.contains("href", Qt::CaseInsensitive)) &&
          !(line.contains("place:folder", Qt::CaseInsensitive)) && !(line.contains("place:sort", Qt::CaseInsensitive))  ){ // URL
          url = getLinkUrl(line);           
          title = getLinkTitle(line);
          out << "<bookmark href=\"" << url << "\"> \n";
          out << "<title>" << title << "</title> \n";
          out << "</bookmark>\n";
      }
      else if (line.contains("</H1>", Qt::CaseInsensitive)){ // Header
          folderStarted = true;
          title = getHeaderTitle(line);
          out << "<folder folded=\"yes\"> \n";
          out << "<title>" << title << "</title>\n";
      }
      else if (line.contains("</H3>", Qt::CaseInsensitive)){ // Folder
          folderStarted = true;
          title = getFolderTitle(line);
          out << "<folder folded=\"yes\"> \n";
          out << "<title>" << title << "</title>\n";
      }
      else if (line.contains("</DL>", Qt::CaseInsensitive)){ // End of Folder
          if (folderStarted == true)
            out << "</folder>  \n";
          folderStarted = false;
      }
  }


  out << "</xbel> \n";

  file.close();
  return 0;
}

int parseTxtFile(QTextStream &out)
{
  QString fileName("c:\\data\\bookmarks.txt");
  QFile file(fileName);
  bool res = file.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream rs( &file );
  QString line, line2, url, title;

  if(!res)
      return -1;

  out << "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?> \n"; 
  out << "<!DOCTYPE xbel> \n";
  out << "<xbel version=\"1.0\"> \n";

  while ( !rs.atEnd() ) {
      line = rs.readLine();         // line of text excluding '\n'
      if (line.contains("Name", Qt::CaseInsensitive) && line.indexOf("Name", 0, Qt::CaseInsensitive ) == 0){ // Title
          title = getTxtLinkTitle(line);
      }
      else if (line.contains("url", Qt::CaseInsensitive) && line.indexOf("url", 0, Qt::CaseInsensitive ) == 0){ //URL
          url = getTxtLinkUrl(line);
          if(url =="")
              continue;
          out << "<bookmark href=\"" << url.toUtf8() << "\"> \n";
          if(title.toUtf8() == "")
          	out << "<title>" << "Imported Bookmark"<< "</title> \n";
          else	
          	out << "<title>" << title.toUtf8 ()<< "</title> \n";
          out << "</bookmark>\n";
      }      
  }


  out << "</xbel> \n";

  file.close();
  return 0;
}

int startNativeBookmarksParsing()
{
    QString fileName("c:\\data\\bookmarks.txt.xml");
    QFile outHtmlFile(fileName);
    bool res = outHtmlFile.open(QIODevice::WriteOnly | QIODevice::Text);
    
    if(!res)
      return -1;
    QTextStream out( &outHtmlFile );

    //parseHtmlFile(out);
    parseTxtFile(out);
    outHtmlFile.close();
    return 0;
}
}// namespace WRT

