#include "aboutview.h"


AboutView::AboutView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
}

AboutView::~AboutView()
{
}

void AboutView::setHash(QString hash)
{
	m_hash = hash;
	ui.commitHashLineEdit->setText(hash);
}

void AboutView::setCommit(QString commit)
{
	m_commit = commit;
	ui.commitIdLineEdit->setText(commit);

}
void AboutView::closeEvent(QCloseEvent *event)
{
	event->ignore();
	((QMdiSubWindow*)this->parent())->hide();
}
