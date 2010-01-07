

TARGET = silence
TEMPLATE = app

SOURCES += 	main.cpp\
			controller.cpp\
			persistence/xmldatastore.cpp\
			data/node/node.cpp\
			data/node/nodeid.cpp\
			data/node/textnodecontent.cpp\
			data/node/richtextnodecontent.cpp\
			data/label.cpp\
			data/model/labelmodel.cpp\
			data/model/treemodel.cpp\
			data/model/simpletreemodel.cpp\
			data/model/filtermodel.cpp\
			data/model/listproxymodel.cpp\
			gui/mainwindow.cpp\
			gui/statusbar.cpp\
			gui/menu/nodemenu.cpp\
			gui/menu/silencemenu.cpp\
			gui/menu/helpmenu.cpp\
			gui/menu/editmenu.cpp\
			gui/menu/viewmenu.cpp\
			gui/dialog/aboutdialog.cpp\
			gui/dialog/newnodedialog.cpp\
			gui/dialog/newlinkdialog.cpp\
			gui/dialog/newtabledialog.cpp\
			gui/dialog/labelmanagementdialog.cpp\
			gui/sidebar/treeview.cpp\
			gui/sidebar/infosidebar.cpp\
			gui/sidebar/searchnodesidebar.cpp\
			gui/sidebar/nodepropertywidget.cpp\
			gui/view/textedit.cpp\
			gui/view/contentview.cpp\
			gui/view/welcomeview.cpp\
			gui/view/richtextedit.cpp\
			gui/widget/rtfedit.cpp\
			gui/widget/textfind.cpp\
			gui/widget/labelwidget.cpp\
			gui/widget/syntaxbox.cpp\
			gui/widget/inputwidget.cpp\
			gui/widget/searchnodeoptions.cpp\
			gui/widget/labellist.cpp\
			commandline/cmdmain.cpp\
			commandline/nodefilter.cpp\
			utils/listutils.cpp

HEADERS +=	controller.h\
			persistence/abstractdatastore.h\
			persistence/xmldatastore.h\
			data/node/node.h\
			data/node/nodeid.h\
			data/node/abstractnodecontent.h\
			data/node/textnodecontent.h\
			data/node/richtextnodecontent.h\
			data/label.h\
			data/model/labelmodel.h\
			data/model/treemodel.h\
			data/model/simpletreemodel.h\
			data/model/listproxymodel.h\
			data/model/filtermodel.h\
			gui/mainwindow.h\
			gui/statusbar.h\
			gui/menu/nodemenu.h\
			gui/menu/silencemenu.h\
			gui/menu/helpmenu.h\
			gui/menu/editmenu.h\
			gui/menu/viewmenu.h\
			gui/dialog/aboutdialog.h\
			gui/dialog/newnodedialog.h\
			gui/dialog/newlinkdialog.h\
			gui/dialog/newtabledialog.h\
			gui/dialog/labelmanagementdialog.h\
			gui/sidebar/treeview.h\
			gui/sidebar/infosidebar.h\
			gui/sidebar/searchnodesidebar.h\
			gui/sidebar/nodepropertywidget.h\
			gui/view/textedit.h\
			gui/view/contentview.h\
			gui/view/welcomeview.h\
			gui/view/richtextedit.h\
			gui/widget/rtfedit.h\
			gui/widget/textfind.h\
			gui/widget/labelwidget.h\
			gui/widget/syntaxbox.h\
			gui/widget/inputwidget.h\
			gui/widget/searchnodeoptions.h\
			gui/widget/labellist.h\
			commandline/cmdmain.h\
			commandline/nodefilter.h\
			utils/listutils.h

QT		+= xml

LIBS	+= -lqscintilla2

CONFIG	+= console

RESOURCES	=	silence.qrc

TRANSLATIONS	=	silence_de_DE.ts \
					silence_en_US.ts \
					silence_en_GB.ts 
