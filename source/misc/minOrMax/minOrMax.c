#include "ext.h"
#define MAX_SIZE 1024
#include "minOrMax.h"

void * this_class;


/************************************ MAIN **********************************************************/

void ext_main(void * r){
    setup((t_messlist **) &this_class, (method)minOrMax_new, 0L, (short)sizeof(MinMax), 0L, A_DEFLONG, 0);
    addmess((method)minOrMax_list, "list", A_GIMME, 0);
    addinx((method)minOrMax_in1, 1);
    addbang((method)minOrMax_bang);
}




/************************************ IMPLEMENTATION ************************************************/

void * minOrMax_new(long value){
    MinMax * x;
    x = (MinMax *)newobject(this_class);
    intin(x,1); // create the right inlet
    x->am_out = intout(x); // create the outlet
    x->am_valright = value;
    x->am_count = 0;
    return(x);
}


void minOrMax_list(MinMax * x, Symbol * s, short ac, Atom * av){
    // if list too long, then shorten the list
    if(ac >= MAX_SIZE)
        ac = MAX_SIZE;
    for(int i=0; i<ac; i++){
        x->am_list[i] = av[i].a_w.w_long;
    }
    x->am_count = ac;
    minOrMax_bang(x);
}


void minOrMax_in1(MinMax * x, long value){
    x->am_valright = value;
}


void minOrMax_bang(MinMax * x){
    long res = x->am_list[0];
    for(int i=0; i<x->am_count-1; i++){
        res = minOrMax_calculate(x, res, x->am_list[i+1]);
    }
    outlet_int(x->am_out, res);
}


long minOrMax_calculate(MinMax * x, long v1, long v2){
    if(x->am_valright < 0){
        if(v1 < v2)
            return v1;
        else
            return v2;
    }
    else{
        if(v1 > v2)
            return v1;
        else
            return v2;
    }
}



