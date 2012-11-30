#ifndef _ENGINE_COMMUNICATION_H_
#define _ENGINE_COMMUNICATION_H_

#include "src\engine.h"
#include "src\lib\dos_comm.h"
#include "src\lib\dos_gfx.h"
#include "src\lib\opl2.h"

namespace engine {
		//engine_message do_communication();
		engine_message do_communication(byte input);
}

#endif/*_ENGINE_COMMUNICATION_H_*/