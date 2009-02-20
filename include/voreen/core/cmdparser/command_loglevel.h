/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2009 Visualization and Computer Graphics Group, *
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

#ifndef VRN_COMMAND_LOGLEVEL_H
#define VRN_COMMAND_LOGLEVEL_H

#include "voreen/core/cmdparser/command.h"
#include "tgt/logmanager.h"

namespace voreen {

/**
 * This commands controls the "verbose-sity" of the tgt Log using the LogLevel enum.
 *
 * <code>Usage: [<-d|--debuglevel> <debug|warning|info|error|fatal>]</code>
 * \sa Command, CommandlineParser
 */
class Command_LogLevel : public Command {
public:
    Command_LogLevel(tgt::LogLevel* dbgLevel);
    bool execute(const std::vector<std::string>& parameters);
    bool checkParameters(const std::vector<std::string>& parameters);

protected:
    tgt::LogLevel* dbgLevel_;
};

} // namespace

#endif // VRN_COMMAND_LOGLEVEL_H
