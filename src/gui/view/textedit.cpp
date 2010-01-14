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

#include "src/controller.h"
#include "src/gui/view/textedit.h"
#include <QApplication>
#include <QClipboard>
#include <QToolBar>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexercmake.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerd.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexeridl.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerpov.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexertex.h>
#include <Qsci/qscilexervhdl.h>
#include <KIcon>


TextEdit::TextEdit(QWidget *parent)
	: QWidget(parent)
{
	setupActions();
	setupEditor();

	layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(toolbar);
	layout->addWidget(editor);
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
	connect(actionFindReplace, SIGNAL(triggered()), findWidget, SLOT(showFull()));

	// cursorposition
	connect(editor, SIGNAL(cursorPositionChanged(int, int)), Controller::create()->getStatusBar(), SLOT(setCursorPosition(int, int)));

	// save
	connect(actionSave, SIGNAL(triggered()), this, SLOT(saveContent()));
	
	// undo, redo
	connect(editor, SIGNAL(textChanged()), this, SLOT(textModified()));

	setWindowModified(editor->isModified());
	actionUndo->setEnabled(editor->isUndoAvailable());
	actionRedo->setEnabled(editor->isRedoAvailable());

	connect(actionUndo, SIGNAL(triggered()), editor, SLOT(undo()));
	connect(actionRedo, SIGNAL(triggered()), editor, SLOT(redo()));

	// cut, copy, paste
	actionCut->setEnabled(false);
	actionCopy->setEnabled(false);

	connect(actionCut, SIGNAL(triggered()), editor, SLOT(cut()));
	connect(actionCopy, SIGNAL(triggered()), editor, SLOT(copy()));
	connect(actionPaste, SIGNAL(triggered()), editor, SLOT(paste()));

	connect(editor, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
	connect(editor, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));

	connect(QApplication::clipboard(), SIGNAL(dataChanged()), 
			this, SLOT(clipboardDataChanged()));

	// selectall
	connect(actionSelectAll, SIGNAL(triggered()), editor, SLOT(selectAll()));

	connect(editor, SIGNAL(textChanged()), this, SLOT(contentChanged()));
}

TextEdit::~TextEdit()
{
	delete actionSave;
	delete actionUndo;
	delete actionRedo;
	delete actionCut;
	delete actionCopy;
	delete actionPaste;
	delete actionSelectAll;
	delete actionFind;
	delete actionFindReplace;
	delete toolbar;
	delete editor;
	delete findWidget;
	delete layout;
}

void TextEdit::setupActions()
{
	EditMenu *menu = Controller::create()->getEditMenu();
	// Toolbar
	toolbar = new QToolBar;
	actionSave = toolbar->addAction(KIcon("document-save"), tr("&Save"));
	actionSave->setShortcut(QKeySequence::Save);
	toolbar->addSeparator();

	actionUndo = toolbar->addAction(KIcon("edit-undo"), tr("&Undo"));
	menu->addAction(actionUndo);
	actionUndo->setShortcut(QKeySequence::Undo);
	actionRedo = toolbar->addAction(KIcon("edit-redo"), tr("&Redo"));
	menu->addAction(actionRedo);
	actionRedo->setShortcut(QKeySequence::Redo);
	actionCut = toolbar->addAction(KIcon("edit-cut"), tr("Cut"));
	menu->addAction(actionCut);
	actionCut->setShortcut(QKeySequence::Cut);
	actionCopy = toolbar->addAction(KIcon("edit-copy"), tr("&Copy"));
	menu->addAction(actionCopy);
	actionCopy->setShortcut(QKeySequence::Copy);
	actionPaste = toolbar->addAction(KIcon("edit-paste"), tr("Paste"));
	menu->addAction(actionPaste);
	actionPaste->setShortcut(QKeySequence::Paste);
	toolbar->addSeparator();

	actionSelectAll = toolbar->addAction(KIcon("edit-select-all"), tr("Select &All"));
	menu->addAction(actionSelectAll);
	actionSelectAll->setShortcut(QKeySequence::SelectAll);
	actionFind = toolbar->addAction(KIcon("edit-find"), tr("&Find"));
	actionFind->setShortcut(QKeySequence::Find);
	actionFindReplace = toolbar->addAction(KIcon("edit-find-replace"), tr("Find/Replace"));
}

void TextEdit::setupEditor()
{
	// Editor
	editor = new QsciScintilla;

	editor->setLexer();
	editor->setUtf8(true);
	editor->setTabWidth(4);
	// show linenumbers
	editor->setMarginWidth(0, 35);
	editor->setMarginLineNumbers(0, true);
	editor->setWrapMode(QsciScintilla::WrapWord);
}

