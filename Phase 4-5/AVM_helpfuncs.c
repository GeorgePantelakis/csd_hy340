// AVM HELPFUNCS
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "AVM.h"


void avm_tableincrefcounter(struct avm_table* t){
    t->refCounter++;
}

void avm_tabledecrefcounter(struct avm_table* t){
    assert(t->refCounter > 0);
    t->refCounter--;
    if(!t->refCounter) avm_tabledestroy(t);
}

void avm_tablebucketsinit(struct avm_table_bucket** p){
    unsigned int i;
    
    for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
        p[i] = NULL;
    }
}

struct avm_table* avm_tablenew(){
    
    struct avm_table* t = (struct avm_table*)malloc(sizeof(struct avm_table));
    AVM_WIPEOUT(*t);
    
    t->refCounter = 0;
    t->total = 0;
    
    avm_tablebucketsinit(t->numIndexed);
    avm_tablebucketsinit(t->strIndexed);
    avm_tablebucketsinit(t->userfuncIndexed);
    avm_tablebucketsinit(t->libfuncIndexed);
    avm_tablebucketsinit(t->boolIndexed);
    
    return t;
}

struct avm_memcell* avm_tablegetelem(struct avm_table* table, struct avm_memcell* index){
    unsigned int hashtableindex;
    struct avm_table_bucket *tmp;

    assert(table && index);
    hashtableindex = hash_function(avm_tostring(index));
    assert(hashtableindex < AVM_TABLE_HASHSIZE);

    switch(index->type){
        case number_m:
            tmp = table->numIndexed[hashtableindex];
            while(tmp){
                if(tmp->key->data.numVal == index->data.numVal){
                    return tmp->value;
                }
            }
            break;
        case string_m:
            tmp = table->strIndexed[hashtableindex];
            while(tmp){
                if(!strcmp(tmp->key->data.strVal, index->data.strVal)){
                    return tmp->value;
                }
            }
            break;
        case userfunc_m:
            tmp = table->userfuncIndexed[hashtableindex];
            while(tmp){
                if(tmp->key->data.funcVal == index->data.funcVal){
                    return tmp->value;
                }
            }
            break;
        case libfunc_m:
            tmp = table->libfuncIndexed[hashtableindex];
            while(tmp){
                if(!strcmp(tmp->key->data.libfuncVal, index->data.libfuncVal)){
                    return tmp->value;
                }
            }
            break;
        case bool_m:
            tmp = table->boolIndexed[hashtableindex];
            while(tmp){
                if(tmp->key->data.boolVal == index->data.boolVal){
                    return tmp->value;
                }
            }
            break;
        default:
            assert(0);
    }
    
    return NULL;
}

