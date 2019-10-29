#pragma once
#include <stdio.h>
#include "stm32f1xx_hal.h"

#include "gfx.h"

#include "UartLogger.h"

struct application {
	void(*init)(void);			
	void(*poll)(void);			
};

extern const struct application Application;