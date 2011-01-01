/*
	Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

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

#include "pal_ticketlock.h"
#include "pal_platform.h"

/* The "Ticketlock":

Initialization:
users = 0;
ticket = 0;

Lock:
me = users;
users++;
while (ticket != me) spin();

Unlock:
ticket++;

When the lock is held, users != ticket
When the lock is available, users == ticket
*/

void palTicketlockInit(palTicketlock* t) {
	t->u = 0;
	__sync_synchronize();
}

void palTicketlockLock(palTicketlock* t) {
	unsigned short me = __sync_fetch_and_add(&t->s.users, 1);
	while (t->s.ticket != me) { cpu_relax(); };
}

void palTicketlockUnlock(palTicketlock* t) {
	t->s.ticket++;
}

int  palTicketlockTryLock(palTicketlock* t) {
	unsigned short me = t->s.users;
	unsigned short menew = me + 1;
	/* (ticket << 16) + users */
	unsigned cmp = ((unsigned) me << 16) + me;
	unsigned cmpnew = ((unsigned) me << 16) + menew;

	if (__sync_bool_compare_and_swap(&t->u, cmp, cmpnew))
	{
		return 0; // locked
	} else {
		return 1; // try again
	}
}

int  palTicketlockLockable(palTicketlock* t) {
	palTicketlock u = *t;
	__sync_synchronize();
	return (u.s.ticket == u.s.users);
}

int palTicketlockLocked(palTicketlock* t) {
	palTicketlock u = *t;
	__sync_synchronize();
	return (u.s.ticket != u.s.users);
}