void avm_tablesetelem(struct avm_table* table, struct avm_memcell *index, struct avm_memcell *content){
    unsigned int hashtableindex;
    struct avm_table_bucket *tmp, *newnode;

    assert(table && index && content);
    hashtableindex = hash_function(avm_tostring(index));
    assert(hashtableindex < AVM_TABLE_HASHSIZE);
    
    newnode = (struct avm_table_bucket*)malloc(sizeof(struct avm_table_bucket));
    newnode->key = (struct avm_memcell*)malloc(sizeof(struct avm_memcell));
    newnode->value = (struct avm_memcell*)malloc(sizeof(struct avm_memcell));
    avm_assign(newnode->key, index);
    avm_assign(newnode->value, content);
    newnode->next = NULL;

    switch(index->type){
        case number_m:
            tmp = table->numIndexed[hashtableindex];
            while(tmp){
                if(tmp->key->data.numVal == index->data.numVal){
                    avm_assign(tmp->value, content);
                    return;
                }
            }
            
            if(!(table->numIndexed[hashtableindex])){
                table->numIndexed[hashtableindex] = newnode;
                return;
            }else{
                tmp = table->numIndexed[hashtableindex];
            }

            break;
        case string_m:
            tmp = table->strIndexed[hashtableindex];
            while(tmp){
                if(!strcmp(tmp->key->data.strVal, index->data.strVal)){
                    avm_assign(tmp->value, content);
                    return;
                }
            }

            if(!(table->strIndexed[hashtableindex])){
                table->strIndexed[hashtableindex] = newnode;
                return;
            }else{
                tmp = table->strIndexed[hashtableindex];
            }
            
            break;
        case userfunc_m:
            tmp = table->userfuncIndexed[hashtableindex];
            while(tmp){
                if(tmp->key->data.funcVal == index->data.funcVal){
                    avm_assign(tmp->value, content);
                    return;
                }
            }
            
            if(!(table->userfuncIndexed[hashtableindex])){
                table->userfuncIndexed[hashtableindex] = newnode;
                return;
            }else{
                tmp = table->userfuncIndexed[hashtableindex];
            }

            break;
        case libfunc_m:
            tmp = table->libfuncIndexed[hashtableindex];
            while(tmp){
                if(!strcmp(tmp->key->data.libfuncVal, index->data.libfuncVal)){
                    avm_assign(tmp->value, content);
                    return;
                }
            }
            
            if(!(table->libfuncIndexed[hashtableindex])){
                table->libfuncIndexed[hashtableindex] = newnode;
                return;
            }else{
                tmp = table->libfuncIndexed[hashtableindex];
            }

            break;
        case bool_m:
            tmp = table->boolIndexed[hashtableindex];
            while(tmp){
                if(tmp->key->data.boolVal == index->data.boolVal){
                    avm_assign(tmp->value, content);
                    return;
                }
            }
            
            if(!(table->boolIndexed[hashtableindex])){
                table->boolIndexed[hashtableindex] = newnode;
                return;
            }else{
                tmp = table->boolIndexed[hashtableindex];
            }

            break;
        default:
            assert(0);
    }
    
    while(tmp->next) tmp = tmp->next;
    tmp->next = newnode;
    
}


void avm_tablebucketsdestroy(struct avm_table_bucket** p){
    unsigned int i;
    struct avm_table_bucket* del;
    struct avm_table_bucket* b;

    for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
        b = *p;     
        while(b){
            del = b;
            b = b->next;
            avm_memcellclear(del->key);
            avm_memcellclear(del->value);
            if(del) free(del);
        }
        p[i] = NULL;
        p++;
    }   
}

void avm_tabledestroy(struct avm_table* t){
    avm_tablebucketsdestroy(t->strIndexed);
    avm_tablebucketsdestroy(t->numIndexed);
    avm_tablebucketsdestroy(t->userfuncIndexed);
    avm_tablebucketsdestroy(t->libfuncIndexed);
    avm_tablebucketsdestroy(t->boolIndexed);
    //if(t) free(t);
}

void avm_assign(struct avm_memcell* lv, struct avm_memcell* rv){
    
    if(lv == rv) return;
    
    if(lv->type == table_m && rv->type == table_m && lv->data.tableVal == rv->data.tableVal) return;
    
    if(rv->type == undef_m) fprintf(stderr, "assigning from \'undef\' content!\n");
    
    avm_memcellclear(lv);
    
    memcpy(lv, rv, sizeof(struct avm_memcell));
    
    if(lv->type == string_m) lv->data.strVal = strdup(rv->data.strVal);
    else if(lv->type == table_m) avm_tableincrefcounter(lv->data.tableVal);
    
}

unsigned int avm_get_envvalue(unsigned int i){
    assert(stack[i].type == number_m);
    unsigned int val = (unsigned int) stack[i].data.numVal;
    assert(stack[i].data.numVal == ((double) val));
    return val;
}

