/*****
  command line parser -- generated by clig
  (http://wsd.iitb.fhg.de/~kir/clighome/)

  The command line parser `clig':
  (C) 1995---2001 Harald Kirsch (kirschh@lionbioscience.com)
*****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "bincand_cmd.h"

char *Program;

/*@-null*/

static Cmdline cmd = {
  /***** -plo: The low pulsar period to check (s) */
  /* ploP = */ 0,
  /* plo = */ (double)0,
  /* ploC = */ 0,
  /***** -phi: The high pulsar period to check (s) */
  /* phiP = */ 0,
  /* phi = */ (double)0,
  /* phiC = */ 0,
  /***** -rlo: The low Fourier frequency bin to check */
  /* rloP = */ 0,
  /* rlo = */ (double)0,
  /* rloC = */ 0,
  /***** -rhi: The high Fourier frequency bin to check */
  /* rhiP = */ 0,
  /* rhi = */ (double)0,
  /* rhiC = */ 0,
  /***** -psr: Name of pulsar to check for (do not include J or B) */
  /* psrnameP = */ 0,
  /* psrname = */ (char*)0,
  /* psrnameC = */ 0,
  /***** -candnum: Number of the candidate to optimize from candfile. */
  /* candnumP = */ 0,
  /* candnum = */ (int)0,
  /* candnumC = */ 0,
  /***** -candfile: Name of the bincary candidate file. */
  /* candfileP = */ 0,
  /* candfile = */ (char*)0,
  /* candfileC = */ 0,
  /***** -usr: Describe your own binary candidate.  Must include all of the following (assumed) parameters */
  /* usrP = */ 0,
  /***** -pb: The orbital period (s) */
  /* pbP = */ 0,
  /* pb = */ (double)0,
  /* pbC = */ 0,
  /***** -x: The projected orbital semi-major axis (lt-sec) */
  /* asinicP = */ 0,
  /* asinic = */ (double)0,
  /* asinicC = */ 0,
  /***** -e: The orbital eccentricity */
  /* eP = */ 1,
  /* e = */ 0,
  /* eC = */ 1,
  /***** -To: The time of periastron passage (MJD) */
  /* ToP = */ 0,
  /* To = */ (double)0,
  /* ToC = */ 0,
  /***** -w: Longitude of periastron (deg) */
  /* wP = */ 0,
  /* w = */ (double)0,
  /* wC = */ 0,
  /***** -wdot: Rate of advance of periastron (deg/yr) */
  /* wdotP = */ 1,
  /* wdot = */ 0,
  /* wdotC = */ 1,
  /***** -mak: Determine optimization parameters from 'infile.mak' */
  /* makefileP = */ 0,
  /***** uninterpreted rest of command line */
  /* argc = */ 0,
  /* argv = */ (char**)0,
  /***** the original command line concatenated */
  /* full_cmd_line = */ NULL
};

/*@=null*/

/***** let LCLint run more smoothly */
/*@-predboolothers*/
/*@-boolops*/


/******************************************************************/
/*****
 This is a bit tricky. We want to make a difference between overflow
 and underflow and we want to allow v==Inf or v==-Inf but not
 v>FLT_MAX. 

 We don't use fabs to avoid linkage with -lm.
*****/
static void
checkFloatConversion(double v, char *option, char *arg)
{
  char *err = NULL;

  if( (errno==ERANGE && v!=0.0) /* even double overflowed */
      || (v<HUGE_VAL && v>-HUGE_VAL && (v<0.0?-v:v)>(double)FLT_MAX) ) {
    err = "large";
  } else if( (errno==ERANGE && v==0.0) 
	     || (v!=0.0 && (v<0.0?-v:v)<(double)FLT_MIN) ) {
    err = "small";
  }
  if( err ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to %s to represent\n",
	    Program, arg, option, err);
    exit(EXIT_FAILURE);
  }
}

int
getIntOpt(int argc, char **argv, int i, int *value, int force)
{
  char *end;
  long v;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  v = strtol(argv[i], &end, 0);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check if it fits into an int */
  if( errno==ERANGE || v>(long)INT_MAX || v<(long)INT_MIN ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to large to represent\n",
	    Program, argv[i], argv[i-1]);
    exit(EXIT_FAILURE);
  }
  *value = (int)v;

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr, 
	  "%s: missing or malformed integer value after option `%s'\n",
	  Program, argv[i-1]);
    exit(EXIT_FAILURE);
}
/**********************************************************************/

