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

#ifndef SISTATUSBAR_H
#define SISTATUSBAR_H

#include <QLabel>
#include <KStatusBar>


/*!\class SiStatusBar
 * \brief The statusbar for the MainWindow.
 *
 * This SiStatusBar is used in the MainWindow to show some status informations.
 *
 * \author Manuel Unglaub
 */
class SiStatusBar : public KStatusBar
{
	Q_OBJECT

	public:
		/*! The constructor.
		 *
		 * Constructs the SiStatusBar which is a child of parent.
		 * \param parent The parent of this SiStatusBar.
		 */
		SiStatusBar(QWidget *parent = 0);

		/*! The destructor.
		 *
		 * Destroys the object.
		 */
		~SiStatusBar();

		/*!
		 * Display the currently selected nodes name.
		 * \param name The string which will be shown in this SiStatusBar.
		 */
		void setNodeName(QString name);
	
	public slots:
		/*!
		 * Display the current cursor position in this SiStatusBar.
		 * \param line Set the line of the current cursor position.
		 * \param col Set the column of the current cursor position.
		 */
		void setCursorPosition(int line, int col);

		/*!
		 * Show if the currently selected node is saved.
		 * \param saved If true it will show that the content is saved and if it is false it will show that the content is changend.
		 */
		void setSaveStatus(bool saved);

	private:
		QLabel	*nodeName,
				*saveStatus,
				*cursorPos;
};

#endif // SISTATUSBAR_H



