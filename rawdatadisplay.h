#ifndef RAWDATADISPLAY_H
#define RAWDATADISPLAY_H

#include <QWidget>
#include "ui_rawdatadisplay.h"
class RawDataDisplay : public QWidget
{
	Q_OBJECT

public:
	explicit RawDataDisplay(QWidget *parent = 0);
	~RawDataDisplay();

private:
	Ui::RawDataDisplay ui;
};

#endif // RAWDATADISPLAY_H
