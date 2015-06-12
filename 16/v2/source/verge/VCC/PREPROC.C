/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
// ³                  The VergeC Compiler version 2.01                   ³
// ³              Copyright (C)1998 BJ Eirich (aka vecna)                ³
// ³                        Pre Processor module                         ³
// ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CHANGELOG:
// <aen, may 14>
// + fixed incorrect file markers and last character omission.
// + one more incorrect file marker fix; was not restoring correctly after
//   #included files.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#ifdef __WATCOMC__
#include <io.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "vcc.h"
#include "lexical.h"

int pp_dump;
int pp_nomark;
static int pp_tempct = 0;

typedef struct
  {
    char *sym;
    int sym_len;

    char *resolve;
    int resolve_len;

  } pp_def;

typedef struct
  {
    char *filename;
    char *data;

  } pp_include;

void Process(char *filename);
char *pp_token(char *p);

////////////////////////////////////////////////////////////////////////////
// DATA ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

pp_def *pp_defarr         = NULL;

int     pp_num_defs       = 0;
int     pp_defs_allocated = 0;
int     PP_DEFS_BLOCK     = 25;

pp_include pp_incarr[100];
//pp_include *pp_icur       = NULL; // get rid of this
char *cur_filename=0;
char *last_filename=0;

int     pp_num_includes   = 0;

int     pp_line           = 0;
int     pp_total_lines    = 0;

const  int     EOF_CHAR          = 0x00;

char    pp_chr_table[256];

enum
  {
    PP_ERROR,
    PP_WHITE,
    PP_PUNC,
    PP_DIGIT,
    PP_LETTER,
    PP_QUOTE,
    PP_DIRECTIVE,
    PP_NEWLINE,
    PP_EOF
  };

FILE   *pp_out            = NULL;

char    pp_tok[1024];
int     pp_toktype        = 0;

int     pp_last_delim     = 1;

////////////////////////////////////////////////////////////////////////////
// CODE ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

int logging=1;
void log(char *str)
{
  if (logging)
  {
    FILE *fp = fopen("error.log", "a");
    fprintf(fp, str);
    fflush(fp);
    fclose(fp);
  }
}

void init_pp_chr_table()
{
  int x;

  memset(pp_chr_table, PP_ERROR, 256);

  pp_chr_table[EOF_CHAR] = PP_EOF;

  for (x='A'; x<='Z'; x++) pp_chr_table[x] = PP_LETTER;
  for (x='a'; x<='z'; x++) pp_chr_table[x] = PP_LETTER;
  pp_chr_table['_'] = PP_LETTER;
  pp_chr_table['.'] = PP_LETTER;

  for (x='0'; x<='9'; x++) pp_chr_table[x] = PP_DIGIT;
  pp_chr_table['$'] = PP_DIGIT;
  pp_chr_table['\''] = PP_DIGIT;

  pp_chr_table['+'] = pp_chr_table['-'] = PP_PUNC;
  pp_chr_table['*'] = pp_chr_table['/'] = PP_PUNC;
  pp_chr_table['%'] = PP_PUNC;
  pp_chr_table['|'] = pp_chr_table['&'] = PP_PUNC;
  pp_chr_table['='] = pp_chr_table['^'] = PP_PUNC;
  pp_chr_table[','] = PP_PUNC;
  pp_chr_table['<'] = pp_chr_table['>'] = PP_PUNC;
  pp_chr_table['('] = pp_chr_table[')'] = PP_PUNC;
  pp_chr_table['['] = pp_chr_table[']'] = PP_PUNC;
  pp_chr_table['{'] = pp_chr_table['}'] = PP_PUNC;
  pp_chr_table[':'] = pp_chr_table[';'] = PP_PUNC;
  pp_chr_table['\\'] = pp_chr_table['!'] = PP_PUNC;

  pp_chr_table[' '] = pp_chr_table['\t'] = PP_WHITE;
  pp_chr_table['\r'] = PP_WHITE;

  pp_chr_table['\n'] = PP_NEWLINE;

  pp_chr_table['\"'] = PP_QUOTE;

  pp_chr_table['#'] = PP_DIRECTIVE;
}

void pp_line_marker()
{
  if (pp_nomark) return;

  fputc(2, pp_out);
  fwrite(&pp_line, 1, 4, pp_out);
}

void pp_file_marker()
{
  if (pp_nomark) return;

  fputc(1, pp_out);
  fwrite(cur_filename, 1, strlen(cur_filename)+1, pp_out);
}

