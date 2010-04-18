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

#include "src/gui/mainwindow.h"
#include <KMenuBar>


MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags f)
	: KMainWindow(parent, f)
{
	setWindowTitle(tr("Silence"));
	setWindowIcon(QIcon(":/icons/Silence_16px.png"));
	controller = Controller::create();

	// ContentView
	contentview = new ContentView;
	setCentralWidget(contentview);
	controller->setContentView(contentview);


	// information sidebar to show the meta-infos
	infosidebar = new InfoSidebar(tr("Info"), this);
	controller->setInfoSidebar(infosidebar);

	nodepropertywidget = new NodePropertyWidget(tr("Properties"), this);
	nodepropertywidget->hide();
	controller->setNodePropertyWidget(nodepropertywidget);

	// the treeview on the left side
	treeview = new TreeView(tr("Nodes"), this);
	controller->setTreeView(treeview);

	// search sidebar
	searchnodesidebar = new SearchNodeSidebar(tr("Search Nodes"), this);
	controller->setSearchNodeSidebar(searchnodesidebar);

	// navigation sidebar
	navigationsidebar = new NavigationSidebar(tr("Navigation"), this);
	controller->setNavigationSidebar(navigationsidebar);

	// docking the widgets
	addDockWidget(Qt::RightDockWidgetArea, navigationsidebar);
	addDockWidget(Qt::RightDockWidgetArea, infosidebar);
	addDockWidget(Qt::LeftDockWidgetArea, treeview);
	addDockWidget(Qt::LeftDockWidgetArea, searchnodesidebar);
	tabifyDockWidget(searchnodesidebar, treeview);
	addDockWidget(Qt::LeftDockWidgetArea, nodepropertywidget);

	statusbar = new StatusBar;
	setStatusBar(statusbar);
	controller->setStatusBar(statusbar);

	// set the initial windowsize
	resize(950, 500);
	
	// add menus
	silencemenu = new SilenceMenu(this);
	menuBar()->addMenu(silencemenu);
	nodemenu = new NodeMenu;
	menuBar()->addMenu(nodemenu);
	editmenu = new EditMenu;
	editmenu->setEnabled(false);
	menuBar()->addMenu(editmenu);
	controller->setEditMenu(editmenu);
	viewmenu = new ViewMenu;
	menuBar()->addMenu(viewmenu);
	menuBar()->addMenu(helpMenu(""));
}

MainWindow::~MainWindow()
{   
	delete contentview;
	delete treeview;
	delete infosidebar;
	delete statusbar;
	delete nodepropertywidget;
	delete searchnodesidebar;

	delete nodemenu;
	delete editmenu;
	delete viewmenu;
	delete silencemenu;

	delete controller;
}

