/* control value filter
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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#define CSC_URI "http://gareus.org/oss/lv2/controlfilter"

#define MAXPORTS 8

typedef struct _ControlFilter {
	/* i/o control ports */
	float* c_in;
	float* c_out;

	/* parameter input ports */
	float* port[MAXPORTS];
	float  port_hist [MAXPORTS];

	/* general purpose */
	float reg  [MAXPORTS];
	float memF [16];
	int   memI [16];

	/* instance specific */
	double samplerate;

	uint32_t n_samples;
	uint32_t pn_samples;

	void (*proc_fn)  (struct _ControlFilter*);
} ControlFilter;

/******************************************************************************
 * include vairant code
 */

#define CSC_CODE
#include "filters.c"
#undef CSC_CODE

static void
run(LV2_Handle instance, uint32_t n_samples)
{
	ControlFilter *self = (ControlFilter*) instance;

	self->n_samples = n_samples;

	self->proc_fn(self);

	if (isnan(*self->c_out) || isinf(*self->c_out)) {
		*self->c_out = 0;
	}

	self->pn_samples = n_samples;
	for (int i = 0 ; i < MAXPORTS ; ++i) {
		if (!self->port[i]) continue;
		self->port_hist[i] = *self->port[i];
	}
}

static LV2_Handle
instantiate(const LV2_Descriptor*         descriptor,
		double                    rate,
		const char*               bundle_path,
		const LV2_Feature* const* features)
{
	int i;
	ControlFilter* self = (ControlFilter*)calloc (1, sizeof(ControlFilter));
	if (!self) return NULL;
	self->samplerate = rate;

	if (0) ;
#define CSC_INSTANTIATE
#include "filters.c"
#undef CSC_INSTANTIATE
	else {
		fprintf(stderr, "controlfilter.lv2 error: unsupported plugin function.\n");
		free(self);
		return NULL;
	}

	for (i = 0; i < MAXPORTS; ++i) {
		self->port_hist[i] = 0;
	}

	return (LV2_Handle)self;
}

static void
connect_port(LV2_Handle    instance,
		uint32_t   port,
		void*      data)
{
	ControlFilter* self = (ControlFilter*)instance;

	switch (port) {
		case 0:
			self->c_in = (float*)data;
			break;
		case 1:
			self->c_out = (float*)data;
			break;
		default:
			if (port < 2 + MAXPORTS) {
				self->port[port - 2] = (float*)data;
			}
			break;
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

#define CSC_DESCRIPTOR(ID, URLSUFFIX) \
	static const LV2_Descriptor descriptor ## ID = { \
		CSC_URI "#" URLSUFFIX, \
		instantiate, \
		connect_port, \
		NULL, \
		run, \
		NULL, \
		cleanup, \
		extension_data \
	};


#define CSC_DESC
#include "filters.c"
#undef CSC_DESC

#define LV2DESC(ID) \
	case ID: return &(descriptor ## ID);


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
	LOOP_DESC(LV2DESC)
	default: return NULL;
	}
}
/* vi:set ts=8 sts=8 sw=8: */