void pp_error(const char *error, ...)
{
  va_list  lst;
  char     string[1024];

  va_start (lst, error);
  vsprintf (string, error, lst);
  va_end   (lst);

  printf   ("*preproc error* ");
  err      (string);
}

void pp_line_error(const char *error, ...)
{
  va_list  lst;
  char     string[1024];

  va_start (lst, error);
  vsprintf (string, error, lst);
  va_end   (lst);

  printf   ("%s(%d) ", cur_filename, pp_line);
  pp_error (string);
}

int pp_is_punc(int c)
  { return (PP_PUNC == pp_chr_table[c]); }

int pp_is_letter(int c)
  { return (PP_LETTER == pp_chr_table[c]); }

int pp_is_digit(int c)
  { return (PP_DIGIT == pp_chr_table[c]); }

int pp_is_ident(int c)
  { return (pp_is_letter(c) || pp_is_digit(c)); }

int pp_is_white(int c)
  { return (PP_WHITE == pp_chr_table[c]); }

int pp_is_directive(int c)
  { return (PP_DIRECTIVE == pp_chr_table[c]); }

int pp_is_eof(int c)
  { return (PP_EOF == pp_chr_table[c]); }

pp_def *pp_def_add(char *sym, char *resolve)
{
  pp_def *pp = NULL;
  pp_def *p = NULL;

  if (!(pp_defs_allocated % PP_DEFS_BLOCK))
  {
    pp_defs_allocated += PP_DEFS_BLOCK;

    p = (pp_def *)realloc(
      pp_defarr, pp_defs_allocated * sizeof(pp_def));
    if (!p) pp_error("unable to grow #define list");

    pp_defarr = p;
  }

  pp = &pp_defarr[pp_num_defs];
  ++pp_num_defs;

  pp->sym = (char *)malloc(strlen(sym) +1);
  if (!pp->sym)
    pp_error("memory exhausted");
  strcpy(pp->sym, sym);
  pp->sym_len = strlen(sym);

  pp->resolve = (char *)malloc(strlen(resolve) +1);
  if (!pp->resolve)
    pp_error("memory exhausted");
  strcpy(pp->resolve, resolve);
  pp->resolve_len = strlen(resolve);

  return pp;
}

pp_include *pp_include_add(char *filename)
{
  FILE       *in = NULL;
  pp_include *pp = NULL;
  int         z  = 0;

  if (pp_num_includes)
  {
    int i;
    for (i=0; i<pp_num_includes; i++)
    {
      if (!stricmp(pp_incarr[i].filename, pp_tok))
        pp_error("circular dependencies");
    }
  }

  // alias
  pp = &pp_incarr[pp_num_includes++];

  in = fopen(filename, "rb");
  if (!in) pp_error("unable to open %s", filename);

  // filelength
  fseek(in, 0, SEEK_END);
  z = ftell(in);
  fseek(in, 0, SEEK_SET);

  // cache file
  pp->data = (char *)malloc((z +2) * sizeof(char));
  if (!pp->data) pp_error("memory exhausted");
  fread(pp->data, 1, z, in);
  fclose(in);

  pp->data[z++] = 32; // <aen, may 12>
  pp->data[z] = EOF_CHAR;

  pp->filename = (char *)malloc(strlen(filename) +1);
  if (!pp->filename) pp_error("memory exhausted");
  strcpy(pp->filename, filename);
  cur_filename=pp->filename;

  return pp;
}

char *pp_skip_c_comment(char *p)
{
  p += 2;
  while (*p && ('*' != *p || '/' != p[1]))
  {
    if ('\n' == *p)
    {
      ++pp_line;
      pp_line_marker();
    }
    if ('/' == *p && '*' == p[1])
      p = pp_skip_c_comment(p);
    else ++p;
  }
  if (*p) p += 2;

  return p;
}

char *pp_skip_cpp_comment(char *p)
{
  p += 2;
  while (*p && '\n' != *p)
    ++p;
  if (*p)
  {
    ++p;

    ++pp_line;
    pp_line_marker();
  }

  return p;
}

char *pp_skipwhite(char *p)
{
  for (;;)
  {
    while (*p && PP_WHITE == pp_chr_table[(int) *p])
      ++p;

    if ('\n' == *p)
    {
      ++p;

      ++pp_line;
      pp_line_marker();
      continue;
    }

    // skip comments
    if ('/' == *p)
    {
      switch (p[1])
      {
        case '/': p = pp_skip_cpp_comment(p); continue;
        case '*': p = pp_skip_c_comment(p);   continue;
      }
    }

    break;
  }

  return p;
}

