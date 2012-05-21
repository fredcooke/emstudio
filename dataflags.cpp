#include "dataflags.h"


DataFlags::DataFlags(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
    /*m_dataFieldList.append(DataField("coreStatusA","Duplicate",60,1,1.0));
 m_dataFieldList.append(DataField("decoderFlags","Various decoder state flags",61,1,1.0));
 m_dataFieldList.append(DataField("flaggableFlags","Flags to go with flaggables",62,2,1.0));*/
}

DataFlags::~DataFlags()
{
}

void DataFlags::passData(QMap<QString,double> data)
{

}

void DataFlags::passDecoder(DataPacketDecoder *decoder)
{
}
