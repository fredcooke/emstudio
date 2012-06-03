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

#include "commands.h"

CharCommand::CharCommand(XByteArray * xData, Cmd cmd, int charPos, char newChar, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _xData = xData;
    _charPos = charPos;
    _newChar = newChar;
    _cmd = cmd;
}

bool CharCommand::mergeWith(const QUndoCommand *command)
{
    const CharCommand *nextCommand = static_cast<const CharCommand *>(command);
    bool result = false;

    if (_cmd != remove)
    {
        if (nextCommand->_cmd == replace)
            if (nextCommand->_charPos == _charPos)
            {
                _newChar = nextCommand->_newChar;
                result = true;
            }
    }
    return result;
}

void CharCommand::undo()
{
    switch (_cmd)
    {
        case insert:
            _xData->remove(_charPos, 1);
            break;
        case replace:
            _xData->replace(_charPos, _oldChar);
            _xData->setDataChanged(_charPos, _wasChanged);
            break;
        case remove:
            _xData->insert(_charPos, _oldChar);
            _xData->setDataChanged(_charPos, _wasChanged);
            break;
    }
}

void CharCommand::redo()
{
    switch (_cmd)
    {
        case insert:
            _xData->insert(_charPos, _newChar);
            break;
        case replace:
            _oldChar = _xData->data()[_charPos];
            _wasChanged = _xData->dataChanged(_charPos);
            _xData->replace(_charPos, _newChar);
            break;
        case remove:
            _oldChar = _xData->data()[_charPos];
            _wasChanged = _xData->dataChanged(_charPos);
            _xData->remove(_charPos, 1);
            break;
    }
}



ArrayCommand::ArrayCommand(XByteArray * xData, Cmd cmd, int baPos, QByteArray newBa, int len, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    _cmd = cmd;
    _xData = xData;
    _baPos = baPos;
    _newBa = newBa;
    _len = len;
}

void ArrayCommand::undo()
{
    switch (_cmd)
    {
        case insert:
            _xData->remove(_baPos, _newBa.length());
            break;
        case replace:
            _xData->replace(_baPos, _oldBa);
            _xData->setDataChanged(_baPos, _wasChanged);
            break;
        case remove:
            _xData->insert(_baPos, _oldBa);
            _xData->setDataChanged(_baPos, _wasChanged);
            break;
    }
}

void ArrayCommand::redo()
{
    switch (_cmd)
    {
        case insert:
            _xData->insert(_baPos, _newBa);
            break;
        case replace:
            _oldBa = _xData->data().mid(_baPos, _len);
            _wasChanged = _xData->dataChanged(_baPos, _len);
            _xData->replace(_baPos, _newBa);
            break;
        case remove:
            _oldBa = _xData->data().mid(_baPos, _len);
            _wasChanged = _xData->dataChanged(_baPos, _len);
            _xData->remove(_baPos, _len);
            break;
    }
}
