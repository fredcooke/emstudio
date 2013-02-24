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

#include "datafield.h"
#include <QDebug>
DataField::DataField()
{
}
DataField::DataField(QString shortname,QString description,int offset,int size,double div,double addoffset,double min,double max,bool isFlags,int bit)
{
	m_offset = offset;
	m_size = size;
	m_div = div;
	m_name = shortname;
	m_description = description;
	m_min = min;
	m_max = max;
	m_addoffset = addoffset;
	m_isFlags = isFlags;
	m_bit = bit;
}
bool DataField::flagValue(QByteArray *payload)
{
	if (!m_isFlags)
	{
		return false;
	}
	if (payload->size() > m_offset+m_size)
	{
		unsigned int val = 0;
		for (int i=0;i<m_size;i++)
		{
			val += ((unsigned char)payload->at(m_offset+i)) << (8*(m_size-(i+1)));
		}
		return (m_bit & val);
	}
	return false;
}

float DataField::getValue(QByteArray *payload)
{
	if (payload->size() > m_offset+m_size)
	{
		float val = 0;
		for (int i=0;i<m_size;i++)
		{
			val += ((unsigned char)payload->at(m_offset+i)) << (8*(m_size-(i+1)));
		}
		return (val / m_div) + m_addoffset;
	}
	return 0;
}
