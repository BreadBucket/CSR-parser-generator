#pragma once


// ---------------------------------- [ Definitions ] --------------------------------------- //


#define SIZEOF_ARRAY(a)				(sizeof(a)/sizeof(*a))

#define VALID_INDEX(i, length)		(0 <= (int)(i) && (int)(i) < (int)(length))

#define VALID_ARRAY_INDEX(i, a)		VALID_INDEX(i, SIZEOF_ARRAY(a))

#define BETWEEN(n, min, max)		((min) <= (n) && (n) <= (max))

#define WITHIN(n, min, max)			((min) < (n) && (n) < (max))

#define CLAMP(n, min, max)			((n) <= (min) ? (min) : ((n) >= (max) ? (max) : (n)))


// ------------------------------------------------------------------------------------------ //