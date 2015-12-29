#define CSC_NAME invert

#ifdef CSC_TTF

	controlfilter:CSC_NAME
	TTL_DEFAULTDEF("Control Linear Scale")
	; rdfs:comment "Control Parameter Inverter out = 1 / in"
	.

#elif defined CSC_CODE

INIT_FN(CSC_NAME)
{
	;
}

PROC_FN(CSC_NAME)
{
	P_OUT = 1.0 / P_IN;
}

#endif
