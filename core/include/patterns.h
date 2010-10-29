#pragma once

// Define the 7-point 3d pattern.
#define _C(var)	(*(var))				// pattern var		(i,	j,	k    )
#define _L(var)	(*((var) - 1))				// pattern Left		(i - 1,	j,	k    )	
#define _R(var)	(*((var) + 1))				// pattern Right	(i + 1,	j,	k    )
#define _U(var)	(*((var) + var##_NX))			// pattern Upper	(i,	j + 1,	k    )	
#define _D(var)	(*((var) - var##_NX))			// pattern Lower	(i,	j - 1,	k    )
#define _F(var)	(*((var) + var##_NP))			// pattern Front	(i,	j,	k + 1)
#define _B(var)	(*((var) - var##_NP))			// pattern Back		(i,	j,	k - 1)

// Define the 15-point 3d pattern additional points.
#define _RD(var) (*((var) - var##_NX + 1))		// pattern Right Lower	(i + 1,	j - 1,	k    )
#define _RB(var) (*((var) - var##_NP + 1))		// pattern Right Back	(i + 1, j,    	k - 1)
#define _LU(var) (*((var) + var##_NX - 1))		// pattern Left Upper	(i - 1, j + 1,	k    )
#define _LF(var) (*((var) + var##_NP - 1))		// pattern Left Front	(i - 1,	j,	k + 1)
#define _UB(var) (*((var) - var##_NP + var##_NX))	// pattern Upper Back	(i,	j + 1,	k - 1)
#define _DF(var) (*((var) + var##_NP - var##_NX))	// pattern Lower Front	(i,	j - 1,	k + 1)

// Define the 18-point 3d pattern additional points.
#define _RU(var) (*((var) + var##_NX + 1))		// pattern Right Upper	(i + 1, j + 1,	k    )
#define _RF(var) (*((var) + var##_NP + 1))		// pattern Right Front	(i + 1, j,	k + 1)
#define _UF(var) (*((var) + var##_NP + var##_NX))	// pattern Upper Front	(i,	j + 1, 	k + 1)
