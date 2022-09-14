// AVM EXECUTIONFUNCS
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "AVM.h"

struct avm_memcell* avm_translate_operand(struct vmarg* arg, struct avm_memcell* reg){
    assert(arg);

    switch(arg->type){
        case global_a: return &stack[AVM_STACKSIZE - 1 - arg->val];
        case local_a: return &stack[topsp - arg->val];
        case formal_a: return &stack[topsp + AVM_STACKENV_SIZE + 1 + arg->val];

        case retval_a: return &retval;

        case number_a:
            reg->type = number_m;
            reg->data.numVal = consts_getnumber(arg->val);
            return reg;

        case string_a:
            reg->type = string_m;
            reg->data.strVal = strdup(consts_getstring(arg->val));
            return reg;

        case bool_a:
            reg->type = bool_m;
            if(arg->val == 0){
                reg->data.boolVal = 0;

            }else{
                reg->data.boolVal = 1;
            }

            return reg;

        case nil_a: reg->type = nil_m; return reg;

        case userfunc_a:
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;

        case libfunc_a:
            reg->type = libfunc_m;
            reg->data.libfuncVal = libfuncs_getused(arg->val);
            return reg;

        default: assert(0);
    }
}

void execute_arithmetic(struct instruction* instr){
    struct avm_memcell *lv, *rv1, *rv2;

    lv = avm_translate_operand(&instr->result, (struct avm_memcell*) 0);
    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(lv);
    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(stderr, "not a number in arithmetic!\n");
        executionFinished = 1;

    }else{
        avm_memcellclear(lv);
        lv->type = number_m;

        switch(instr->opcode){
            case add_v:
                lv->data.numVal = (rv1->data.numVal + rv2->data.numVal);
                break;
            case sub_v:
                lv->data.numVal = (rv1->data.numVal - rv2->data.numVal);
                break;
            case mul_v:
                lv->data.numVal = (rv1->data.numVal * rv2->data.numVal);
                break;
            case div_v:
                if(rv2->data.numVal == 0){
                    fprintf(stderr, "div by 0\n");
                    executionFinished = 1;
                    return;
                }
                lv->data.numVal = (rv1->data.numVal / rv2->data.numVal);
                break;
            case mod_v:
                if(rv2->data.numVal == 0){
                    fprintf(stderr, "mod by 0\n");
                    executionFinished = 1;
                    return;
                }
                lv->data.numVal = ((unsigned int)rv1->data.numVal % (unsigned int)rv2->data.numVal);
                break;
        }
    }
}

void execute_assign(struct instruction* instr){
    struct avm_memcell *lv, *rv;

    lv = avm_translate_operand(&instr->result, (struct avm_memcell*) 0);
    rv = avm_translate_operand(&instr->arg1, &ax);

    assert(lv);
    assert(rv);

    avm_assign(lv,rv);
}

void execute_call(struct instruction* instr){
    char* s;
    struct avm_memcell* func = avm_translate_operand(&instr->arg1, &ax);
    assert(func);
    avm_callsaveenviroment();

    switch(func->type){
        case userfunc_m:
            pc = func->data.funcVal;
            assert(pc < AVM_ENDING_PC);
            assert(code[pc].opcode == funcenter_v);
            break;

        case string_m:
            avm_calllibfunc(func->data.strVal);
            break;

        case libfunc_m:
            avm_calllibfunc(func->data.libfuncVal);
            break;

        default:
            s = avm_tostring(func);
            fprintf(stderr, "call: cannot bind: \'%s\' to function!\n", s);
            if(s) free(s);
            executionFinished = 1;
    }
}

void execute_funcenter(struct instruction* instr){ // to be tested
    struct userfunc_array* funcInfo;
    struct avm_memcell* func = avm_translate_operand(&instr->arg1, &ax);
    assert(func);
    assert(pc == func->data.funcVal);

    totalActuals = 0;
    funcInfo = avm_getfuncinfo(pc);
    topsp = top;
    top = top - funcInfo->localSize;
}

void execute_funcexit(struct instruction* instr){
    unsigned int oldTop = top;

    top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
    topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);

    while(++oldTop <= top) avm_memcellclear(&stack[oldTop]);
}

void execute_pusharg(struct instruction* instr){
    struct avm_memcell* arg = avm_translate_operand(&instr->arg1, &ax);
    assert(arg);

    avm_assign(&stack[top], arg);
    totalActuals++;
    avm_dec_top();
}

void execute_jeq(struct instruction* instr){
    struct avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    if(rv1->type == undef_m || rv2->type == undef_m){
        fprintf(stderr, "\'undef\' involved in equality!\n");
        executionFinished = 1;
    } 
    else if(rv1->type == nil_m || rv2->type == nil_m) result = (rv1->type == nil_m && rv2->type == nil_m);
    else if(rv1->type == bool_m || rv2->type == bool_m) result = (avm_tobool(rv1) == avm_tobool(rv2));
    else if(rv1->type != rv2->type){
        fprintf(stderr, "%s == %s is illegal!\n", typeString[rv1->type], typeString[rv2->type]);
        executionFinished = 1;
    } 
    else{
        switch(rv1->type){
            case number_m:
                result = (rv1->data.numVal == rv2->data.numVal);
                break;
            case string_m:
                result = (!strcmp(rv1->data.strVal, rv2->data.strVal));
                break;
            case table_m:
                result = (rv1->data.tableVal == rv2->data.tableVal);
                break;
            case userfunc_m:
                result = (rv1->data.funcVal == rv2->data.funcVal);
                break;
            case libfunc_m:
                result = (!strcmp(rv1->data.strVal, rv2->data.strVal));
                break;
            default:
                assert(0);
        }
    }

    if(!executionFinished && result) pc = instr->result.val;
}

