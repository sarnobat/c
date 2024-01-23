#include "gom.h"
#include "dump_gom.h"
#include "portability.h"
#include <stdio.h>
#include <locale.h>

int main(int argc, char* argv[])
{
  Gedcom_err_mech mech = IMMED_FAIL;
  int compat_enabled = 1;
  int debug_level = 0;
  int result      = 0;
  char* outfilename = "rohidekar.ged";
  char* file_name = NULL;

  gedcom_init();
	printf("hi\n");
}