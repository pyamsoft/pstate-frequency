/*
    pstate_frequency Easier control of the Intel p-state driver

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    For questions please contact P.Yam Software at pyam.soft@gmail.com

*/

#include <src/mhz.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

// copied from the i7z project, 
// https://github.com/ajaiantilal/i7z/blob/master/helper_functions.c
// which was in turn copied from the link below
//copied blantantly from http://www.cs.helsinki.fi/linux/linux-kernel/2001-37/0256.html

__inline__ unsigned long long int
rdtsc()
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

double
estimate_MHz()
{
    //copied blantantly from http://www.cs.helsinki.fi/linux/linux-kernel/2001-37/0256.html
    /*
    * $Id: MHz.c,v 1.4 2001/05/21 18:58:01 davej Exp $
    * This file is part of x86info.
    * (C) 2001 Dave Jones.
    *
    * Licensed under the terms of the GNU GPL License version 2.
    *
    * Estimate CPU MHz routine by Andrea Arcangeli <andrea@suse.de>
    * Small changes by David Sterba <sterd9am@ss1000.ms.mff.cuni.cz>
    *
    */
    struct timezone tz;
    struct timeval tvstart, tvstop;
    unsigned long long int cycles[2];       /* must be 64 bit */
    unsigned long long int microseconds;    /* total time taken */

    memset(&tz, 0, sizeof (tz));

    /* get this function in cached memory */
    gettimeofday(&tvstart, &tz);
    cycles[0] = rdtsc();
    gettimeofday(&tvstart, &tz);

    /* we don't trust that this is any specific length of time */
    /*1 sec will cause rdtsc to overlap multiple times perhaps. 100msecs is a good spot */
    usleep(10000);

    cycles[1] = rdtsc ();
    gettimeofday(&tvstop, &tz);
    microseconds = ((tvstop.tv_sec - tvstart.tv_sec) * 1000000) +
                   (tvstop.tv_usec - tvstart.tv_usec);

    unsigned long long int elapsed = 0;
    if (cycles[1] < cycles[0])
    {
        //printf("c0 = %llu   c1 = %llu",cycles[0],cycles[1]);
        elapsed = UINT32_MAX - cycles[0];
        elapsed = elapsed + cycles[1];
        //printf("c0 = %llu  c1 = %llu max = %llu elapsed=%llu\n",cycles[0], cycles[1], UINT32_MAX,elapsed);
    }
    else
    {
        elapsed = cycles[1] - cycles[0];
        //printf("\nc0 = %llu  c1 = %llu elapsed=%llu\n",cycles[0], cycles[1],elapsed);
    }

    //printf("%llg MHz processor (estimate).  diff cycles=%llu  microseconds=%llu \n", mhz, elapsed, microseconds);
    //printf("%g  elapsed %llu  microseconds %llu\n",mhz, elapsed, microseconds);
    return elapsed / microseconds;
}