char* avm_tostring(struct avm_memcell* m){
    char *result;

    assert(m->type >= 0 && m->type <= undef_m);
    switch(m->type){
        case number_m:
            result = number_tostring(m);
            break;
        case string_m:
            result = string_tostring(m);
            break;
        case bool_m:
            result = bool_tostring(m);
            break;
        case table_m:
            result = table_tostring(m);
            break;
        case userfunc_m:
            result = userfunc_tostring(m);
            break;
        case libfunc_m:
            result = libfunc_tostring(m);
            break;
        case nil_m:
            result = nil_tostring(m);
            break;
        case undef_m:
            result = undef_tostring(m);
            break;
    }

    return result;
}

char* number_tostring(struct avm_memcell* m){
    char *result;

    result = (char *)malloc(AVM_STANDARD_STRING_SIZE * sizeof(char));

    sprintf(result, "%.3lf", m->data.numVal);

    return result;
}

char* string_tostring(struct avm_memcell* m){
    char *result;

    result = strdup(m->data.strVal);
    
    return result;
}

char* bool_tostring(struct avm_memcell* m){
    char *result;

    if(m->data.boolVal == 1)
        sprintf(result, "TRUE");
    else if (m->data.boolVal == 0)
        sprintf(result, "FALSE");
    else{
        fprintf(stderr, "Unknown type of bool \'%c\'.\n", m->data.boolVal);
        executionFinished = 1;
    }
        

    return result;
}

char* table_tostring(struct avm_memcell* m){
    char *result = (char*)malloc(sizeof(char));
    struct avm_table_bucket **tmp_num, **tmp_str, **tmp_userfunc, **tmp_libfunc, **tmp_bool;

    strcpy(result, "[");

    tmp_num = m->data.tableVal->numIndexed;
    tmp_str = m->data.tableVal->strIndexed;
    tmp_userfunc = m->data.tableVal->userfuncIndexed;
    tmp_libfunc = m->data.tableVal->libfuncIndexed;
    tmp_bool = m->data.tableVal->boolIndexed;

    bucket_runner(tmp_num, result);
    bucket_runner(tmp_str, result);
    bucket_runner(tmp_userfunc, result);
    bucket_runner(tmp_libfunc, result);
    bucket_runner(tmp_bool, result);

    strcat(result, "]");

    return result;
}

void bucket_runner(struct avm_table_bucket** tmp, char* result){
    unsigned int i;
    struct avm_table_bucket* list_runner;

    for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
        if(tmp[i]){
            list_runner = tmp[i];
            while(list_runner){
                key_and_value_print(list_runner, result);
                list_runner = list_runner->next;
            }
        }
    }

}

void key_and_value_print(struct avm_table_bucket* tmp, char* result){
    strcat(result, " { ");
    strcat(result, avm_tostring(tmp->key));
    strcat(result, " : ");

    if(tmp->value->type == table_m) strcat(result, avm_tostring(tmp->value));
    else strcat(result, avm_tostring(tmp->value));

    strcat(result, " } ");
}

char* userfunc_tostring(struct avm_memcell* m){
    char *result;

    result = (char *)malloc(AVM_STANDARD_STRING_SIZE * sizeof(char));

    sprintf(result, "user function at %d", m->data.funcVal);

    return result;
}

char* libfunc_tostring(struct avm_memcell* m){
    char *result;

    result = (char *)malloc(AVM_STANDARD_STRING_SIZE * sizeof(char));

    sprintf(result, "library function: %s", m->data.libfuncVal);

    return result;
}

char* nil_tostring(struct avm_memcell* m){
    char *result;

    result = (char *)malloc(AVM_STANDARD_STRING_SIZE * sizeof(char));

    sprintf(result, "NIL");

    return result;
}

char* undef_tostring(struct avm_memcell* m){
    char *result;

    result = (char *)malloc(AVM_STANDARD_STRING_SIZE * sizeof(char));

    sprintf(result, "UNDEFINED");

    return result;
}