char *pp_token_punc(char *p)
{
  char *s = pp_tok;

  do {
    *s++ = *p++;
  } while (*p && pp_is_punc(*p));
  *s = 0;

  return p;
}

/*
int is_hex(int c)
{
  return (
     (c >= '0' && c <= '9')
  || (c >= 'A' && c <= 'F')
  || (c >= 'a' && c <= 'f'));
}
*/

char *pp_token_digit(char *p)
{
  char *s = pp_tok;

  if ('\'' == *p)
  {
    *s++ = *p++;
    if (!*p) return NULL;
    *s++ = *p++;
    if (!*p) return NULL;
    *s++ = *p++;
    if (!*p) return NULL;
    *s = 0;
  }
  else
  {
    do {
      *s++ = *p++;
    } while (*p && pp_is_digit(*p));
    if (*p) *s = 0;
  }

  return p;
}

char *pp_token_letter(char *p)
{
  char *s = pp_tok;

  do {
    *s++ = *p++;
  } while (*p && pp_is_ident(*p));
  if (*p) *s = 0;

  return p;
}

char *pp_token_directive(char *p)
{
  char *s = pp_tok;

  ++p; // skip #

  // directive name
  do {
    *s++ = *p++;
  } while (pp_is_ident(*p));
  *s = 0;

  // #include
  if (!strcmp(pp_tok, "include"))
  {
    // boogly boogly boogly!
    int l = 0;
    char *e = NULL;

    //printf("#include detected \n");

    // any whitespace after 'include'
    p = pp_skipwhite(p);

    // skip, but don't expect, a "
    if ('\"' == *p) ++p;

    // grab the filename
    e = pp_tok;
    while (*p && '\"' != *p)
      *e++ = *p++;
    // if we didn't hit eof...?
    if (*p)
    {
      // zero term filename
      *e = 0;
      // skip closing "
      ++p;

      last_filename=cur_filename; // <aen, may 14>
      l = pp_line;      // save
      Process(pp_tok);
      pp_line = l;      // restore
      cur_filename=last_filename; // <aen, may 14>
      // reset file marker
      pp_file_marker();

      // disregard rest of line
      while (*p && '\n' != *p)
        ++p;
      // if we didn't hit eof...?
      if (*p)
      {
        // skip newline char
        ++p;

        // update line
        ++pp_line;
        pp_line_marker();

        // dump null token... writes nothing i think...
        pp_tok[0]='\0';
      }
      //else pp_error("screw you!");
    }
  }
  // #define
  else if (!strcmp(pp_tok, "define"))
  {
    char *e = NULL;
    char sym[256];
    char resolve[1024];

    // find start of symbol
    while (' ' == *p || '\t' == *p)
      ++p;

    // parse symbol
    e = sym;
    while (pp_is_ident(*p))
      { *e++ = *p++; }
    *e = 0;

    // check for duplicate #defines
    if (pp_num_defs)
    {
      int i;
      int z = strlen(sym);
      for (i=0; i<pp_num_defs; i++)
      {
        if (z == pp_defarr[i].sym_len)
        {
          if (!stricmp(pp_defarr[i].sym, sym))
            pp_error("duplicate #define symbol");
        }
      }
    }

    // find start of argument list (if any), or resolve
    while (' ' == *p || '\t' == *p)
      ++p;

    // found argument list; discard for now
    if ('(' == *p)
    {
      ++p;
      while (')' != *p)
        ++p;
      ++p;

      // find start of resolve
      while (' ' == *p || '\t' == *p)
        ++p;
    }

    pp_last_delim = 1;
    e = resolve;
    // parse resolve
    while ('\n' != *p)
    {
      // whitespace
      if (' ' == *p || '\t' == *p)
      {
        do {
          ++p;
        } while (' ' == *p || '\t' == *p);
      }

      // continuation? (multiple lines)
      if ('\\' == *p)
      {
        ++p;
        while (*p && '\n' != *p)
          ++p;
        // if not eof
        if (*p)
        {
          ++p;

          ++pp_line;
          pp_line_marker();
        }
        continue;
      }

      // special case for 'x' chars
      if ('\'' == *p)
      {
        *e++ = *p++;
        do {
          *e++ = *p++;
        } while (*p && '\'' != *p);
        *e++ = *p++;

        pp_last_delim = 1;
        continue;
      }

      // special case for string literals
      if ('\"' == *p)
      {
        do {
          *e++ = *p++;
        } while (*p && '\"' != *p);
        if (*p)
          *e++ = *p++;

        pp_last_delim = 1;
        continue;
      }

      // this is just the preprocessor, so i don't need
      // to get punctuation or digits in their correct form, i just
      // have to copy them over as i see them.
      if (pp_is_punc(*p) || pp_is_digit(*p))
      {
        do {
          // d'oh!
          if ('\'' == *p || '\\' == *p)
            break;

          *e++ = *p++;
        } while (pp_is_punc(*p) || pp_is_digit(*p));

        pp_last_delim = 1;
        continue;
      }

      if (pp_is_letter(*p))
      {
        if (!pp_last_delim)
          *e++ = ' ';

        do {
          *e++ = *p++;
        } while (pp_is_ident(*p));

        pp_last_delim = 0;
        continue;
      }

      ++p; // unknown?
    }
    *e = 0;

    // update define list
    pp_def_add(sym, resolve);

    pp_tok[0]='\0';
  }
  // #???
  else pp_error("unknown preprocessor directive");

  return p;
}

