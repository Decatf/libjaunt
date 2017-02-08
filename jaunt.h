/*
 * libjaunt
 * Copyright (C) 2017 <decatf@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __JAUNT_H__
#define __JAUNT_H__

#include <sys/user.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define VFP_MAGIC		0x56465001

struct vfp_sigframe
{
	unsigned long		magic;
	unsigned long		size;
	struct user_vfp		ufp;
	struct user_vfp_exc	ufp_exc;
} __attribute__((__aligned__(8)));

#endif /* __JAUNT_H__ */