void avm_calllibfunc(char* id){
    unsigned int flag = 0;

    if(strcmp(id, "print") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_print();

    }else if(strcmp(id, "input") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_input();

    }else if(strcmp(id, "objectmemeberkeys") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_objectmemberkeys();

    }else if(strcmp(id, "objecttotalmembers") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_objecttotalmembers();

    }else if(strcmp(id, "objectcopy") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_objectcopy();

    }else if(strcmp(id, "totalarguments") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_totalarguments();

    }else if(strcmp(id, "argument") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_arguments();

    }else if(strcmp(id, "typeof") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_typeof();

    }else if(strcmp(id, "strtonum") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_strtonum();

    }else if(strcmp(id, "sqrt") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_sqrt();

    }else if(strcmp(id, "cos") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_cos();

    }else if(strcmp(id, "sin") == 0){
        topsp = top;
        totalActuals = 0;
        flag = 1;
        libfunc_sin();

    }else{
        fprintf(stderr, "unsupported lib func \'%s\' called!\n", id);
        executionFinished = 1;
    }

    if(flag && !executionFinished){
        execute_funcexit((struct instruction*) 0);
    }
}

void libfunc_print(){
    char *s;
    unsigned int i ,n = avm_totalactuals();

    for(i = 0; i < n; i++){
        s = avm_tostring(avm_getactual(i));
        if(s){
            fprintf(stdout, "%s", s);
        } 
    }
    fprintf(stdout, "\n");
}

void libfunc_input(){
    char *buffer;
    double number;

    buffer = (char *)malloc(100 * sizeof(char));

    read_from_file(stdin, buffer, 100, 1);

    number = atof(buffer);

    avm_memcellclear(&retval);

    if(number != 0){
        retval.type = number_m;
        retval.data.numVal = number;
        if(buffer) free(buffer);
        return;
    }else{
        if(!strcmp(buffer, "0")){
            retval.type = number_m;
            retval.data.numVal = 0;
            if(buffer) free(buffer);
            return;
        }
    }

    if(!strcmp(buffer, "true")){
        retval.type = bool_m;
        retval.data.boolVal = 1;
        if(buffer) free(buffer);
        return;
    }else if(!strcmp(buffer, "false")){
        retval.type = bool_m;
        retval.data.boolVal = 0;
        if(buffer) free(buffer);
        return;
    }else if(!strcmp(buffer, "nil")){
        retval.type = nil_m;
        if(buffer) free(buffer);
        return;
    }

    retval.type = string_m;
    retval.data.strVal = strdup(buffer);

    if(buffer) free(buffer);
}

void libfunc_objectmemberkeys(){
    unsigned int n = avm_totalactuals();
    struct avm_memcell *temp;
    int i;
    struct avm_table_bucket *current_bucket;
    struct avm_table* newTable;
    struct avm_memcell *index;

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'objectmemberkeys\'!\n", n);
        executionFinished = 1;
    }else{
        avm_memcellclear(&retval);
        temp = avm_getactual(0);
        if(temp->type != table_m){
            retval.type = nil_m;
            return;
        }else{
            newTable = avm_tablenew();
            index = (struct avm_memcell *)malloc(sizeof(struct avm_memcell));
            index->type = number_m;
            index->data.numVal = 0;
            for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
                current_bucket = temp->data.tableVal->strIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, index, current_bucket->key);
                    index->data.numVal++;
                    current_bucket = current_bucket->next;
                }
            }
            for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
                current_bucket = temp->data.tableVal->numIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, index, current_bucket->key);
                    index->data.numVal++;
                    current_bucket = current_bucket->next;
                }
            }
            for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
                current_bucket = temp->data.tableVal->boolIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, index, current_bucket->key);
                    index->data.numVal++;
                    current_bucket = current_bucket->next;
                }
            }
            for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
                current_bucket = temp->data.tableVal->libfuncIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, index, current_bucket->key);
                    index->data.numVal++;
                    current_bucket = current_bucket->next;
                }
            }
            for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
                current_bucket = temp->data.tableVal->userfuncIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, index, current_bucket->key);
                    index->data.numVal++;
                    current_bucket = current_bucket->next;
                }
            }
        }
    }
}

