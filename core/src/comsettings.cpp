/***************************************************************************
*   Copyright (C) 2012  Michael Carpenter (malcom2073)                     *
*                                                                          *
*   This file is a part of EMStudio                                        *
*                                                                          *
*   EMStudio is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License version 2 as      *
*   published by the Free Software Foundation.                             *
*                                                                          *
*   EMStudio is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
									   *
*   You should have received a copy of the GNU General Public License      *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
****************************************************************************/

#include "comsettings.h"
#include <QFileDialog>
ComSettings::ComSettings(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.cancelPushButton,SIGNAL(clicked()),this,SIGNAL(cancelClicked()));
	connect(ui.savePushButton,SIGNAL(clicked()),this,SIGNAL(saveClicked()));
	connect(ui.datalogLocationBrowseButton,SIGNAL(clicked()),this,SLOT(browseLogFolderClicked()));
}

ComSettings::~ComSettings()
{
}
void ComSettings::setInterByteDelay(int delay)
{
	ui.interByteDelaySpinBox->setValue(delay);
}
void ComSettings::browseLogFolderClicked()
{
	QString dir = QFileDialog::getExistingDirectory(this,"Select Log Folder",".");
	if (dir == "")
	{
		return;
	}
	ui.datalogLocationLineEdit->setText(dir);
}

void ComSettings::setBaud(int baud)
{
	ui.baudRateLineEdit->setText(QString::number(baud));
}
void ComSettings::setComPort(QString port)
{
	ui.portNameLineEdit->setText(port);
}
QString ComSettings::getDataLogDir()
{
	return ui.datalogLocationLineEdit->text();
}

bool ComSettings::getSaveDataLogs()
{
	return (ui.saveDataLogCheckBox->checkState() == Qt::Checked);
}

bool ComSettings::getClearDataLogs()
{
	return (ui.clearDataLogsCheckBox->checkState() == Qt::Checked);
}

int ComSettings::getNumLogsToSave()
{
	return ui.logCountSpinBox->value();
}

void ComSettings::setDataLogDir(QString dir)
{
	ui.datalogLocationLineEdit->setText(dir);
}

void ComSettings::setSaveDataLogs(bool save)
{
	if (save)
	{
		ui.saveDataLogCheckBox->setCheckState(Qt::Checked);
	}
	else
	{
		ui.saveDataLogCheckBox->setCheckState(Qt::Unchecked);
	}
}

void ComSettings::setClearDataLogs(bool clear)
{
	if (clear)
	{
		ui.clearDataLogsCheckBox->setCheckState(Qt::Checked);
	}
	else
	{
		ui.clearDataLogsCheckBox->setCheckState(Qt::Unchecked);
	}
}

void ComSettings::setNumLogsToSave(int num)
{
	ui.logCountSpinBox->setValue(num);
}


int ComSettings::getInterByteDelay()
{
	return ui.interByteDelaySpinBox->value();
}
int ComSettings::getBaud()
{
	return ui.baudRateLineEdit->text().toInt();
}
QString ComSettings::getComPort()
{
	return ui.portNameLineEdit->text();
}
