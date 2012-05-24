/***************************************************************************
*                                                                          *
*   This file is a part of QHexEdit                                        *
*                                                                          *
*   QHexEdit is free software: you can redistribute it and/or modify       *
*   it under the terms of the GNU Lesser General Public License version    *
*   2.1 as published by the Free Software Foundation, or (at your option)  *
*   any later version.                                                     *
*                                                                          *
*   QHexEdit is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU Lesser General Public License for more details.                    *
									   *
*   You should have received a copy of the GNU Lesser General Public       *
*   License along with this program.  If not, see                          *
*   <http://www.gnu.org/licenses/>.                                        *
****************************************************************************/

#ifndef XBYTEARRAY_H
#define XBYTEARRAY_H

/** \cond docNever */

#include <QtCore>

/*! XByteArray represents the content of QHexEcit.
XByteArray comprehend the data itself and informations to store if it was
changed. The QHexEdit component uses these informations to perform nice
rendering of the data

XByteArray also provides some functionality to insert, replace and remove
single chars and QByteArras. Additionally some functions support rendering
and converting to readable strings.
*/
class XByteArray
{
public:
    explicit XByteArray();

    int addressOffset();
    void setAddressOffset(int offset);

    int addressWidth();
    void setAddressWidth(int width);

    QByteArray & data();
    void setData(QByteArray data);

    bool dataChanged(int i);
    QByteArray dataChanged(int i, int len);
    void setDataChanged(int i, bool state);
    void setDataChanged(int i, const QByteArray & state);

    int realAddressNumbers();
    int size();

    QByteArray & insert(int i, char ch);
    QByteArray & insert(int i, const QByteArray & ba);

    QByteArray & remove(int pos, int len);

    QByteArray & replace(int index, char ch);
    QByteArray & replace(int index, const QByteArray & ba);
    QByteArray & replace(int index, int length, const QByteArray & ba);

    QChar asciiChar(int index);
    QString toRedableString(int start=0, int end=-1);

signals:

public slots:

private:
    QByteArray _data;
    QByteArray _changedData;

    int _addressNumbers;                    // wanted width of address area
    int _addressOffset;                     // will be added to the real addres inside bytearray
    int _realAddressNumbers;                // real width of address area (can be greater then wanted width)
    int _oldSize;                           // size of data
};

/** \endcond docNever */
#endif // XBYTEARRAY_H
