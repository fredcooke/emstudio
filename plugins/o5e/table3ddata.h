/**************************************************************************************
 * Copyright (c) 2013, Michael Carpenter  (malcom2073@gmail.com)                      *
 * All rights reserved.                                                               *
 *                                                                                    *
 * Redistribution and use in source and binary forms, with or without                 *
 * modification, are permitted provided that the following conditions are met:        *
 *     * Redistributions of source code must retain the above copyright               *
 *       notice, this list of conditions and the following disclaimer.                *
 *     * Redistributions in binary form must reproduce the above copyright            *
 *       notice, this list of conditions and the following disclaimer in the          *
 *       documentation and/or other materials provided with the distribution.         *
 *     * Neither the name EMStudio nor the                                            *
 *       names of its contributors may be used to endorse or promote products         *
 *       derived from this software without specific prior written permission.        *
 *                                                                                    *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND    *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED      *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE             *
 * DISCLAIMED. IN NO EVENT SHALL MICHAEL CARPENTER BE LIABLE FOR ANY                  *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES         *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;       *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND        *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT         *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS      *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                       *
 **************************************************************************************/

#ifndef TABLE3DDATA_H
#define TABLE3DDATA_H

#include <QObject>
#include "headers.h"
#include "tabledata.h"
class Table3DData : public TableData
{
	Q_OBJECT
public:
	//Table3DData(unsigned short locationid,bool isflashonly, QByteArray data,Table3DMetaData metadata);
	virtual void setData(unsigned short locationid,bool isflashonly,QByteArray payload,Table3DMetaData metadata)=0;
	virtual void setData(unsigned short locationid,bool isflashonly,QByteArray payload)=0;
	virtual QByteArray data()=0;
	virtual QList<double> xAxis()=0;
	virtual QList<double> yAxis()=0;
	virtual QList<QList<double> > values()=0;
	virtual int columns()=0;
	virtual int rows()=0;
	virtual void setCell(int row, int column,double val)=0;
	virtual void setXAxis(int column,double val)=0;
	virtual void setYAxis(int row,double val)=0;
	virtual double maxXAxis()=0;
	virtual double maxYAxis()=0;
	virtual double maxZAxis()=0;
	virtual double minXAxis()=0;
	virtual double minYAxis()=0;
	virtual double minZAxis()=0;
	virtual void setWritesEnabled(bool enabled)=0;
	virtual void writeWholeLocation()=0;
signals:
	void saveSingleData(unsigned short locationid,QByteArray data, unsigned short offset, unsigned short size);
public slots:
	
};

#endif // TABLE3DDATA_H