/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2008 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#ifndef VRN_COMMANDS_CONVERT_H
#define VRN_COMMANDS_CONVERT_H 

#ifdef VRN_WITH_DEVIL

#include "command.h"

namespace voreen { 

class CommandStackImg : public Command {
public:
    CommandStackImg();
    bool execute(const std::vector<std::string>& parameters);
};

class CommandStackRaw : public Command {
public:
    CommandStackRaw();
    bool execute(const std::vector<std::string>& parameters);
};

class CommandConvert : public Command {
public:
    CommandConvert();
    bool execute(const std::vector<std::string>& parameters);
};

}   //namespace voreen

#endif // VRN_WITH_DEVIL

#endif //VRN_COMMANDS_CONVERT_H
