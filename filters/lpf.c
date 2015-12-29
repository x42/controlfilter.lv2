#define CSC_NAME lowpass

#ifdef CSC_TTF

	controlfilter:CSC_NAME
	TTL_DEFAULTDEF("Control Low Pass")
	, TTL_IPORT(0, "attack",  "Attack",  0.1, 50, 1, units:unit units:hz ; lv2:portProperty pprop:logarithmic; )
	, TTL_IPORT(1, "release", "Release", 0.1, 50, 1, units:unit units:hz ; lv2:portProperty pprop:logarithmic; )
	; rdfs:comment "Control Parameter Low Pass Filter"
	.

#elif defined CSC_CODE

INIT_FN(CSC_NAME) (ControlFilter *self)
{
	self->reg[0] = 0; // smoothed output
	self->memF[0] = .1; // coeff for attack
	self->memF[1] = .1; // coeff for release
}

PROC_FN(CSC_NAME) (ControlFilter *self)
{
	const float in = *self->c_in;

	if (*self->port[0] != self->port_hist[0] || self->n_samples != self->pn_samples) {
		float freq = *self->port[0];
		float rate = self->samplerate / self->n_samples;
		if (freq < .1 ) freq = .1;
		if (freq > self->samplerate * .4 ) freq = self->samplerate * .4;
		 self->memF[0] = 1.0f - expf (-2.0 * M_PI * freq / rate);
	}

	if (*self->port[1] != self->port_hist[1] || self->n_samples != self->pn_samples) {
		float freq = *self->port[1];
		float rate = self->samplerate / self->n_samples;
		if (freq < .1 ) freq = .1;
		if (freq > self->samplerate * .4 ) freq = self->samplerate * .4;
		 self->memF[1] = 1.0f - expf (-2.0 * M_PI * freq / rate);
	}

	if (fabsf(in) > fabsf(self->reg[0])) {
		self->reg[0] += self->memF[0] * (in - self->reg[0]);
	} else {
		self->reg[0] += self->memF[1] * (in - self->reg[0]);
	}

	*self->c_out = self->reg[0];
}

#endif
