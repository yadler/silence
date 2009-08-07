#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/contentview.h"
#include "gui/editmenu.h"
#include "gui/filemenu.h"
#include "gui/helpmenu.h"
#include "gui/infosidebar.h"
#include "gui/statusbar.h"
#include "gui/treeview.h"
#include "gui/viewmenu.h"
#include <QMainWindow>


class MainWindow : public QMainWindow
{

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();
	private:
		ContentView *contentview;
		TreeView *treeview;
		InfoSidebar *infosidebar;
		StatusBar *statusbar;

		// Menus
		FileMenu *filemenu;
		EditMenu *editmenu;
		ViewMenu *viewmenu;
		HelpMenu *helpmenu;
};

#endif // MAINWINDOW_H


