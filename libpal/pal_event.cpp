/*
	Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#include "libpal/pal_event.h"

/* Configure the pool allocator for event delegates */
#define kPalEventDelegateMax 4096
#define kPalEventDelegatePoolSize palEventDelegateList::element_size*kPalEventDelegateMax

static unsigned char* event_delegate_pool_memory[kPalEventDelegatePoolSize];

palPoolAllocator defualt_event_delegate_pool_allocator(&event_delegate_pool_memory[0], kPalEventDelegatePoolSize, palEventDelegateList::element_size, 8);