void execute_jne(struct instruction* instr){
    struct avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    if(rv1->type == undef_m || rv2->type == undef_m){
        fprintf(stderr, "\'undef\' involved in equality!\n");
        executionFinished = 1;
    } 
    else if(rv1->type == nil_m || rv2->type == nil_m) result = !(rv1->type == nil_m && rv2->type == nil_m);
    else if(rv1->type == bool_m || rv2->type == bool_m) result = !(avm_tobool(rv1) == avm_tobool(rv2));
    else if(rv1->type != rv2->type){
        fprintf(stderr, "%s == %s is illegal!\n", typeString[rv1->type], typeString[rv2->type]);
        executionFinished = 1;
    } 
    else{
        switch(rv1->type){
            case number_m:
                result = (rv1->data.numVal != rv2->data.numVal);
                break;
            case string_m:
                result = (strcmp(rv1->data.strVal, rv2->data.strVal));
                break;
            case table_m:
                result = (rv1->data.tableVal != rv2->data.tableVal);
                break;
            case userfunc_m:
                result = (rv1->data.funcVal != rv2->data.funcVal);
                break;
            case libfunc_m:
                result = (strcmp(rv1->data.strVal, rv2->data.strVal));
                break;
            default:
                assert(0);
        }
    }

    if(!executionFinished && result) pc = instr->result.val;
}

void execute_jle(struct instruction* instr){
    struct avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(stderr, "not a number in jle!\n");
        executionFinished = 1;

    }else{
        result = (rv1->data.numVal <= rv2->data.numVal);
    }

    if(!executionFinished && result) pc = instr->result.val;
}

void execute_jge(struct instruction* instr){
    struct avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(stderr, "not a number in jge!\n");
        executionFinished = 1;

    }else{
        result = (rv1->data.numVal >= rv2->data.numVal);
    }

    if(!executionFinished && result) pc = instr->result.val;
}

void execute_jlt(struct instruction* instr){
    struct avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(stderr, "not a number in jlt!\n");
        executionFinished = 1;

    }else{
        result = (rv1->data.numVal < rv2->data.numVal);
    }

    if(!executionFinished && result) pc = instr->result.val;
}

void execute_jgt(struct instruction* instr){
    struct avm_memcell *rv1, *rv2;
    unsigned char result = 0;

    assert(instr->result.type == label_a);

    rv1 = avm_translate_operand(&instr->arg1, &ax);
    rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(rv1 && rv2);

    if(rv1->type != number_m || rv2->type != number_m){
        fprintf(stderr, "not a number in jgt!\n");
        executionFinished = 1;

    }else{
        result = (rv1->data.numVal > rv2->data.numVal);
    }

    if(!executionFinished && result) pc = instr->result.val;
}

void execute_newtable(struct instruction* instr){
    struct avm_memcell* lv = avm_translate_operand(&instr->arg1, (struct avm_memcell*) 0);
    assert(lv);

    avm_memcellclear(lv);

    lv->type = table_m;
    lv->data.tableVal = avm_tablenew();
    avm_tableincrefcounter(lv->data.tableVal);
}

void execute_tablegetelem(struct instruction* instr){
    struct avm_memcell *lv, *t, *i, *content;
    char *ts, *is;

    lv = avm_translate_operand(&instr->result, (struct avm_memcell*) 0);
    t = avm_translate_operand(&instr->arg1, (struct avm_memcell*) 0);
    i = avm_translate_operand(&instr->arg2, &ax);

    assert(lv);
    assert(t);
    assert(i);

    avm_memcellclear(lv);
    lv->type = nil_m;

    if(t->type != table_m){
        fprintf(stderr, "illegal use of type %s as table\n", typeString[t->type]);
        executionFinished = 1;
    } 
    else{
        content = avm_tablegetelem(t->data.tableVal, i);

        if(content) avm_assign(lv, content);
        else{
            ts = avm_tostring(t);
            is = avm_tostring(i);
            fprintf(stderr, "%s[%s] not found!\n", ts, is);
            if(ts) free(ts);
            if(is) free(is);
        }
    }
    
}

void execute_tablesetelem(struct instruction* instr){
    struct avm_memcell *t, *i, *c;

    t = avm_translate_operand(&instr->result, (struct avm_memcell*) 0);
    i = avm_translate_operand(&instr->arg1, &ax);
    c = avm_translate_operand(&instr->arg2, &bx);

    assert(t && i && c);
   
    if(t->type != table_m){
        fprintf(stderr, "illegal use of type %s as table!\n", typeString[t->type]);
        executionFinished = 1;
    } 
    else avm_tablesetelem(t->data.tableVal, i, c);
}

void execute_jump(struct instruction* instr){ 

    if(instr->result.val > codeSize){
        fprintf(stderr, "jump not to an instruction!\n");
        executionFinished = 1;
    } 
    else pc = instr->result.val;
}
