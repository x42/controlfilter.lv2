/* control value scale
 *
 * Copyright (C) 2015 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#define CSC_URI "http://gareus.org/oss/lv2/cs"

typedef enum {
	CSC_CIN,
	CSC_COUT,
	CSC_MULT,
	CSC_ADD,
} PortIndex;

typedef struct {
	/* control ports */
	float* c_in;
	float* c_out;
	/* modifiers */
	float* m_mult;
	float* m_add;
} ControValueScale;


#define SMOOTHGAIN(AMP, TARGET_AMP) (AMP + (TARGET_AMP - AMP) * (float) MIN(pos, fade_len) / (float)fade_len)

static void
run(LV2_Handle instance, uint32_t n_samples)
{
	ControValueScale* self = (ControValueScale*)instance;
	*self->c_out = *self->c_in * *self->m_mult + *self->m_add;
}

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
	ControValueScale* self = (ControValueScale*)calloc (1, sizeof(ControValueScale));
	if (!self) return NULL;
	return (LV2_Handle)self;
}

static void
connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
	ControValueScale* self = (ControValueScale*)instance;

	switch ((PortIndex)port) {
		case CSC_CIN:  self->c_in = data; break;
		case CSC_COUT: self->c_out = data; break;
		case CSC_MULT: self->m_mult = data; break;
		case CSC_ADD:  self->m_add = data; break;
	}
}

static void
cleanup(LV2_Handle instance)
{
	free(instance);
}

const void*
extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
	CSC_URI,
	instantiate,
	connect_port,
	NULL,
	run,
	NULL,
	cleanup,
	extension_data
};

#undef LV2_SYMBOL_EXPORT
#ifdef _WIN32
#    define LV2_SYMBOL_EXPORT __declspec(dllexport)
#else
#    define LV2_SYMBOL_EXPORT  __attribute__ ((visibility ("default")))
#endif
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	switch (index) {
	case 0:
		return &descriptor;
	default:
		return NULL;
	}
}