char *pp_token_quote(char *p)
{
  char *s = pp_tok;

  *s++ = *p++;

  while (*p && '\"' != *p)
    *s++ = *p++;
  // if not eof
  if (*p)
  {
    *s++ = *p++;
    *s   = 0;
  }

  return p;
}

char *pp_token(char *p)
{
  p = pp_skipwhite(p);
  if (!*p)
  {
    pp_tok[0] = '\0'; // gotta have this! (fixes last sym replication bug)
    return p;
  }

  pp_toktype = pp_chr_table[(int) *p];

  switch (pp_toktype)
  {
    case PP_PUNC:    p = pp_token_punc(p);   break;
    case PP_DIGIT:   p = pp_token_digit(p);  break;
    case PP_LETTER:  p = pp_token_letter(p); break;

    case PP_QUOTE:   p = pp_token_quote(p);  break;

    case PP_DIRECTIVE:
      p = pp_token_directive(p);
      break;

    case PP_EOF:
      break;

    case PP_ERROR:
      pp_line_error("unknown character <%c:%i>", *p, *p);
  }

  return p;
}

void Process(char *filename)
{
  char *s = NULL;
  pp_include *ppi  = NULL;

  // starts fresh for each #include file
  pp_line = 1;
  //pp_line_marker(); // hmm... did this kill the wat ver? i forget..

  // push file
  ppi = pp_include_add(filename);
  s   = ppi->data;

  pp_file_marker();

  pp_last_delim = 1;
  s = pp_token(s);

  // loop until we hit an EOF token
  while (*s && PP_EOF != pp_toktype)
  {
    // for compactor; this is absolutely required for the parsing to
    // work correctly for hex numbers
    if (PP_LETTER == pp_toktype || PP_DIGIT == pp_toktype)
    {
      if (!pp_last_delim)
        fputc(' ', pp_out);
    }

    if (pp_num_defs)
    {
      // #define replacement
      if (PP_LETTER == pp_toktype)
      {
        pp_def *ppd = NULL;
        int pp_token_len = strlen(pp_tok);
        int i = 0;

        for (i=0; i<pp_num_defs; i++)
        {
          ppd = pp_defarr + i;
          // first see if the lengths match, for possible quick discard
          if (ppd->sym_len == pp_token_len)
          {
            // lengths match, compare bytes
            if (!stricmp(pp_tok, ppd->sym))
            {
              // match! overwrite token with resolve
              memcpy(pp_tok, ppd->resolve, strlen(ppd->resolve)+1);
              break;
            }
          }
        }
      }
    }

    // write whatever we got
    fwrite(pp_tok, 1, strlen(pp_tok), pp_out);

    pp_last_delim = (PP_LETTER != pp_toktype);
    s = pp_token(s);
  }

  // pop file
  free(ppi->data);
  free(ppi->filename);
  --pp_num_includes;
  //pp_icur = &pp_incarr[pp_num_includes]; // oops! :-)

  pp_total_lines += pp_line;
}

void pp_dump_output()
{
  char strx[1024];

  sprintf(strx, "copy vcctemp.$$$ vcc%i.$$$", pp_tempct);
  ++pp_tempct;

  system(strx);
}

void PreProcess(char *filename)
{
  pp_num_includes = 0;
  init_pp_chr_table();

  pp_out = fopen("VCCTEMP.$$$", "wb");
  if (!pp_out) pp_error("unable to open VCCTEMP.$$$");

  Process(filename);

  fclose(pp_out);

  // dump preproc output?
  if (0 != pp_dump)
    pp_dump_output();
}
