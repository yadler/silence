

TARGET = silence
TEMPLATE = app

SOURCES += 	main.cpp\
			gui/mainwindow.cpp\
			gui/filemenu.cpp\
			gui/helpmenu.cpp\
			gui/aboutdialog.cpp\
			gui/editmenu.cpp\
			gui/viewmenu.cpp\
			gui/treeview.cpp\
			gui/textedit.cpp\
			gui/infosidebar.cpp\
			gui/statusbar.cpp\
			gui/newnodedialog.cpp

HEADERS +=	gui/mainwindow.h\
			gui/filemenu.h\
			gui/helpmenu.h\
			gui/aboutdialog.h\
			gui/editmenu.h\
			gui/viewmenu.h\
			gui/treeview.h\
			gui/textedit.h\
			gui/infosidebar.h\
			gui/statusbar.h\
			gui/newnodedialog.h

LIBS	+=	-lqscintilla2


