#include "interrogateprogressview.h"


InterrogateProgressView::InterrogateProgressView(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
}

InterrogateProgressView::~InterrogateProgressView()
{
}
void InterrogateProgressView::setProgress(int progress)
{
	ui.progressBar->setValue(progress);
}

void InterrogateProgressView::setMax(int max)
{
	ui.progressBar->setMaximum(max);
}
