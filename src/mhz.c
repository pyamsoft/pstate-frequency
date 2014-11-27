/* #define _GNU_SOURCE */
/* #include <src/mhz.h> */
/* #include <sys/time.h> */
/* #include <string.h> */
/* #include <unistd.h> */
/* #include <stdint.h> */

/* // copied from the i7z project, */ 
/* // https://github.com/ajaiantilal/i7z/blob/master/helper_functions.c */
/* // which was in turn copied from the link below */
/* //copied blantantly from http://www.cs.helsinki.fi/linux/linux-kernel/2001-37/0256.html */

/* __inline__ unsigned long long int rdtsc() */
/* { */
/*         unsigned long long int x; */
/*         __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x)); */
/*         return x; */
/* } */

/* double estimate_KHz() */
/* { */
/*         return estimate_MHz() * 1000; */
/* } */

/* double estimate_MHz() */
/* { */
/*         //copied blantantly from http://www.cs.helsinki.fi/linux/linux-kernel/2001-37/0256.html */
/*         * $Id: MHz.c,v 1.4 2001/05/21 18:58:01 davej Exp $ */
/*         * This file is part of x86info. */
/*         * (C) 2001 Dave Jones. */
/*         * */
/*         * Licensed under the terms of the GNU GPL License version 2. */
/*         * */
/*         * Estimate CPU MHz routine by Andrea Arcangeli <andrea@suse.de> */
/*         * Small changes by David Sterba <sterd9am@ss1000.ms.mff.cuni.cz> */
/*         * */
/*         *1/ */
/*         struct timezone tz; */
/*         struct timeval tvstart, tvstop; */
/*         unsigned long long int first_cycle, second_cycle; */
/*         unsigned long long int microseconds; */

/*         memset(&tz, 0, sizeof (tz)); */

/*         gettimeofday(&tvstart, &tz); */
/*         first_cycle = rdtsc(); */
/*         gettimeofday(&tvstart, &tz); */
/*         usleep(10000); */
/*         second_cycle = rdtsc (); */
/*         gettimeofday(&tvstop, &tz); */
/*         microseconds = ((tvstop.tv_sec - tvstart.tv_sec) * 1000000) + */
/*                         (tvstop.tv_usec - tvstart.tv_usec); */
/*         unsigned long long int elapsed; */
/*         if (second_cycle < first_cycle) { */
/*                 elapsed = UINT32_MAX - first_cycle; */
/*                 elapsed += second_cycle; */
/*         } else { */
/*                 elapsed = second_cycle - first_cycle; */
/*         } */
/*         return elapsed / microseconds; */
/* } */
