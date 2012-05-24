#ifndef RAWDATAVIEW_H
#define RAWDATAVIEW_H

#include <QWidget>

namespace Ui {
class RawDataView;
}

class RawDataView : public QWidget
{
	Q_OBJECT
	
public:
	explicit RawDataView(QWidget *parent = 0);
	~RawDataView();
	
private:
	Ui::RawDataView *ui;
};

#endif // RAWDATAVIEW_H
