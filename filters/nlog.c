#define CSC_NAME nlog

#ifdef CSC_TTF

	controlfilter:CSC_NAME
	TTL_DEFAULTDEF("Control Port Logarithm")
	, TTL_IPORTFLOAT(0, "base", "Base",  2, 100, 2.718281828)
	, TTL_IPORTFLOAT(1, "m1", "Inner Multiplicand",  -10, 10, 1)
	, TTL_IPORTFLOAT(2, "m2", "Outer Multiplicand",  -10, 10, 1)
	; rdfs:comment "Control Parameter Logarithm out = m2 * log ( abs (in * m1) ) / log (base)"
	.

#elif defined CSC_CODE

INIT_FN(CSC_NAME)
{
	;
}

PROC_FN(CSC_NAME)
{
	// Note: main function catches NaN and Inf
	P_OUT = PORT(2) * log2f (fabsf (P_IN * PORT(1))) / log2f (PORT(0));
}

#endif
