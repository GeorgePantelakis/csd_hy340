// CREATE QUAD FILE
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "Utilities.h"

void create_quad_file(){
    FILE *quad_file;
    int i;
    struct quad current_quad;

    quad_file = fopen("quads.txt", "w");
    if(!quad_file){
        fprintf(stderr, "\nCannot create quad file\n");
        exit(1);
    }
    
    fprintf(quad_file, "Line Number\tQuad number\tOpcode\t\t\tResult\t\t\tArgument 1\t\t\tArgument 2\t\t\tLabel\n");
    fprintf(quad_file, "---------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    for(i = 0; i < quad_index; i++){
        current_quad = quads[i];
        switch (current_quad.op){
            case assign:
                fprintf(quad_file, "[%u]\t\t%d:\t\t assign\t\t\t", current_quad.line ,i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);\
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                break;
            case add:
                fprintf(quad_file, "[%u]\t\t%d:\t\t add\t\t\t", current_quad.line ,i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);
                    break;
                default:
                    if((current_quad.result)->sym->type == USERFUNC || (current_quad.result)->sym->type == LIBFUNC)
                        fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.funcVal->name);
                    else
                        fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                break;
            case sub:
                fprintf(quad_file, "[%u]\t\t%d:\t\t sub\t\t\t",current_quad.line ,i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                break;
            case mul:
                fprintf(quad_file, "[%u]\t\t%d:\t\t mul\t\t\t",current_quad.line ,i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }break;
            case div_e:
                fprintf(quad_file, "[%u]\t\t%d:\t\t div\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }break;
            case mod:
                fprintf(quad_file, "[%u]\t\t%d:\t\t mod\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }break;
            case uminus:
                fprintf(quad_file, "[%u]\t\t%d:\t\t uminus\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }break;
            case if_eq:
                fprintf(quad_file, "[%u]\t\t%d:\t\t if_eq\t\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                fprintf(quad_file, "%d\n", current_quad.label + 1);
                break;
            case if_noteq:
                fprintf(quad_file, "[%u]\t\t%d:\t\t if_noteq\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                fprintf(quad_file, "%d\n", current_quad.label + 1);
                break;
            case if_lesseq:
                fprintf(quad_file, "[%u]\t\t%d:\t\t if_lesseq\t\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                fprintf(quad_file, "%d\n", current_quad.label + 1);
                break;
            case if_greatereq:
                fprintf(quad_file, "[%u]\t\t%d:\t\t if_greatereq\t\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                fprintf(quad_file, "%d\n", current_quad.label + 1);
                break;
            case if_less:
                fprintf(quad_file, "[%u]\t\t%d:\t\t if_less\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                fprintf(quad_file, "%d\n", current_quad.label + 1);
                break;
            case if_greater:
                fprintf(quad_file, "[%u]\t\t%d:\t\t if_greater\t\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                fprintf(quad_file, "%d\n", current_quad.label + 1);
                break;
            case call:
                fprintf(quad_file, "[%u]\t\t%d:\t\t call\t\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                break;
            case param:
                fprintf(quad_file, "[%u]\t\t%d:\t\t param\t\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                break;
            case ret:
                fprintf(quad_file, "[%u]\t\t%d:\t\t ret\t\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                break;
            case getretval:
                fprintf(quad_file, "[%u]\t\t%d:\t\t getretval\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.result)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                break;
            case funcstart:
                fprintf(quad_file, "[%u]\t\t%d:\t\t funcstart\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                break;
            case funcend:
                 fprintf(quad_file, "[%u]\t\t%d:\t\t funcend\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                break;
            case tablecreate:
                 fprintf(quad_file, "[%u]\t\t%d:\t\t tablecreate\t\t\t\t\t",current_quad.line , i + 1);
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                break;
            case tablegetelem:
                fprintf(quad_file, "[%u]\t\t%d:\t\t tablegetelem\t\t",current_quad.line , i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                break;
            case tablesetelem:
                fprintf(quad_file, "[%u]\t\t%d:\t\t tablesetelem\t\t",current_quad.line , i + 1);
                switch ((current_quad.result)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t", (int)(current_quad.result)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t", (float)(current_quad.result)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.result)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t", (current_quad.result)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t", (current_quad.result)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg1)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\t\t\t\t");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\t\t\t\t", (int)(current_quad.arg1)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\t\t\t\t", (float)(current_quad.arg1)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg1)->boolConst == 1){
                        fprintf(quad_file, "true\t\t\t\t");
                    }else{
                        fprintf(quad_file, "false\t\t\t\t");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\t\t\t\t", (current_quad.arg1)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\t\t\t\t", (current_quad.arg1)->sym->value.varVal->name);
                    break;
                }
                switch ((current_quad.arg2)->type)
                {
                case nil_e:
                    fprintf(quad_file, "nil\n");
                    break;
                case constint_e:
                    fprintf(quad_file, "%d\n", (int)(current_quad.arg2)->numConst);
                    break;
                case constfloat_e:
                    fprintf(quad_file, "%f\n", (float)(current_quad.arg2)->numConst);
                    break;
                case constbool_e:
                    if((current_quad.arg2)->boolConst == 1){
                        fprintf(quad_file, "true\n");
                    }else{
                        fprintf(quad_file, "false\n");
                    }
                    break;
                case conststring_e:
                    fprintf(quad_file, "\"%s\"\n", (current_quad.arg2)->strConst);
                    break;
                default:
                    fprintf(quad_file, "%s\n", (current_quad.arg2)->sym->value.varVal->name);
                    break;
                }
                break;
            case jump:
                 fprintf(quad_file, "[%u]\t\t%d:\t\t jump\t\t\t\t\t\t\t\t\t\t\t\t\t\t%d\n",current_quad.line , i + 1, current_quad.label + 1);
                break;
            default:
                break;
        }
    }

    fprintf(quad_file, "[%u]\t\t%d: \n",yylineno ,i + 1);
    fprintf(quad_file, "\n---------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    fclose(quad_file);
}

