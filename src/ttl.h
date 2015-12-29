#ifndef _TTL_H_
#define _TTL_H_

/* cfg-port offsets 0,1, are control i/o ports, . */
#define ADDP_0 2
#define ADDP_1 3
#define ADDP_2 4
#define ADDP_3 5
#define ADDP_4 6
#define ADDP_5 7
#define ADDP_6 8
#define ADDP_7 9

#define PORTIDX(x) ADDP_##x

#define MAINTAINER <HTTPP/gareus.org/rgareus#me>

#define TTL_DEF(DOAPNAME, INSUPPORTS) \
	a lv2:Plugin, lv2:UtilityPlugin ; \
	doap:name DOAPNAME ; \
	@VERSION@ \
	doap:license <HTTPP/usefulinc.com/doap/licenses/gpl> ; \
  lv2:project <HTTPP/gareus.org/oss/lv2/controlfilter> ; \
	lv2:optionalFeature lv2:hardRTCapable ; \
	lv2:port \
	[ \
		a lv2:InputPort , \
			lv2:ControlPort ; \
		lv2:index 0 ; \
		lv2:symbol "in" ; \
		lv2:name "Control Input" \
		lv2:default 0.0 ; \
		lv2:minimum -1.0 ; \
		lv2:maximum 1.0 ; \
	] , [ \
		a lv2:OutputPort , \
			lv2:ControlPort ; \
		lv2:index 1 ; \
		lv2:symbol "out" ; \
		lv2:name "Control Output"; \
		lv2:minimum -1.0 ; \
		lv2:maximum  1.0 ; \
	]

#define TTL_DEFAULTDEF(DOAPNAME) TTL_DEF(DOAPNAME,)

#define TTL_PORT(TYPE, IDX, SYM, DESC, VMIN, VMAX, VDFLT, ATTR) \
	[ \
    a TYPE, \
      lv2:ControlPort ; \
    lv2:index PORTIDX(IDX) ; \
    lv2:symbol SYM ; \
    lv2:name DESC; \
    lv2:minimum VMIN ; \
    lv2:maximum VMAX ; \
    lv2:default VDFLT; \
    ATTR \
  ]

#define TTL_IPORT(IDX, SYM, DESC, VMIN, VMAX, VDFLT, ATTR) \
	TTL_PORT(lv2:InputPort, IDX, SYM, DESC, VMIN, VMAX, VDFLT, ATTR)

#define TTL_IPORTFLOAT(IDX, SYM, DESC, VMIN, VMAX, VDFLT) \
	TTL_IPORT(IDX, SYM, DESC, VMIN, VMAX, VDFLT, )

#define TTL_IPORTINT(IDX, SYM, DESC, VMIN, VMAX, VDFLT) \
	TTL_IPORT(IDX, SYM, DESC, VMIN, VMAX, VDFLT, lv2:portProperty lv2:integer)

#define TTL_IPORTTOGGLE(IDX, SYM, DESC, VDFLT) \
	TTL_IPORT(IDX, SYM, DESC, 0, 1, VDFLT, lv2:portProperty lv2:integer; lv2:portProperty lv2:toggled)

#define XINIT_FN(NAME) static void flt_init_ ## NAME (ControlFilter *self)
#define XPROC_FN(NAME) static void flt_proc_ ## NAME (ControlFilter *self)
#endif

/* variable part */

#ifdef CSC_FLT
#undef CSC_FLT
#endif

#ifdef CSC_INSTANTIATE
#define CSC_FLT(ID, FNX) \
	else if (!strcmp(descriptor->URI, CSC_URI "#" # FNX)) { self->proc_fn = flt_proc_ ## FNX; flt_init_ ## FNX(self); }

#elif (defined CSC_DESC)

#define CSC_FLT(ID, FNX) \
	CSC_DESCRIPTOR(ID, "" # FNX)

#elif (defined CSC_MANIFEST)

#define CSC_FLT(ID, FNX) \
  <HTTPP/gareus.org/oss/lv2/controlfilterHASH ## FNX> \
	a lv2:Plugin ; \
	lv2:binary <@LV2NAME@@LIB_EXT@>  ; \
	rdfs:seeAlso <@LV2NAME@.ttl> . \

#else

#define P_IN (*self->c_in)
#define P_OUT (*self->c_out)

#define PORT(N) (*self->port[N])
#define PORT_CHANGED(N) (*self->port[N] != self->port_hist[N])
#define PERIOD_CHANGED (self->n_samples != self->pn_samples)

#define CSC_FLT(ID, FNX)

#endif