void libfunc_objecttotalmembers(){
    unsigned int n = avm_totalactuals();
    struct avm_memcell *temp;

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'objecttotalmembers\'!\n", n);
        executionFinished = 1;
    }else{
        avm_memcellclear(&retval);
        temp = avm_getactual(0);
        if(temp->type == table_m){
            retval.type = number_m;
            retval.data.numVal = (double)temp->data.tableVal->total;
        }else{
            retval.type = number_m;
            retval.data.numVal = 0;
        }
    }
}

void libfunc_objectcopy(){
    unsigned int n = avm_totalactuals();
    struct avm_memcell *temp;
    int i;
    struct avm_table_bucket *current_bucket;
    struct avm_table* newTable;

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'objectcopy\'!\n", n);
        executionFinished = 1;
    }else{
        avm_memcellclear(&retval);
        temp = avm_getactual(0);
        if(temp->type != table_m){
            retval.type = nil_m;
            return;
        }else{
            newTable = avm_tablenew();
            for(i = 0; i < AVM_TABLE_HASHSIZE; i++){
                current_bucket = temp->data.tableVal->strIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, current_bucket->key, current_bucket->value);
                    current_bucket = current_bucket->next;
                }
                current_bucket = temp->data.tableVal->numIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, current_bucket->key, current_bucket->value);
                    current_bucket = current_bucket->next;
                }
                current_bucket = temp->data.tableVal->boolIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, current_bucket->key, current_bucket->value);
                    current_bucket = current_bucket->next;
                }
                current_bucket = temp->data.tableVal->libfuncIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, current_bucket->key, current_bucket->value);
                    current_bucket = current_bucket->next;
                }
                current_bucket = temp->data.tableVal->userfuncIndexed[i];
                while(current_bucket != NULL){
                    avm_tablesetelem(newTable, current_bucket->key, current_bucket->value);
                    current_bucket = current_bucket->next;
                }
            }
        }
    }
}

void libfunc_totalarguments(){
    unsigned int p_topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
    avm_memcellclear(&retval);

    if(!p_topsp){
        fprintf(stderr, "\'totalarguments\' called outside a function!\n");
        retval.type = nil_m;

    }else{
        retval.type = number_m;
        retval.data.numVal = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET);
    }
}

void libfunc_arguments(){
    unsigned int n = avm_totalactuals();
    struct avm_memcell *temp;
    int arg_number;
    unsigned int p_topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'argument\'!\n", n);
        executionFinished = 1;
    }else{
        avm_memcellclear(&retval);
        if(!p_topsp){
            fprintf(stderr, "\'argument\' called outside a function!\n");
            retval.type = nil_m;
            return;
        }else{
            temp = avm_getactual(0);
            if(temp->type != number_m){
                retval.type = nil_m;
                return;
            }
            arg_number = temp->data.numVal;
            if(arg_number >= avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET)){
                retval.type = nil_m;
                return;
            }
            temp = &stack[topsp + AVM_STACKENV_SIZE + 1 + arg_number];
            avm_assign(&retval, temp);
        }
    }
}

void libfunc_typeof(){
    unsigned int n = avm_totalactuals();

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'typeof\'!\n", n);
        executionFinished = 1;
    }else{
        avm_memcellclear(&retval);
        retval.type = string_m;
        retval.data.strVal = strdup(typeString[avm_getactual(0)->type]);
    }
}

void libfunc_strtonum(){
    unsigned int n = avm_totalactuals();
    double result;

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'strtonum\'!\n", n);
        executionFinished = 1;
    }else{
        avm_memcellclear(&retval);
        result = atof(avm_tostring(avm_getactual(0)));
        if(result == (double) 0){
            if(!strcmp(avm_tostring(avm_getactual(0)), "0")){
                retval.type = number_m;
                retval.data.numVal = result;
            }else{
                retval.type = nil_m;
            }
        }else{
            retval.type = number_m;
            retval.data.numVal = result;
        }
    }
}

