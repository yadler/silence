#include "controller.h"
#include "gui/textedit.h"
#include <QApplication>
#include <QClipboard>
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
#include <QToolBar>
#include <QVBoxLayout>


TextEdit* TextEdit::textedit = 0;

TextEdit::TextEdit(QWidget *parent)
	: QWidget(parent)
{
	setupActions();
	setupEditor();

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(toolbar);
	layout->addWidget(editor);
	setLayout(layout);
	
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
}

TextEdit::~TextEdit()
{
	delete actionSave;
	delete actionUndo;
	delete actionRedo;
	delete actionCut;
	delete actionCopy;
	delete actionPaste;
	delete toolbar;
	delete editor;
}

TextEdit* TextEdit::create()
{
	if (textedit == 0)
		textedit = new TextEdit;
	return textedit;
}

void TextEdit::setupActions()
{
	EditMenu *menu = Controller::create()->getEditMenu();
	// Toolbar
	toolbar = new QToolBar;
	actionSave = toolbar->addAction(QIcon("icons/document-save.png"), tr("Save"));
	actionSave->setShortcut(QKeySequence::Save);
	toolbar->addSeparator();

	actionUndo = toolbar->addAction(QIcon("icons/edit-undo.png"), tr("Undo"));
	menu->addAction(actionUndo);
	actionUndo->setShortcut(QKeySequence::Undo);
	actionRedo = toolbar->addAction(QIcon("icons/edit-redo.png"), tr("Redo"));
	menu->addAction(actionRedo);
	actionRedo->setShortcut(QKeySequence::Redo);
	actionCut = toolbar->addAction(QIcon("icons/edit-cut.png"), tr("Cut"));
	menu->addAction(actionCut);
	actionCut->setShortcut(QKeySequence::Cut);
	actionCopy = toolbar->addAction(QIcon("icons/edit-copy.png"), tr("Copy"));
	menu->addAction(actionCopy);
	actionCopy->setShortcut(QKeySequence::Copy);
	actionPaste = toolbar->addAction(QIcon("icons/edit-paste.png"), tr("Paste"));
	menu->addAction(actionPaste);
	actionPaste->setShortcut(QKeySequence::Paste);
	toolbar->addSeparator();

	toolbar->addAction(QIcon("icons/edit-find.png"), tr("Find"));
	toolbar->addAction(QIcon("icons/edit-find-replace.png"), tr("Find/Replace"));
}

void TextEdit::setupEditor()
{
	// Editor
	editor = new QsciScintilla;

	editor->setLexer();
	editor->setTabWidth(4);
	// show linenumbers
	editor->setMarginWidth(0, 35);
	editor->setMarginLineNumbers(0, true);
}

void TextEdit::setContent(TextNodeContent *content)
{
	this->content = content;
	editor->setText(content->getText());
}

void TextEdit::saveContent()
{
	content->setText(editor->text());
}

void TextEdit::setSyntax(QString syntax)
{
	if (syntax == "Bash")
		editor->setLexer(new QsciLexerBash(editor));
	else if (syntax == "Batch")
		editor->setLexer(new QsciLexerBatch(editor));
	else if (syntax == "CMake")
		editor->setLexer(new QsciLexerCMake(editor));
	else if (syntax == "C++")
		editor->setLexer(new QsciLexerCPP(editor));
	else if (syntax == "C#")
		editor->setLexer(new QsciLexerCSharp(editor));
	else if (syntax == "CSS")
		editor->setLexer(new QsciLexerCSS(editor));
	else if (syntax == "D")
		editor->setLexer(new QsciLexerD(editor));
	else if (syntax == "Diff")
		editor->setLexer(new QsciLexerDiff(editor));
	else if (syntax == "HTML")
		editor->setLexer(new QsciLexerHTML(editor));
	else if (syntax == "IDL")
		editor->setLexer(new QsciLexerIDL(editor));
	else if (syntax == "Java")
		editor->setLexer(new QsciLexerJava(editor));
	else if (syntax == "JavaScript")
		editor->setLexer(new QsciLexerJavaScript(editor));
	else if (syntax == "Lua")
		editor->setLexer(new QsciLexerLua(editor));
	else if (syntax == "Makefile")
		editor->setLexer(new QsciLexerMakefile(editor));
	else if (syntax == "Perl")
		editor->setLexer(new QsciLexerPerl(editor));
	else if (syntax == "POV")
		editor->setLexer(new QsciLexerPOV(editor));
	else if (syntax == "Python")
		editor->setLexer(new QsciLexerPython(editor));
	else if (syntax == "Ruby")
		editor->setLexer(new QsciLexerRuby(editor));
	else if (syntax == "SQL")
		editor->setLexer(new QsciLexerSQL(editor));
	else if (syntax == "TeX")
		editor->setLexer(new QsciLexerTeX(editor));
	else if (syntax == "VHDL")
		editor->setLexer(new QsciLexerVHDL(editor));
	else
	{
		editor->setLexer();
		QFont font("Monospace", 10);
		editor->setFont(font);
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
	this->QWidget::setVisible(visible);
	actionUndo->setVisible(visible);
	actionRedo->setVisible(visible);
	actionCut->setVisible(visible);
	actionCopy->setVisible(visible);
	actionPaste->setVisible(visible);
}

