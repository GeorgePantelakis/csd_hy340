// CREATE TCODE TXT FILE
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "Utilities.h"

void create_tcode_txt_file(){
    unsigned int i;
    struct const_string_list *tmp_string_list;
    struct const_number_list *tmp_number_list;
    struct userfunc_list *tmp_userfunc_list;
    struct libfunc_list *tmp_libfunc_list;
    struct instruction current_instruction;

    tmp_string_list = string_list;
    tmp_number_list = number_list;
    tmp_userfunc_list = userfunc_list;
    tmp_libfunc_list = libfunc_list;

    tcode_txt_file = fopen("tcode.txt", "w");
    if(!tcode_txt_file){
        fprintf(stderr, "\nCannot create tcode file\n");
        exit(1);
    }

    fprintf(tcode_txt_file, "\nMagic Number: %u\n", magic_number);
    fprintf(tcode_txt_file, "Number of Global Variables: %u\n", (programm_var_offset + 1));
    fprintf(tcode_txt_file, "\n------------------STRING ARRAY------------------\n");
    fprintf(tcode_txt_file, "String Array Size: %u\n", string_array_size);
    while(tmp_string_list != NULL){
        fprintf(tcode_txt_file, "%d: %s\n", tmp_string_list->index, tmp_string_list->value);
        tmp_string_list = tmp_string_list->next;
    }

    fprintf(tcode_txt_file, "\n------------------NUMBER ARRAY------------------\n");
    fprintf(tcode_txt_file, "Number Array Size: %u\n", number_array_size);
    while(tmp_number_list != NULL){
        fprintf(tcode_txt_file, "%d: %f\n", tmp_number_list->index, tmp_number_list->value);
        tmp_number_list = tmp_number_list->next;
    }

    fprintf(tcode_txt_file, "\n------------------USERFUNC ARRAY------------------\n");
    fprintf(tcode_txt_file, "Userfunc Array Size: %u\n", userfunc_array_size);
    while(tmp_userfunc_list != NULL){
        fprintf(tcode_txt_file, "%d: %s,%u,%u\n", tmp_userfunc_list->index, tmp_userfunc_list->value->value.funcVal->name, tmp_userfunc_list->value->value.funcVal->taddress, tmp_userfunc_list->value->value.funcVal->localVars);
        tmp_userfunc_list = tmp_userfunc_list->next;
    }

    fprintf(tcode_txt_file, "\n------------------LIBFUNC ARRAY------------------\n");
    fprintf(tcode_txt_file, "Libfunc Array Size: %u\n", libfunc_array_size);
    while(tmp_libfunc_list != NULL){
        fprintf(tcode_txt_file, "%d: %s\n", tmp_libfunc_list->index, tmp_libfunc_list->value);
        tmp_libfunc_list = tmp_libfunc_list->next;
    }

    fprintf(tcode_txt_file, "\n------------------INSTRUCTIONS------------------\n");
    fprintf(tcode_txt_file, "Code Size: %u\n", instruction_index);
    for(i = 0; i < instruction_index; i++){
        current_instruction = instructions[i];
        fprintf(tcode_txt_file, "%u: ", i);
        switch(current_instruction.opcode){
            case add_t:
                fprintf(tcode_txt_file, "add ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case sub_t:
                fprintf(tcode_txt_file, "sub ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case mul_t:
                fprintf(tcode_txt_file, "mul ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case div_tc:
                fprintf(tcode_txt_file, "div ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case mod_t:
                fprintf(tcode_txt_file, "mod ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case newtable:
                fprintf(tcode_txt_file, "newtable ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case tablegetelem_t:
                fprintf(tcode_txt_file, "tablegetelem ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case tablesetelem_t:
                fprintf(tcode_txt_file, "tablesetelem ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case assign_t:
                fprintf(tcode_txt_file, "assign ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case jump_t:
                fprintf(tcode_txt_file, "jump ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case jeq:
                fprintf(tcode_txt_file, "jeq ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case jne:
                fprintf(tcode_txt_file, "jne ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case jgt:
                fprintf(tcode_txt_file, "jgt ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case jge:
                fprintf(tcode_txt_file, "jge ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case jlt:
                fprintf(tcode_txt_file, "jlt ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case jle:
                fprintf(tcode_txt_file, "jle ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case pusharg:
                fprintf(tcode_txt_file, "puharg ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case callfunc:
                fprintf(tcode_txt_file, "callfunc ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case enterfunc:
                fprintf(tcode_txt_file, "enterfunc ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            case exitfunc:
                fprintf(tcode_txt_file, "exitfunc ");
                print_register(current_instruction.result);
                print_register(current_instruction.arg1);
                print_register(current_instruction.arg2);
                fprintf(tcode_txt_file, "line: %u\n", current_instruction.srcLine);
                break;
            default: 
                assert(0);
        }

    }

    fclose(tcode_txt_file);
}

void print_register(struct vmarg arg){
    switch(arg.type){
        case global_a:
            fprintf(tcode_txt_file, "01,%u ", arg.val);
            break;
        case local_a:
            fprintf(tcode_txt_file, "03,%u ", arg.val);
            break;
        case formal_a:
            fprintf(tcode_txt_file, "02,%u ", arg.val);
            break;
        case bool_a:
            fprintf(tcode_txt_file, "06,%u ", arg.val);
            break;
        case string_a:
            fprintf(tcode_txt_file, "05,%u ", arg.val);
            break;
        case number_a:
            fprintf(tcode_txt_file, "04,%u ", arg.val);
            break;
        case nil_a:
            fprintf(tcode_txt_file, "07,%u ", arg.val);
            break;
        case userfunc_a:
            fprintf(tcode_txt_file, "08,%u ", arg.val);
            break;
        case libfunc_a:
            fprintf(tcode_txt_file, "09,%u ", arg.val);
            break;
        case retval_a:
            fprintf(tcode_txt_file, "10,%u ", arg.val);
            break;
        case label_a:
            fprintf(tcode_txt_file, "00,%u ", arg.val);
            break;
        case ignore_a:
            break;
        default:
            assert(0);
    }
}