void libfunc_sqrt(){
    unsigned int n = avm_totalactuals();
    double result;
    struct avm_memcell *temp;

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'sqrt\'!\n", n);
        executionFinished = 1;
    } 
    else{
        avm_memcellclear(&retval);
        temp = avm_getactual(0);
        if(temp->type == number_m){
            if(temp->data.numVal >= 0){
                result = sqrt(temp->data.numVal);
                retval.type = number_m;
                retval.data.numVal = result;
            }else{
                retval.type = nil_m;
            }
        }else{
            retval.type = nil_m;
        }
    }
}

void libfunc_cos(){
    unsigned int n = avm_totalactuals();
    double result;
    struct avm_memcell *temp;

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'cos\'!\n", n);
        executionFinished = 1;
    }else{
        avm_memcellclear(&retval);
        temp = avm_getactual(0);
        if(temp->type == number_m){
            result = cos((temp->data.numVal) * (PI / 180.0));
            retval.type = number_m;
            retval.data.numVal = result;
        }else{
            retval.type = nil_m;
        }
    }
}

void libfunc_sin(){
    unsigned int n = avm_totalactuals();
    double result;
    struct avm_memcell *temp;

    if(n != 1){
        fprintf(stderr, "one argument (not %d) expected in \'sin\'!\n", n);
        executionFinished = 1;
    }else{
        avm_memcellclear(&retval);
        temp = avm_getactual(0);
        if(temp->type == number_m){
            result = sin((temp->data.numVal) * (PI / 180.0));
            retval.type = number_m;
            retval.data.numVal = result;
        }else{
            retval.type = nil_m;
        }
    }
}

unsigned int avm_totalactuals(){
    return avm_get_envvalue(topsp + AVM_NUMACTUALS_OFFSET);
}

struct avm_memcell* avm_getactual(unsigned int i){
    assert(i < avm_totalactuals());
    return &stack[topsp + AVM_STACKENV_SIZE + 1 + i];
}

unsigned char avm_tobool(struct avm_memcell* m){
    assert(m->type >= 0 && m->type < undef_m);

    switch(m->type){
        case number_m:
            return m->data.numVal != 0;
        case string_m:
            return m->data.strVal[0] != 0;
        case bool_m:
            return m->data.boolVal;
        case table_m:
            return 1;
        case userfunc_m:
            return 1;
        case libfunc_m:
            return 1;
        case nil_m:
            return 0;
        case undef_m:
            assert(0);
        default: 
            assert(0);
    }
}

void read_from_file(FILE* file, char* buffer, int max_input, int abort_on_newline) {
    char ch;
    int index;

    for(index = 0; index < (max_input - 1); index++){
        ch = getc(stdin);

        if (ch == EOF) {
            break;
        }

        if (abort_on_newline && ch == '\n') {
            break;
        }

        if(ch < 32){
            index--;
            continue;
        }

        buffer[index] = ch;
    }

    buffer[index] = '\0';
}

double consts_getnumber(unsigned int index){
    assert(index < const_number_array_size);
    return const_number_array[index];
}

char* consts_getstring(unsigned int index){
    assert((index < const_string_array_size) && (const_string_array[index]));
    return const_string_array[index];
}

char* libfuncs_getused(unsigned int index){
    assert((index < libfunc_array_size) && (libfunc_array[index]));
    return libfunc_array[index];
}

struct userfunc_array* avm_getfuncinfo(unsigned int address){
    unsigned int i;

    for(i = 0; i < userfunc_array_size; i++){
        if(userfunc_array[i].address == address) return &(userfunc_array[i]);
    }
}

unsigned int hash_function(char* string){
    unsigned int hash = 5381;
    int c;

    while (c = *string++)
        hash = ((hash << 5) + hash) + c;

    return (hash % AVM_TABLE_HASHSIZE);
}