int
getIntOpts(int argc, char **argv, int i, 
	   int **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;
  long v;
  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values. It does not hurt to have room
    for a bit more values than cmax.
  *****/
  alloced = cmin + 4;
  *values = (int*)calloc((size_t)alloced, sizeof(int));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (int *) realloc(*values, alloced*sizeof(int));
      if( !*values ) goto outMem;
    }

    errno = 0;
    v = strtol(argv[used+i+1], &end, 0);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE || v>(long)INT_MAX || v<(long)INT_MIN ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to large to represent\n",
	      Program, argv[i+used+1], argv[i]);
      exit(EXIT_FAILURE);
    }

    (*values)[used] = (int)v;

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be an "
	    "integer value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getLongOpt(int argc, char **argv, int i, long *value, int force)
{
  char *end;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  *value = strtol(argv[i], &end, 0);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  if( errno==ERANGE ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to large to represent\n",
	    Program, argv[i], argv[i-1]);
    exit(EXIT_FAILURE);
  }
  return i;

nothingFound:
  /***** !force means: this parameter may be missing.*/
  if( !force ) return i-1;

  fprintf(stderr, 
	  "%s: missing or malformed value after option `%s'\n",
	  Program, argv[i-1]);
    exit(EXIT_FAILURE);
}
/**********************************************************************/

int
getLongOpts(int argc, char **argv, int i, 
	    long **values,
	    int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values. It does not hurt to have room
    for a bit more values than cmax.
  *****/
  alloced = cmin + 4;
  *values = calloc((size_t)alloced, sizeof(long));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = realloc(*values, alloced*sizeof(long));
      if( !*values ) goto outMem;
    }

    errno = 0;
    (*values)[used] = strtol(argv[used+i+1], &end, 0);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1; 
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to large to represent\n",
	      Program, argv[i+used+1], argv[i]);
      exit(EXIT_FAILURE);
    }

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be an "
	    "integer value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getFloatOpt(int argc, char **argv, int i, float *value, int force)
{
  char *end;
  double v;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  v = strtod(argv[i], &end);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  checkFloatConversion(v, argv[i-1], argv[i]);

  *value = (float)v;

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr,
	  "%s: missing or malformed float value after option `%s'\n",
	  Program, argv[i-1]);
  exit(EXIT_FAILURE);
 
}
/**********************************************************************/

int
getFloatOpts(int argc, char **argv, int i, 
	   float **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;
  double v;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values.
  *****/
  alloced = cmin + 4;
  *values = (float*)calloc((size_t)alloced, sizeof(float));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (float *) realloc(*values, alloced*sizeof(float));
      if( !*values ) goto outMem;
    }

    errno = 0;
    v = strtod(argv[used+i+1], &end);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    checkFloatConversion(v, argv[i], argv[i+used+1]);
    
    (*values)[used] = (float)v;
  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be a "
	    "floating-point value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getDoubleOpt(int argc, char **argv, int i, double *value, int force)
{
  char *end;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  *value = strtod(argv[i], &end);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  if( errno==ERANGE ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to %s to represent\n",
	    Program, argv[i], argv[i-1],
	    (*value==0.0 ? "small" : "large"));
    exit(EXIT_FAILURE);
  }

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr,
	  "%s: missing or malformed value after option `%s'\n",
	  Program, argv[i-1]);
  exit(EXIT_FAILURE);
 
}
/**********************************************************************/

int
getDoubleOpts(int argc, char **argv, int i, 
	   double **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values.
  *****/
  alloced = cmin + 4;
  *values = (double*)calloc((size_t)alloced, sizeof(double));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (double *) realloc(*values, alloced*sizeof(double));
      if( !*values ) goto outMem;
    }

    errno = 0;
    (*values)[used] = strtod(argv[used+i+1], &end);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to %s to represent\n",
	      Program, argv[i+used+1], argv[i],
	      ((*values)[used]==0.0 ? "small" : "large"));
      exit(EXIT_FAILURE);
    }

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be a "
	    "double value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

/**
  force will be set if we need at least one argument for the option.
*****/
int
getStringOpt(int argc, char **argv, int i, char **value, int force)
{
  i += 1;
  if( i>=argc ) {
    if( force ) {
      fprintf(stderr, "%s: missing string after option `%s'\n",
	      Program, argv[i-1]);
      exit(EXIT_FAILURE);
    } 
    return i-1;
  }
  
  if( !force && argv[i][0] == '-' ) return i-1;
  *value = argv[i];
  return i;
}
/**********************************************************************/

