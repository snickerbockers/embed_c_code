/*******************************************************************************
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2019, snickerbockers
 * All rights reserved.
 * 
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are
 *    met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

static void print_usage(char *cmd) {
    fprintf(stderr, "usage: %s -i <input_file> -o <output_file> -t <var> [-h include_guard_const]\n",
            cmd);
    exit(1);
}

int main(int argc, char **argv) {
    int optch;
    char const *inpath = NULL;
    char const *outpath = NULL;
    char const *varname = NULL;
    char const *include_guard_const = NULL;
    while ((optch = getopt(argc, argv, "i:o:t:h:")) != -1) {
        switch (optch) {
        case 'i':
            inpath = optarg;
            break;
        case 'o':
            outpath = optarg;
            break;
        case 't':
            varname = optarg;
            break;
        case 'h':
            include_guard_const = optarg;
            break;
        default:
            print_usage(argv[0]);
        }
    }
    if (!(inpath && outpath && varname))
        print_usage(argv[0]);
    FILE *infile = fopen(inpath, "rb");
    if (!infile) {
        fprintf(stderr, "failure to open \"%s\"\n", inpath);
        exit(1);
    }
    FILE *outfile = fopen(outpath, "wb");
    if (!outfile) {
        fprintf(stderr, "failure to open \"%s\"\n", outpath);
        exit(1);
    }

    if (include_guard_const) {
        fprintf(outfile, "#ifndef %s\n#define %s\n",
                include_guard_const, include_guard_const);
    }

    fprintf(outfile, "static char const %s[] = {", varname);

    bool first_val = true;
    int ch;
    int col = 0;
    while ((ch = fgetc(infile)) != EOF) {
        if (first_val)
            first_val = false;
        else
            fputc(',', outfile);
        if (col == 0)
            fputs("\n\t", outfile);
        else
            fputc(' ', outfile);
        
        col = (1 + col) % 10;

        fprintf(outfile, "0x%02x", ch);
    }

    fprintf(outfile, "\n};\n");

    if (include_guard_const)
        fputs("#endif\n", outfile);

    fclose(outfile);
    fclose(infile);
}
