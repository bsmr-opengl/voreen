/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2010 The Voreen Team. <http://www.voreen.org>   *
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

#ifdef WIN32
#include <windows.h>
#endif

#include "voreen/core/voreenapplication.h"
#include "voreen/core/processors/processorfactory.h"
#include "voreen/modules/moduleregistration.h"
#include "tgt/init.h"

using namespace voreen;

int main(int argc, char** argv) {
    VoreenApplication app("simple-memcheck", "simple-memcheck", argc, argv,
                          (VoreenApplication::ApplicationType)
                          (VoreenApplication::APP_DEFAULT & ~VoreenApplication::APP_PYTHON));
    app.init();
    addAllModules(&app);

    ProcessorFactory::getInstance();
    ProcessorFactory::destroy();
    tgt::deinit();
    return 0;
}
