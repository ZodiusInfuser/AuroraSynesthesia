#pragma once


//DEFINITIONS

#define	PROGRAM_NAME		"Aurora Synesthesia"
#define PROGRAM_VERSION		"1.06"

#define NULL	0


#define TRAY_NOTIFY			(WM_USER + 1)



#define DECIBEL_DROP		-48.0f


//MACROS

#define CLAMP(a, min, max)		(((a) <= (max)) ? (((a) >= (min)) ? (a) : (min)) : (max))

#define MIN3(x,y,z)  ((y) <= (z) ? \
                     ((x) <= (y) ? (x) : (y)) \
                     : \
                     ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                     ((x) >= (y) ? (x) : (y)) \
                     : \
                     ((x) >= (z) ? (x) : (z)))

#define LOGSCALE(x)	(((20.0f * (float)log10(x)) < DECIBEL_DROP) ? \
					(0.0f) : \
					(1.0f - ((20.0f * (float)log10(x)) / DECIBEL_DROP)))