void TextEdit::setContent(TextNodeContent *content)
{
	this->content = content;
	editor->setText(content->getText());
	Controller::create()->getStatusBar()->setSaveStatus(true);
}

void TextEdit::saveContent()
{
	content->setText(editor->text());
	Controller::create()->getStatusBar()->setSaveStatus(true);
}

void TextEdit::setSyntax(QString syntax)
{
	QsciLexer *lexer = 0;
	if (syntax == "Bash")
		lexer = new QsciLexerBash(editor);
	else if (syntax == "Batch")
		lexer = new QsciLexerBatch(editor);
	else if (syntax == "CMake")
		lexer = new QsciLexerCMake(editor);
	else if (syntax == "C++")
		lexer = new QsciLexerCPP(editor);
	else if (syntax == "C#")
		lexer = new QsciLexerCSharp(editor);
	else if (syntax == "CSS")
		lexer = new QsciLexerCSS(editor);
	else if (syntax == "D")
		lexer = new QsciLexerD(editor);
	else if (syntax == "Diff")
		lexer = new QsciLexerDiff(editor);
	else if (syntax == "HTML")
		lexer = new QsciLexerHTML(editor);
	else if (syntax == "IDL")
		lexer = new QsciLexerIDL(editor);
	else if (syntax == "Java")
		lexer = new QsciLexerJava(editor);
	else if (syntax == "JavaScript")
		lexer = new QsciLexerJavaScript(editor);
	else if (syntax == "Lua")
		lexer = new QsciLexerLua(editor);
	else if (syntax == "Makefile")
		lexer = new QsciLexerMakefile(editor);
	else if (syntax == "Perl")
		lexer = new QsciLexerPerl(editor);
	else if (syntax == "POV")
		lexer = new QsciLexerPOV(editor);
	else if (syntax == "Python")
		lexer = new QsciLexerPython(editor);
	else if (syntax == "Ruby")
		lexer = new QsciLexerRuby(editor);
	else if (syntax == "SQL")
		lexer = new QsciLexerSQL(editor);
	else if (syntax == "TeX")
		lexer = new QsciLexerTeX(editor);
	else if (syntax == "VHDL")
		lexer = new QsciLexerVHDL(editor);

	if (lexer)
	{
		lexer->setFont(QFont("Monospace", 10));
		editor->setLexer(lexer);
	} else {
		editor->setLexer();
		editor->setFont(QFont("Monospace", 10));
	}

}

void TextEdit::textModified()
{
	actionUndo->setEnabled(editor->isUndoAvailable());
	actionRedo->setEnabled(editor->isRedoAvailable());
}

void TextEdit::clipboardDataChanged()
{
	actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void TextEdit::setVisible(bool visible)
{
	Controller::create()->getEditMenu()->setEnabled(true);
	this->QWidget::setVisible(visible);
	actionUndo->setVisible(visible);
	actionRedo->setVisible(visible);
	actionCut->setVisible(visible);
	actionCopy->setVisible(visible);
	actionPaste->setVisible(visible);
	actionSelectAll->setVisible(visible);
}

void TextEdit::findFirst()
{
	editor->setCursorPosition(0, 0);
	find(true);
}

void TextEdit::findNext()
{
	find(true);
}

void TextEdit::findPrev()
{
	find(false);
}

bool TextEdit::find(bool forward, bool wrap)
{
	bool found = false;
	if (findWidget->getSearchString().isEmpty())
		return found;

	bool regex = false;
	bool caseSensitive = findWidget->getCaseSensitivity();
	bool wholeWord = findWidget->getWholeWord();
	int line = -1;
	int index = -1;
	if (forward == false)
	{
		editor->getCursorPosition(&line, &index);
		--index;
	}
	bool show = true;
	found = editor->findFirst(findWidget->getSearchString(), regex, caseSensitive, wholeWord, wrap, forward, line, index, show);
	findWidget->setFound(found);

	return found;
}

void TextEdit::replace()
{
	if (editor->selectedText() == findWidget->getSearchString())
		editor->replace(findWidget->getReplaceString());
	else
		if (find(true))
			editor->replace(findWidget->getReplaceString());
}

void TextEdit::replaceAll()
{
	editor->setCursorPosition(0, 0);
	bool found = false;
	do {
		found = find(true, false);
		if (found)
			editor->replace(findWidget->getReplaceString());
	} while (found);
}

void TextEdit::contentChanged()
{
	Controller::create()->getStatusBar()->setSaveStatus(false);
}