int
getStringOpts(int argc, char **argv, int i, 
	   char*  **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  alloced = cmin + 4;
    
  *values = (char**)calloc((size_t)alloced, sizeof(char*));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory during parsing of option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (char **)realloc(*values, alloced*sizeof(char*));
      if( !*values ) goto outMem;
    }

    if( used>=cmin && argv[used+i+1][0]=='-' ) break;
    (*values)[used] = argv[used+i+1];
  }
    
  if( used<cmin ) {
    fprintf(stderr, 
    "%s: less than %d parameters for option `%s', only %d found\n",
	    Program, cmin, argv[i], used);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

void
checkIntLower(char *opt, int *values, int count, int max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%d\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkIntHigher(char *opt, int *values, int count, int min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%d\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkLongLower(char *opt, long *values, int count, long max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%ld\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkLongHigher(char *opt, long *values, int count, long min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%ld\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkFloatLower(char *opt, float *values, int count, float max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%f\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkFloatHigher(char *opt, float *values, int count, float min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%f\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkDoubleLower(char *opt, double *values, int count, double max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%f\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkDoubleHigher(char *opt, double *values, int count, double min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%f\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

static char *
catArgv(int argc, char **argv)
{
  int i;
  size_t l;
  char *s, *t;

  for(i=0, l=0; i<argc; i++) l += (1+strlen(argv[i]));
  s = (char *)malloc(l);
  if( !s ) {
    fprintf(stderr, "%s: out of memory\n", Program);
    exit(EXIT_FAILURE);
  }
  strcpy(s, argv[0]);
  t = s;
  for(i=1; i<argc; i++) {
    t = t+strlen(t);
    *t++ = ' ';
    strcpy(t, argv[i]);
  }
  return s;
}
/**********************************************************************/

void
showOptionValues(void)
{
  int i;

  printf("Full command line is:\n`%s'\n", cmd.full_cmd_line);

  /***** -plo: The low pulsar period to check (s) */
  if( !cmd.ploP ) {
    printf("-plo not found.\n");
  } else {
    printf("-plo found:\n");
    if( !cmd.ploC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.plo);
    }
  }

  /***** -phi: The high pulsar period to check (s) */
  if( !cmd.phiP ) {
    printf("-phi not found.\n");
  } else {
    printf("-phi found:\n");
    if( !cmd.phiC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.phi);
    }
  }

  /***** -rlo: The low Fourier frequency bin to check */
  if( !cmd.rloP ) {
    printf("-rlo not found.\n");
  } else {
    printf("-rlo found:\n");
    if( !cmd.rloC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.rlo);
    }
  }

  /***** -rhi: The high Fourier frequency bin to check */
  if( !cmd.rhiP ) {
    printf("-rhi not found.\n");
  } else {
    printf("-rhi found:\n");
    if( !cmd.rhiC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.rhi);
    }
  }

  /***** -psr: Name of pulsar to check for (do not include J or B) */
  if( !cmd.psrnameP ) {
    printf("-psr not found.\n");
  } else {
    printf("-psr found:\n");
    if( !cmd.psrnameC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%s'\n", cmd.psrname);
    }
  }

  /***** -candnum: Number of the candidate to optimize from candfile. */
  if( !cmd.candnumP ) {
    printf("-candnum not found.\n");
  } else {
    printf("-candnum found:\n");
    if( !cmd.candnumC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%d'\n", cmd.candnum);
    }
  }

  /***** -candfile: Name of the bincary candidate file. */
  if( !cmd.candfileP ) {
    printf("-candfile not found.\n");
  } else {
    printf("-candfile found:\n");
    if( !cmd.candfileC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%s'\n", cmd.candfile);
    }
  }

  /***** -usr: Describe your own binary candidate.  Must include all of the following (assumed) parameters */
  if( !cmd.usrP ) {
    printf("-usr not found.\n");
  } else {
    printf("-usr found:\n");
  }

  /***** -pb: The orbital period (s) */
  if( !cmd.pbP ) {
    printf("-pb not found.\n");
  } else {
    printf("-pb found:\n");
    if( !cmd.pbC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.pb);
    }
  }

  /***** -x: The projected orbital semi-major axis (lt-sec) */
  if( !cmd.asinicP ) {
    printf("-x not found.\n");
  } else {
    printf("-x found:\n");
    if( !cmd.asinicC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.asinic);
    }
  }

  /***** -e: The orbital eccentricity */
  if( !cmd.eP ) {
    printf("-e not found.\n");
  } else {
    printf("-e found:\n");
    if( !cmd.eC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.e);
    }
  }

  /***** -To: The time of periastron passage (MJD) */
  if( !cmd.ToP ) {
    printf("-To not found.\n");
  } else {
    printf("-To found:\n");
    if( !cmd.ToC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.To);
    }
  }

  /***** -w: Longitude of periastron (deg) */
  if( !cmd.wP ) {
    printf("-w not found.\n");
  } else {
    printf("-w found:\n");
    if( !cmd.wC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.w);
    }
  }

  /***** -wdot: Rate of advance of periastron (deg/yr) */
  if( !cmd.wdotP ) {
    printf("-wdot not found.\n");
  } else {
    printf("-wdot found:\n");
    if( !cmd.wdotC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.wdot);
    }
  }

  /***** -mak: Determine optimization parameters from 'infile.mak' */
  if( !cmd.makefileP ) {
    printf("-mak not found.\n");
  } else {
    printf("-mak found:\n");
  }
  if( !cmd.argc ) {
    printf("no remaining parameters in argv\n");
  } else {
    printf("argv =");
    for(i=0; i<cmd.argc; i++) {
      printf(" `%s'", cmd.argv[i]);
    }
    printf("\n");
  }
}
/**********************************************************************/

