#define CSC_NAME exp

#ifdef CSC_TTF

	controlfilter:CSC_NAME
	TTL_DEFAULTDEF("Control Port Exponental")
	, TTL_IPORTFLOAT(0, "base", "base",  -10, 10, 2.718281828)
	; rdfs:comment "Control Parameter Exponential out = base ^ in"
	.

#elif defined CSC_CODE

INIT_FN(CSC_NAME) (ControlFilter *self)
{
	;
}

PROC_FN(CSC_NAME) (ControlFilter *self)
{
	P_OUT = powf (PORT(0), P_IN);
}

#endif
