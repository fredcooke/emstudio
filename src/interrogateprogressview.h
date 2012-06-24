#ifndef INTERROGATEPROGRESSVIEW_H
#define INTERROGATEPROGRESSVIEW_H

#include <QWidget>
#include "ui_interrogateprogressview.h"

class InterrogateProgressView : public QWidget
{
	Q_OBJECT
	
public:
	explicit InterrogateProgressView(QWidget *parent = 0);
	~InterrogateProgressView();
	void setProgress(int progress);
	void setMax(int max);
	int max() { return ui.progressBar->maximum(); }
	int progress() { return ui.progressBar->value(); }
private:
	Ui::InterrogateProgressView ui;
};

#endif // INTERROGATEPROGRESSVIEW_H
