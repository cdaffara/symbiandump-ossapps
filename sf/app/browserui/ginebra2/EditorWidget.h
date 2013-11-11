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

#ifndef __EDITOR_WIDGET_H__
#define __EDITOR_WIDGET_H__

#include "ActionButton.h"
#include "NativeChromeItem.h"
#include "TitleItem.h"

#include <QGraphicsTextItem>
#include <QGraphicsWidget>
#include <QtGui>

namespace GVA {

  class ChromeSnippet;
  class ChromeWidget;

  class GTextLineItem : public QGraphicsTextItem
  {
    Q_OBJECT

  public:
    GTextLineItem(QGraphicsItem * parent);
    virtual ~GTextLineItem();

    qreal cursorX();
    qreal anchorX();
    QRectF selectionRectF();
    void setText(const QString & text);
    void setMaxTextLength(int length);
    void selectAll();
    void unselect();
    qreal textWidth();
    void setCursorPosition(int pos);
    bool hasSelection() { return (cursorX()!= anchorX()); }
    Qt::InputMethodHints inputMethodHints() const { return m_hints; }
    void setInputMethodHints(Qt::InputMethodHints hints);
    void setSpecificButton(QString& commitString, QString& buttonIconPath);
    void launchVKB();
    void  sendInputPanelEvent(QEvent::Type type);

  protected:
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void keyReleaseEvent(QKeyEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void focusInEvent(QFocusEvent * event);
    virtual void focusOutEvent(QFocusEvent * event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  private slots:
    void contentsChange(int position, int charsRemoved, int charsAdded);
    void specificBtnTriggered(bool checked);

  signals:
    void cursorXChanged(qreal newx);
    void textMayChanged();
    void focusChanged(bool focusIn);
    void tapped(QPointF& pos);

  private:
    QTextLine m_textLine;
    int m_defaultStartDragDistance;
    int m_maxTextLength;
    Qt::InputMethodHints m_hints;
    bool m_setSpecificBtn;
    QString m_spBtnCommitString;
    QString m_spBtnIconPath;
  };

  class GLineEditor : public QGraphicsWidget
  {
    Q_OBJECT

  public:
    GLineEditor(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent = 0);
    virtual ~GLineEditor();

    void selectAll() { m_editor->selectAll(); }
    void unselect() { m_editor->unselect(); }

    void setTextColor(QColor & color);
	
    void setBackgroundColor(QColor & color) {m_backgroundColor = color;}
    void setPadding(qreal padding);
    void setRightTextMargin(qreal margin);
    QString text() const;
    void updateEditor();
    bool tappedOnText(qreal x) const;
    void grabFocus() { m_editor->setFocus(); }
    void removeFocus() {m_editor->clearFocus(); }
    void setCursorPosition (int pos) { m_editor->setCursorPosition(pos); }
    void shiftToLeftEnd() { setCursorPosition(0); }
    int characterCount() { return m_editor->document()->characterCount(); }
    bool hasSelection() { return m_editor->hasSelection(); }
    Qt::InputMethodHints inputMethodHints() { return m_editor->inputMethodHints(); }
    // Calling this function will overwrite the existing hints
    void setInputMethodHints(Qt::InputMethodHints hints) { m_editor->setInputMethodHints(hints); }
    void setMaxTextLength(int length) { m_editor->setMaxTextLength(length);}
#ifdef BROWSER_LAYOUT_TENONE
    void changeEditorMode(bool edit);
    void setTitle(const QString & text);
    void setTitleColor(QColor & color);
    void setTitleFont(QFont & font);

#endif
    void setSpecificButton(QString commitString, QString buttonIcon) { m_editor->setSpecificButton(commitString, buttonIcon);}
    void closeVKB();
    void openVKB();

  protected:
    virtual bool eventFilter(QObject * object, QEvent * event);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    virtual void resizeEvent(QGraphicsSceneResizeEvent * event);

  signals:
    void activated();
    void textMayChanged();
    void contentsChange(int position, int charsRemoved, int charsAdded);
    void focusChanged(bool focusIn);
    void tapped(QPointF& pos);
    void titleMouseEvent(QPointF& pos);

  private slots:
    void makeVisible(qreal cursorX);

  public slots:
    void setText(const QString & text);

  private:
    void internalScroll(qreal deltaX);

  protected:
    ChromeWidget * m_chrome;

    // Cached values used for painting and scrolling.
    qreal m_viewPortWidth;
#ifdef BROWSER_LAYOUT_TENONE
    qreal m_titleModeWidth;
    GTitleItem * m_title;
    QColor m_titleColor;
#endif

    // At runtime, UrlSearchSnippet is parent to a QGraphicsWidget
    // (m_viewPort) that is parent to a UrlEditorWidget (m_editor).
    QGraphicsWidget * m_viewPort;
    GTextLineItem * m_editor;

    // Attributes
    QString m_text;
    QColor m_textColor;

    QColor m_backgroundColor;
    qreal m_padding;
    //TODO: add left margin too
    qreal m_rightTextMargin;

  };

  class GTextEditor : public GLineEditor
  {
    Q_OBJECT

  public:
    GTextEditor(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent = 0);
    virtual ~GTextEditor();
    void setBorderColor(QColor & color) { m_borderColor = color; }

  protected:
    virtual void paintBorder(QPainter * painter);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
  private:
    QColor m_borderColor;
  };


  class GProgressEditor : public GTextEditor
  {
    Q_OBJECT

  public:
    GProgressEditor(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent = 0);
    virtual ~GProgressEditor();

    void setProgressColor(QColor & color) { m_progressColor = color; }

    public slots:
    void setProgress(int percent);

  protected:
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

  private:
    int m_percent;
    QColor m_progressColor;
  };

  class TextEditItem : public NativeChromeItem 
  {
    Q_OBJECT
  public:
    TextEditItem(ChromeSnippet * snippet, ChromeWidget * chrome, QGraphicsItem * parent = 0);
    virtual ~TextEditItem();
    GTextEditor * editor() { return m_textEditor; } 
    QString text() { return m_textEditor->text(); }
    void setText(const QString & text){ m_textEditor->setText(text); }
    int characterCount() { return m_textEditor->characterCount(); }
    void setCursorPosition(int pos) { m_textEditor->setCursorPosition(pos); }
    void selectAll() { m_textEditor->selectAll(); }
    void unselect() { m_textEditor->unselect(); }
    int getTextOptions() { return (int) m_textEditor->inputMethodHints(); }
    // Calling this function will overwrite the existing options
    void setTextOptions (int flag);
    void setMaxTextLength(int length) { m_textEditor->setMaxTextLength(length); }
 
  private slots:	
	void tapped(QPointF&);
	void focusChanged(bool focusIn);

  protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent * ev);
  private:
    GTextEditor * m_textEditor;
	 bool m_justFocusIn;
  };

} // namespace GVA

#endif // __EDITOR_WIDGET_H__
