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
		.in = "test.vcd",
		.out = "test.svg",
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
		vcd_print_short(vcd);
	} 

	if(par.module)
		vcd_print_module(vcd, par.module_i);

	if(par.signal)
		vcd_sig_print(vcd->mod[par.x]->sig[par.y]);

	vcd_free(&vcd);

	return 0;
}
