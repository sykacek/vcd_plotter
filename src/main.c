#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "param.h"
#include "vcd.h"
#include "svg.h"

/* 
 * Error codes:
 *
 * 100 - VCD error
 * 200 - SVG error
 *
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

	if(par.module){
		if(par.sh)
			vcd_mod_fprint_short(vcd->mod[par.module_i], fd);
		else
			vcd_mod_fprint(vcd->mod[par.module_i]);
	}

	if(par.signal)
		vcd_sig_fprint(vcd->mod[par.x]->sig[par.y], fd);

	vcd_free(&vcd);

	if(par.fd){
		fclose(fd);
	}

	return 0;
}
