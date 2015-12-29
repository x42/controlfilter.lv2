#define CSC_NAME linearscale

#ifdef CSC_TTF

	controlfilter:CSC_NAME
	TTL_DEFAULTDEF("Control Linear Scale")
	, TTL_IPORTFLOAT(0, "mult", "Multiplier",  -10, 10, 1)
	, TTL_IPORTFLOAT(1, "add",  "Offset",  -10, 10, 0)
	; rdfs:comment "Control Parameter Scale/Offset out = in * Multiplier + Offset"
	.

#elif defined CSC_CODE

INIT_FN(CSC_NAME) (ControlFilter *self)
{
	;
}

PROC_FN(CSC_NAME) (ControlFilter *self)
{
	*self->c_out = *self->c_in * (*self->port[0]) + (*self->port[1]);
}

#endif
