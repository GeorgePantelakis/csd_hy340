// AVM STACKFUNCS
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "AVM.h"

void avm_initstack(){
    unsigned int i;

    top = AVM_STACKSIZE - 1 - total_global_vals;
    topsp = AVM_STACKSIZE - 1 - total_global_vals;

    for(i = 0; i < AVM_STACKSIZE; i++){
        AVM_WIPEOUT(stack[i]);
        stack[i].type = undef_m;
    }
}

void avm_memcellclear(struct avm_memcell* m){

    if(m->type == string_m){
        assert(m->data.strVal);
        free(m->data.strVal);

    }else if(m->type == table_m){
        assert(m->data.tableVal);
        avm_tabledecrefcounter(m->data.tableVal);
        
    }
    
    m->type = undef_m;
}

void avm_dec_top(){
    if(!top){
        fprintf(stderr, "Stack Overflow!\n");
        executionFinished = 1;
    }else{
        top--;
    }
}

void avm_push_envvalue(unsigned int val){
    stack[top].type = number_m;
    stack[top].data.numVal = val;
    avm_dec_top();
}

void avm_callsaveenviroment(){
    avm_push_envvalue(totalActuals);
    avm_push_envvalue(pc+1);
    avm_push_envvalue(top + totalActuals + 2);
    avm_push_envvalue(topsp);
}
