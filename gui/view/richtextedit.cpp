/*
 * Silence
 *
 * Copyright (C) 2009 Manuel Unglaub
 *
 * This file is part of Silence.
 *
 * Silence is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version GPLv2 only of the License.
 *
 * Silence is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Silence.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "controller.h"
#include "gui/view/richtextedit.h"
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDatabase>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextList>


RichTextEdit::RichTextEdit(QWidget *parent)
	: QWidget(parent)
{
	layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	
	toolbar = new QToolBar;
	setupActions();
	layout->addWidget(toolbar);

	fontToolbar = new QToolBar;
	setupFontActions();
	layout->addWidget(fontToolbar);

	textedit = new RtfEdit;
	QList<QAction*> contextActions;
	contextActions.append(actionUndo);
	contextActions.append(actionRedo);
	contextActions.append(actionCut);
	contextActions.append(actionCopy);
	contextActions.append(actionPaste);
	contextActions.append(actionSelectAll);
	textedit->addContextActions(contextActions);
	textedit->setTabStopWidth(40);
	layout->addWidget(textedit);

	findWidget = new TextFind(this);
	findWidget->hide();
	layout->addWidget(findWidget);

	setLayout(layout);

	// find
	connect(actionFind, SIGNAL(triggered()), findWidget, SLOT(show()));
	connect(findWidget, SIGNAL(findNext()), this, SLOT(findNext()));
	connect(findWidget, SIGNAL(findPrev()), this, SLOT(findPrev()));
	connect(findWidget, SIGNAL(searchStringChanged(const QString&)), this, SLOT(findFirst()));
	connect(findWidget, SIGNAL(replace()), this, SLOT(replace()));
	connect(findWidget, SIGNAL(replaceAll()), this, SLOT(replaceAll()));

	// save
	connect(actionSave, SIGNAL(triggered()), this, SLOT(saveContent()));

	// alignment
	alignmentChanged(textedit->alignment());
	connect(textedit, SIGNAL(cursorPositionChanged()), 
			this, SLOT(cursorPositionChanged()));

	// color & font
	connect(textedit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)),
			this, SLOT(currentCharFormatChanged(const QTextCharFormat &)));
	colorChanged(textedit->textColor());	
	fontChanged(textedit->font());

	// undo & redo
	connect(textedit->document(), SIGNAL(undoAvailable(bool)), 
			actionUndo, SLOT(setEnabled(bool)));
	connect(textedit->document(), SIGNAL(redoAvailable(bool)), 
			actionRedo, SLOT(setEnabled(bool)));
	setWindowModified(textedit->document()->isModified());
	actionUndo->setEnabled(textedit->document()->isUndoAvailable());
	actionRedo->setEnabled(textedit->document()->isRedoAvailable());

	connect(actionUndo, SIGNAL(triggered()), textedit, SLOT(undo()));
	connect(actionRedo, SIGNAL(triggered()), textedit, SLOT(redo()));

	// cut, copy, paste
	actionCut->setEnabled(false);
	actionCopy->setEnabled(false);

	connect(actionCut, SIGNAL(triggered()), textedit, SLOT(cut()));
	connect(actionCopy, SIGNAL(triggered()), textedit, SLOT(copy()));
	connect(actionPaste, SIGNAL(triggered()), textedit, SLOT(paste()));

	connect(textedit, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
	connect(textedit, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));

	connect(QApplication::clipboard(), SIGNAL(dataChanged()), 
			this, SLOT(clipboardDataChanged()));

	// select all
	connect(actionSelectAll, SIGNAL(triggered()), textedit, SLOT(selectAll()));

	connect(textedit, SIGNAL(textChanged()), this, SLOT(contentChanged()));

	// lists
	connect(actionIncreaseIndent, SIGNAL(triggered()), this, SLOT(increaseIndent()));
	connect(actionDecreaseIndent, SIGNAL(triggered()), this, SLOT(decreaseIndent()));
	connect(actionOrderedList, SIGNAL(triggered()), this, SLOT(createOrderedList()));
	connect(actionUnorderedList, SIGNAL(triggered()), this, SLOT(createUnorderedList()));

	// picture
	connect(actionAddPicture, SIGNAL(triggered()), this, SLOT(addPicture()));
}


RichTextEdit::~RichTextEdit()
{
	delete actionSave;
	delete actionUndo;
	delete actionRedo;
	delete actionCut;
	delete actionCopy;
	delete actionPaste;
	delete actionSelectAll;
	delete actionTextBold;
	delete actionTextItalic;
	delete actionTextUnderline;
	delete actionAlignLeft;
	delete actionAlignCenter;
	delete actionAlignRight;
	delete actionAlignJustify;
	delete actionTextColor;
	delete actionFind;
	delete actionIncreaseIndent;
	delete actionDecreaseIndent;
	delete actionOrderedList;
	delete actionUnorderedList;
	delete findWidget;
	delete toolbar;
	delete comboFont;
	delete comboSize;
	delete fontToolbar;
	delete textedit;
	delete layout;
}


void RichTextEdit::setupActions()
{
	EditMenu *menu = Controller::create()->getEditMenu();
	toolbar->setWindowTitle(tr("Edit Actions"));

	actionSave = toolbar->addAction(QIcon(":/icons/actions/document-save.png"), tr("Save"));
	actionSave->setShortcut(QKeySequence::Save);
	toolbar->addSeparator();

	actionUndo = toolbar->addAction(QIcon(":/icons/actions/edit-undo.png"), tr("Undo"));
	menu->addAction(actionUndo);
	actionUndo->setShortcut(QKeySequence::Undo);
	actionRedo = toolbar->addAction(QIcon(":/icons/actions/edit-redo.png"), tr("Redo"));
	menu->addAction(actionRedo);
	actionRedo->setShortcut(QKeySequence::Redo);
	actionCut = toolbar->addAction(QIcon(":/icons/actions/edit-cut.png"), tr("Cut"));
	menu->addAction(actionCut);
	actionCut->setShortcut(QKeySequence::Cut);
	actionCopy = toolbar->addAction(QIcon(":/icons/actions/edit-copy.png"), tr("Copy"));
	menu->addAction(actionCopy);
	actionCopy->setShortcut(QKeySequence::Copy);
	actionPaste = toolbar->addAction(QIcon(":/icons/actions/edit-paste.png"), tr("Paste"));
	menu->addAction(actionPaste);
	actionPaste->setShortcut(QKeySequence::Paste);
	actionSelectAll = toolbar->addAction(QIcon(":/icons/actions/edit-select-all.png"), tr("Select All"));
	menu->addAction(actionSelectAll);
	actionSelectAll->setShortcut(QKeySequence::SelectAll);
	toolbar->addSeparator();

	actionTextBold = toolbar->addAction(QIcon(":/icons/actions/format-text-bold.png"), tr("Bold"));
	menu->addAction(actionTextBold);
	actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
	QFont bold;
	bold.setBold(true);
	actionTextBold->setFont(bold);
	connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
	actionTextBold->setCheckable(true);

	actionTextItalic = toolbar->addAction(QIcon(":/icons/actions/format-text-italic.png"), tr("Italic"));
	menu->addAction(actionTextItalic);
	actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
	QFont italic;
	italic.setItalic(true);
	actionTextItalic->setFont(italic);
	connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
	actionTextItalic->setCheckable(true);

	actionTextUnderline = toolbar->addAction(QIcon(":/icons/actions/format-text-underline.png"), tr("Underline"));
	menu->addAction(actionTextUnderline);
	actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
	QFont underline;
	underline.setUnderline(true);
	actionTextUnderline->setFont(underline);
	connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
	actionTextUnderline->setCheckable(true);	
	
	
	toolbar->addSeparator();

	QActionGroup *grp = new QActionGroup(this);
	connect(grp, SIGNAL(triggered(QAction *)), this, SLOT(textAlign(QAction *)));

	actionAlignLeft = new QAction(QIcon(":/icons/actions/format-justify-left.png"), tr("&Left"), grp);
	actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
	actionAlignLeft->setCheckable(true);
	actionAlignCenter = new QAction(QIcon(":/icons/actions/format-justify-center.png"), tr("C&enter"), grp);
	actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
	actionAlignCenter->setCheckable(true);
	actionAlignRight = new QAction(QIcon(":/icons/actions/format-justify-right.png"), tr("&Right"), grp);
	actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
	actionAlignRight->setCheckable(true);
	actionAlignJustify = new QAction(QIcon(":/icons/actions/format-justify-fill.png"), tr("&Justify"), grp);
	actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
	actionAlignJustify->setCheckable(true);

	toolbar->addActions(grp->actions());
	menu->addActions(grp->actions());
	toolbar->addSeparator();

	actionUnorderedList = toolbar->addAction(QIcon(":/icons/actions/format-list-unordered.png"), tr("Create Unordered List"));
	actionOrderedList = toolbar->addAction(QIcon(":/icons/actions/format-list-ordered.png"), tr("Create Ordered List"));
	actionIncreaseIndent = toolbar->addAction(QIcon(":/icons/actions/format-indent-more.png"), tr("Indent more"));
	actionDecreaseIndent = toolbar->addAction(QIcon(":/icons/actions/format-indent-less.png"), tr("Indent less"));
	toolbar->addSeparator();

	actionAddPicture = toolbar->addAction(QIcon(":/icons/actions/insert-image.png"), tr("Insert Image"));

	// color
	QPixmap pix(16, 16);
	pix.fill(Qt::black);
	actionTextColor = new QAction(pix, tr("&Color..."), this);
	connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
	toolbar->addAction(actionTextColor);
	menu->addAction(actionTextColor);

	actionFind = toolbar->addAction(QIcon(":/icons/actions/edit-find.png"), tr("&Find"));
	actionFind->setShortcut(QKeySequence::Find);
	menu->addAction(actionFind);
}

void RichTextEdit::setupFontActions()
{
	comboFont = new QFontComboBox(fontToolbar);
	fontToolbar->addWidget(comboFont);
	connect(comboFont, SIGNAL(activated(const QString &)),
		this, SLOT(textFamily(const QString &)));


	comboSize = new QComboBox(fontToolbar);
	comboSize->setObjectName("comboSize");
	fontToolbar->addWidget(comboSize);
	comboSize->setEditable(true);
	
	QFontDatabase db;
	foreach(int size, db.standardSizes())
		comboSize->addItem(QString::number(size));
	
	connect(comboSize, SIGNAL(activated(const QString &)),
			this, SLOT(textSize(const QString &)));
	comboSize->setCurrentIndex(comboSize->findText(QString::number(
		QApplication::font().pointSize())));


}

void RichTextEdit::textBold()
{
	QTextCharFormat fmt;
	fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
	mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textItalic()
{
	QTextCharFormat fmt;
	fmt.setFontItalic(actionTextItalic->isChecked());
	mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textUnderline()
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(actionTextUnderline->isChecked());
	mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
	QTextCursor cursor = textedit->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	textedit->mergeCurrentCharFormat(format);
}

void RichTextEdit::textAlign(QAction *action)
{
	if (action == actionAlignLeft)
		textedit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
	else if (action == actionAlignCenter)
		textedit->setAlignment(Qt::AlignHCenter);
	else if (action == actionAlignRight)
		textedit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
	else if (action == actionAlignJustify)
		textedit->setAlignment(Qt::AlignJustify);

}

void RichTextEdit::alignmentChanged(Qt::Alignment alignment)
{
	if (alignment & Qt::AlignLeft) {
		actionAlignLeft->setChecked(true);
	} else if (alignment & Qt::AlignHCenter) {
		actionAlignCenter->setChecked(true);
	} else if (alignment & Qt::AlignRight) {
		actionAlignRight->setChecked(true);
	} else if (alignment & Qt::AlignJustify) {
		actionAlignJustify->setChecked(true);
	}
}

void RichTextEdit::cursorPositionChanged()
{
	alignmentChanged(textedit->alignment());
	int line = textedit->textCursor().block().blockNumber();
	int col = textedit->textCursor().position() - textedit->textCursor().block().position();
	Controller::create()->getStatusBar()->setCursorPosition(line, col);
}

void RichTextEdit::textColor()
{
	QColor col = QColorDialog::getColor(textedit->textColor(), this);
	if (!col.isValid())
		return;
	QTextCharFormat fmt;
	fmt.setForeground(col);
	mergeFormatOnWordOrSelection(fmt);
	colorChanged(col);
}

void RichTextEdit::colorChanged(const QColor &color)
{
	QPixmap pix(16, 16);
	pix.fill(color);
	actionTextColor->setIcon(pix);
}

void RichTextEdit::textFamily(const QString &font)
{
	QTextCharFormat fmt;
	fmt.setFontFamily(font);
	mergeFormatOnWordOrSelection(fmt);
}

void RichTextEdit::textSize(const QString &p)
{
	qreal pointSize = p.toFloat();
	if (p.toFloat() > 0) {
		QTextCharFormat fmt;
		fmt.setFontPointSize(pointSize);
		mergeFormatOnWordOrSelection(fmt);
	}
}

void RichTextEdit::fontChanged(const QFont &font)
{
	comboFont->setCurrentIndex(comboFont->findText(QFontInfo(font).family()));
	comboSize->setCurrentIndex(comboSize->findText(QString::number(font.pointSize())));
	actionTextBold->setChecked(font.bold());
	actionTextItalic->setChecked(font.italic());
	actionTextUnderline->setChecked(font.underline());
}

void RichTextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
	fontChanged(format.font());
	colorChanged(format.foreground().color());
}

void RichTextEdit::setContent(RichTextNodeContent *content)
{
	this->content = content;
	textedit->setHtml(content->getText());
	Controller::create()->getStatusBar()->setSaveStatus(true);
}

void RichTextEdit::saveContent()
{
	content->setText(textedit->toHtml());
	Controller::create()->getStatusBar()->setSaveStatus(true);
}

void RichTextEdit::clipboardDataChanged()
{
	actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void RichTextEdit::setVisible(bool visible)
{
	Controller::create()->getEditMenu()->setEnabled(true);
	this->QWidget::setVisible(visible);
	actionUndo->setVisible(visible);
	actionRedo->setVisible(visible);
	actionCut->setVisible(visible);
	actionCopy->setVisible(visible);
	actionPaste->setVisible(visible);
	actionSelectAll->setVisible(visible);
	actionTextBold->setVisible(visible);
	actionTextItalic->setVisible(visible);
	actionTextUnderline->setVisible(visible);
	actionAlignLeft->setVisible(visible);
	actionAlignCenter->setVisible(visible);
	actionAlignRight->setVisible(visible);
	actionAlignJustify->setVisible(visible);
	actionTextColor->setVisible(visible);
	actionFind->setVisible(visible);
}

bool RichTextEdit::find(bool forward)
{
	bool found = false;
	if (findWidget->getSearchString().isEmpty())
		return found;

	QTextDocument::FindFlags flags = 0;
	
	if (!forward)
		flags = flags | QTextDocument::FindBackward;
	if (findWidget->getCaseSensitivity())
		flags = flags | QTextDocument::FindCaseSensitively;
	if (findWidget->getWholeWord())
		flags = flags | QTextDocument::FindWholeWords;

	found = textedit->find(findWidget->getSearchString(), flags);
	findWidget->setFound(found);
	return found;
}

void RichTextEdit::findNext()
{
	find(true);
}

void RichTextEdit::findPrev()
{
	find(false);
}

void RichTextEdit::findFirst()
{
	textedit->moveCursor(QTextCursor::Start);
	find(true);
}

void RichTextEdit::replace()
{
	if (textedit->textCursor().selectedText() == findWidget->getSearchString())
		textedit->insertPlainText(findWidget->getReplaceString());
	else
		if (find(true))
			textedit->insertPlainText(findWidget->getReplaceString());
}

void RichTextEdit::replaceAll()
{
	textedit->moveCursor(QTextCursor::Start);

	bool found = false;
	do {
		found = find(true);
		if (found)
			textedit->insertPlainText(findWidget->getReplaceString());
	} while (found);
}

void RichTextEdit::increaseIndent()
{
	QTextCursor cursor = textedit->textCursor();
	if (cursor.currentList())
	{
		cursor.beginEditBlock();

		QTextListFormat listFmt = cursor.currentList()->format();
		listFmt.setIndent(listFmt.indent() + 1);

		switch (listFmt.style()) {
			default:
			case QTextListFormat::ListDisc:
				listFmt.setStyle(QTextListFormat::ListCircle);
				break;
			case QTextListFormat::ListCircle:
				listFmt.setStyle(QTextListFormat::ListSquare);
				break;
			case QTextListFormat::ListSquare:
				listFmt.setStyle(QTextListFormat::ListDisc);
				break;

			case QTextListFormat::ListDecimal:
				listFmt.setStyle(QTextListFormat::ListLowerAlpha);
				break;
			case QTextListFormat::ListLowerAlpha:
				listFmt.setStyle(QTextListFormat::ListUpperAlpha);
				break;
			case QTextListFormat::ListUpperAlpha:
				listFmt.setStyle(QTextListFormat::ListDecimal);
				break;
		}
		cursor.createList(listFmt);

		cursor.endEditBlock();
	} else {
		QTextBlockFormat bfmt;
		bfmt.setIndent(cursor.blockFormat().indent() + 1);
		cursor.mergeBlockFormat(bfmt);
	}
}

void RichTextEdit::decreaseIndent()
{
	QTextCursor cursor = textedit->textCursor();
	if (cursor.currentList())
	{
		cursor.beginEditBlock();

		QTextListFormat listFmt = cursor.currentList()->format();
		if (listFmt.indent() > 1)
		{
			listFmt.setIndent(listFmt.indent() - 1);
			switch (listFmt.style()) {
				default:
				case QTextListFormat::ListDisc:
					listFmt.setStyle(QTextListFormat::ListSquare);
					break;
				case QTextListFormat::ListCircle:
					listFmt.setStyle(QTextListFormat::ListDisc);
					break;
				case QTextListFormat::ListSquare:
					listFmt.setStyle(QTextListFormat::ListCircle);
					break;

				case QTextListFormat::ListDecimal:
					listFmt.setStyle(QTextListFormat::ListUpperAlpha);
					break;
				case QTextListFormat::ListLowerAlpha:
					listFmt.setStyle(QTextListFormat::ListDecimal);
					break;
				case QTextListFormat::ListUpperAlpha:
					listFmt.setStyle(QTextListFormat::ListLowerAlpha);
					break;
			}
			cursor.createList(listFmt);
		}
		cursor.endEditBlock();
	} else {
		if (cursor.blockFormat().indent() > 0) {
			QTextBlockFormat bfmt;
			bfmt.setIndent(cursor.blockFormat().indent() - 1);
			cursor.mergeBlockFormat(bfmt);
		}
	}
}

void RichTextEdit::createOrderedList()
{
	createList(QTextListFormat::ListDecimal);
}

void RichTextEdit::createUnorderedList()
{
	createList(QTextListFormat::ListDisc);
}

void RichTextEdit::createList(QTextListFormat::Style style)
{
	QTextCursor cursor = textedit->textCursor();
	if (cursor.currentList())
		return;

	cursor.beginEditBlock();

	QTextBlockFormat blockFmt = cursor.blockFormat();
	QTextListFormat listFmt;

	listFmt.setIndent(blockFmt.indent() + 1);
	blockFmt.setIndent(0);
	cursor.setBlockFormat(blockFmt);

	listFmt.setStyle(style);
	cursor.createList(listFmt);

	cursor.endEditBlock();
}

void RichTextEdit::contentChanged()
{
	Controller::create()->getStatusBar()->setSaveStatus(false);
}

void RichTextEdit::addPicture()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp);; All Files (*)"));

	if (fileName.isEmpty())
		return;

	QImage image(fileName);
	if (image.isNull())
	{
		// TODO: inform the user about the problem
		return;
	}

	//TODO: copy the image to a safe location

	QTextCursor cursor = textedit->textCursor();
	cursor.beginEditBlock();
	cursor.insertImage(fileName);
	cursor.endEditBlock();
}



