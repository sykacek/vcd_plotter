#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "param.h"
#include "vcd.h"
#include "svg.h"

/* 
 * Error codes:
 *
 * 100 - VCD error
 *
 */

static struct stat st = {0};

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}

int rmrf(char *path)
{
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

/*
 * MAIN FUNCTION
 */
int main(int argc, char **argv){
	param_t par = {
		.ok = false,
	};

	param_read(&par, argc, argv);

	if(par.ok == false){
		param_help();
		return 0;
	}

	vcd_t *vcd = vcd_read(par.in);
	if(!vcd)
		return 100;

	if(par.list){
		vcd_print_list(vcd);
	} 

	FILE *fd = stdout;

	if(par.fd){
		fd = fopen(par.out, "w");
	}

	// print whole module
	if(par.module){
		if(par.sh)
			vcd_mod_fprint_short(vcd->mod[par.module_i], fd);
		else {
			if (stat(".data", &st) == -1) {
				mkdir(".data", 0744);
			} else {
				rmrf(".data");
				mkdir(".data", 0744);
			}

			vcd_mod_fprint(vcd->mod[par.module_i]);
		}
	}

	// print only signal
	if(par.signal)
		vcd_sig_fprint(vcd->mod[par.x]->sig[par.y], fd);

	vcd_free(&vcd);

	if(par.fd){
		fclose(fd);
	}

	return 0;
}