void
usage(void)
{
  fprintf(stderr, "usage: %s%s", Program, "\
 [-plo plo] [-phi phi] [-rlo rlo] [-rhi rhi] [-psr psrname] [-candnum candnum] [-candfile candfile] [-usr] [-pb pb] [-x asinic] [-e e] [-To To] [-w w] [-wdot wdot] [-mak] [--] infile\n\
    Generates a set of fake orbits and their Fourier modulation spectra for a binary candidate (usually generated by search_bin).  It tries to return the optimum fit for the binary candidate.\n\
       -plo: The low pulsar period to check (s)\n\
             1 double value between 0 and oo\n\
       -phi: The high pulsar period to check (s)\n\
             1 double value between 0 and oo\n\
       -rlo: The low Fourier frequency bin to check\n\
             1 double value between 0 and oo\n\
       -rhi: The high Fourier frequency bin to check\n\
             1 double value between 0 and oo\n\
       -psr: Name of pulsar to check for (do not include J or B)\n\
             1 char* value\n\
   -candnum: Number of the candidate to optimize from candfile.\n\
             1 int value between 1 and oo\n\
  -candfile: Name of the bincary candidate file.\n\
             1 char* value\n\
       -usr: Describe your own binary candidate.  Must include all of the following (assumed) parameters\n\
        -pb: The orbital period (s)\n\
             1 double value between 0 and oo\n\
         -x: The projected orbital semi-major axis (lt-sec)\n\
             1 double value between 0 and oo\n\
         -e: The orbital eccentricity\n\
             1 double value between 0 and 0.9999999\n\
             default: `0'\n\
        -To: The time of periastron passage (MJD)\n\
             1 double value between 0 and oo\n\
         -w: Longitude of periastron (deg)\n\
             1 double value between 0 and 360\n\
      -wdot: Rate of advance of periastron (deg/yr)\n\
             1 double value\n\
             default: `0'\n\
       -mak: Determine optimization parameters from 'infile.mak'\n\
     infile: Input fft file name (without a suffix) of floating point fft data.  A '.inf' file of the same name must also exist\n\
             1 value\n\
version: 22Apr04\n\
");
  exit(EXIT_FAILURE);
}
/**********************************************************************/
Cmdline *
parseCmdline(int argc, char **argv)
{
  int i;

  Program = argv[0];
  cmd.full_cmd_line = catArgv(argc, argv);
  for(i=1, cmd.argc=1; i<argc; i++) {
    if( 0==strcmp("--", argv[i]) ) {
      while( ++i<argc ) argv[cmd.argc++] = argv[i];
      continue;
    }

    if( 0==strcmp("-plo", argv[i]) ) {
      int keep = i;
      cmd.ploP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.plo, 1);
      cmd.ploC = i-keep;
      checkDoubleHigher("-plo", &cmd.plo, cmd.ploC, 0);
      continue;
    }

    if( 0==strcmp("-phi", argv[i]) ) {
      int keep = i;
      cmd.phiP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.phi, 1);
      cmd.phiC = i-keep;
      checkDoubleHigher("-phi", &cmd.phi, cmd.phiC, 0);
      continue;
    }

    if( 0==strcmp("-rlo", argv[i]) ) {
      int keep = i;
      cmd.rloP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.rlo, 1);
      cmd.rloC = i-keep;
      checkDoubleHigher("-rlo", &cmd.rlo, cmd.rloC, 0);
      continue;
    }

    if( 0==strcmp("-rhi", argv[i]) ) {
      int keep = i;
      cmd.rhiP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.rhi, 1);
      cmd.rhiC = i-keep;
      checkDoubleHigher("-rhi", &cmd.rhi, cmd.rhiC, 0);
      continue;
    }

    if( 0==strcmp("-psr", argv[i]) ) {
      int keep = i;
      cmd.psrnameP = 1;
      i = getStringOpt(argc, argv, i, &cmd.psrname, 1);
      cmd.psrnameC = i-keep;
      continue;
    }

    if( 0==strcmp("-candnum", argv[i]) ) {
      int keep = i;
      cmd.candnumP = 1;
      i = getIntOpt(argc, argv, i, &cmd.candnum, 1);
      cmd.candnumC = i-keep;
      checkIntHigher("-candnum", &cmd.candnum, cmd.candnumC, 1);
      continue;
    }

    if( 0==strcmp("-candfile", argv[i]) ) {
      int keep = i;
      cmd.candfileP = 1;
      i = getStringOpt(argc, argv, i, &cmd.candfile, 1);
      cmd.candfileC = i-keep;
      continue;
    }

    if( 0==strcmp("-usr", argv[i]) ) {
      cmd.usrP = 1;
      continue;
    }

    if( 0==strcmp("-pb", argv[i]) ) {
      int keep = i;
      cmd.pbP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.pb, 1);
      cmd.pbC = i-keep;
      checkDoubleHigher("-pb", &cmd.pb, cmd.pbC, 0);
      continue;
    }

    if( 0==strcmp("-x", argv[i]) ) {
      int keep = i;
      cmd.asinicP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.asinic, 1);
      cmd.asinicC = i-keep;
      checkDoubleHigher("-x", &cmd.asinic, cmd.asinicC, 0);
      continue;
    }

    if( 0==strcmp("-e", argv[i]) ) {
      int keep = i;
      cmd.eP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.e, 1);
      cmd.eC = i-keep;
      checkDoubleLower("-e", &cmd.e, cmd.eC, 0.9999999);
      checkDoubleHigher("-e", &cmd.e, cmd.eC, 0);
      continue;
    }

    if( 0==strcmp("-To", argv[i]) ) {
      int keep = i;
      cmd.ToP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.To, 1);
      cmd.ToC = i-keep;
      checkDoubleHigher("-To", &cmd.To, cmd.ToC, 0);
      continue;
    }

    if( 0==strcmp("-w", argv[i]) ) {
      int keep = i;
      cmd.wP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.w, 1);
      cmd.wC = i-keep;
      checkDoubleLower("-w", &cmd.w, cmd.wC, 360);
      checkDoubleHigher("-w", &cmd.w, cmd.wC, 0);
      continue;
    }

    if( 0==strcmp("-wdot", argv[i]) ) {
      int keep = i;
      cmd.wdotP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.wdot, 1);
      cmd.wdotC = i-keep;
      continue;
    }

    if( 0==strcmp("-mak", argv[i]) ) {
      cmd.makefileP = 1;
      continue;
    }

    if( argv[i][0]=='-' ) {
      fprintf(stderr, "\n%s: unknown option `%s'\n\n",
              Program, argv[i]);
      usage();
    }
    argv[cmd.argc++] = argv[i];
  }/* for i */


  /*@-mustfree*/
  cmd.argv = argv+1;
  /*@=mustfree*/
  cmd.argc -= 1;

  if( 1>cmd.argc ) {
    fprintf(stderr, "%s: there should be at least 1 non-option argument(s)\n",
            Program);
    exit(EXIT_FAILURE);
  }
  if( 1<cmd.argc ) {
    fprintf(stderr, "%s: there should be at most 1 non-option argument(s)\n",
            Program);
    exit(EXIT_FAILURE);
  }
  /*@-compmempass*/  return &cmd;
}

