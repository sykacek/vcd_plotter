#ifndef __VCD_H__
#define __VCD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STEP		(2)


/* VCD SIGNAL STRUCT */
typedef struct {
	char type[32];	// wire / reg / integer
	int count;		// how many signals
	char id;		// character identifier
	char name[64];	// name
	size_t tlen;	// dynamic size
	size_t vlen;	// dynamic size
	float *time;		// dynamic time array
	char **val;		// count * len
} vcd_sig_t;

vcd_sig_t *vcd_sig_init(void);
void vcd_sig_free(vcd_sig_t **sig);

// update values array with new values
void vcd_sig_update_values(vcd_sig_t *sig, char *val, size_t len, float time);

// for integrity duplicate previous values with time
void vcd_sig_duplicate(vcd_sig_t *sig, float time);

void vcd_sig_dump(vcd_sig_t *sig);

void vcd_sig_fprint(vcd_sig_t *sig, FILE *fd);

/* VCD MODULE STRUCT */
typedef struct {
	char id[64];
	size_t len;
	vcd_sig_t **sig; // signals
} vcd_mod_t;

vcd_mod_t *vcd_mod_init(const char *name);
void vcd_mod_free(vcd_mod_t **mod);

void vcd_mod_dump(vcd_mod_t *mod);
void vcd_mod_dump_rec(vcd_mod_t *mod); //recursive 

// print to log files in .data dir.
void vcd_mod_fprint(vcd_mod_t *mod);

// print to fd shortly
void vcd_mod_fprint_short(vcd_mod_t *mod, FILE *fd);

/* VCD GLOBAL STRUCT */
typedef struct {
	char date[64];
	char version[64];
	char timescale[64];
	char null[64]; // for discarding non relevant sections
	char file[64];
	size_t len;
	vcd_mod_t **mod; // modules
} vcd_t;

vcd_t *vcd_read(char *file);
void vcd_free(vcd_t **vcd);

// debug dumping
void vcd_dump(vcd_t *vcd);
void vcd_dump_rec(vcd_t *vcd); // recursive dump

// print signals in hierarchy
void vcd_print_list(vcd_t *vcd);

// update signal values and time
void vcd_update_sig(vcd_t *vcd, char c, char *val, size_t n, float time);

void vcd_update_sig_time(vcd_t *vcd, float time);

#endif //__VCD_H__

