#ifndef TEXTNODECONTENT_H
#define TEXTNODECONTENT_H

#include "node/abstractnodecontent.h"

class TextNodeContent : public AbstractNodeContent
{
	Q_OBJECT

	public:
		TextNodeContent();
		~TextNodeContent();

		void setText(QString text);
		QString getText() const;

		QDomElement getXmlData(QDomDocument &doc);
		void setXmlData(QDomElement &xmlNode);
		
		QWidget* getWidget();
		QHash<QString, QString>* getMetaInfos();
		void addMetaInfo(QString key, QString value);
		QString getMimeType();
		void setSyntax(QString syntax);

		QPixmap getPixmap();
		QIcon getIcon();
	
	signals:
		void changed();

	private:
		QHash<QString, QString> *metaInfos;
		QString text;

		QIcon icon;
		QPixmap pixmap;

		void setGraphics();
};

#endif // TEXTNODECONTENT_H


