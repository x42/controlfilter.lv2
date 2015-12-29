#define CSC_NAME lowpass

#ifdef CSC_TTF

	controlfilter:CSC_NAME
	TTL_DEFAULTDEF("Control Low Pass")
	, TTL_IPORT(0, "attack",  "Attack",  0.1, 50, 1, units:unit units:hz ; lv2:portProperty pprop:logarithmic; )
	, TTL_IPORT(1, "release", "Release", 0.1, 50, 1, units:unit units:hz ; lv2:portProperty pprop:logarithmic; )
	; rdfs:comment "Control Parameter Low Pass Filter"
	.

#elif defined CSC_CODE

INIT_FN(CSC_NAME)
{
	self->reg[0]  = 0; // smoothed output
	self->memF[0] = .1; // time constant for attack
	self->memF[1] = .1; // time constant for release
}

PROC_FN(CSC_NAME)
{
	const float in = P_IN;

	// update time constants
	if (PORT_CHANGED(0) || PERIOD_CHANGED) {
		const float rate = self->samplerate / self->n_samples;
		float freq = PORT(0);
		if (freq < .1 ) { freq = .1; }
		if (freq > rate * .49 ) { freq = rate * .49; }
		self->memF[0] = 1.0f - expf (-2.0 * M_PI * freq / rate);
	}

	if (PORT_CHANGED(1) || PERIOD_CHANGED) {
		const float rate = self->samplerate / self->n_samples;
		float freq = PORT(1);
		if (freq < .1 ) { freq = .1; }
		if (freq > rate * .49 ) { freq = rate * .49; }
		self->memF[1] = 1.0f - expf (-2.0 * M_PI * freq / rate);
	}

	// low pass filter
	if (fabsf(in) > fabsf(self->reg[0])) {
		self->reg[0] += self->memF[0] * (in - self->reg[0]);
	} else {
		self->reg[0] += self->memF[1] * (in - self->reg[0]);
	}

	P_OUT = self->reg[0];
}

#endif
