#ifndef ABOUTVIEW_H
#define ABOUTVIEW_H

#include <QWidget>
#include <QCloseEvent>
#include <QMdiSubWindow>
#include "ui_aboutview.h"

class AboutView : public QWidget
{
	Q_OBJECT
	
public:
	explicit AboutView(QWidget *parent = 0);
	~AboutView();
	void setHash(QString hash);
	void setCommit(QString commit);
protected:
	void closeEvent(QCloseEvent *event);
private:
	QString m_commit;
	QString m_hash;
	Ui::AboutView ui;
};

#endif // ABOUTVIEW_H
