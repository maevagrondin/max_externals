#include "ext.h"
#define MAX_SIZE 1024
#include "minOrMaxObjC.h"
#include <objc/runtime.h>

void * this_class;


/************************************ MAIN **********************************************************/

void ext_main(void * r){
    setup((t_messlist **) &this_class, (method)minOrMaxObjC_new, 0L, (short)sizeof(MinOrMaxObjC), 0L, A_DEFLONG, 0);
    addmess((method)minOrMaxObjC_list, "list", A_GIMME, 0);
    addinx((method)minOrMaxObjC_in1, 1);
    addbang((method)minOrMaxObjC_bang);
}



/************************************ IMPLEMENTATION ************************************************/

void * minOrMaxObjC_new(long value){
    MinOrMaxObjC * x;
    x = (MinOrMaxObjC *)newobject(this_class);
    x->am_objc = [[MinMax alloc]init];
    intin(x,1); // create the right inlet
    x->struct_out = intout(x); // create the outlet
    return (x);
}


void minOrMaxObjC_list(MinOrMaxObjC * x, Symbol * s, short ac, Atom * av){
    [x->am_objc am_list:av andLength:ac];
    minOrMaxObjC_bang(x);
}


void minOrMaxObjC_in1(MinOrMaxObjC * x, long value){
    [x->am_objc am_in1:value];
}


void minOrMaxObjC_bang(MinOrMaxObjC * x){
    [x->am_objc am_bang];
    outlet_int(x->struct_out, [x->am_objc am_getRes]);
}


/************************************ IMPLEMENTATION OBJET ******************************************/

@implementation MinMax

- (void) am_new: (long) value{
    am_valright = value;
    am_count = 0;
}


- (void) am_list:(Atom *)av andLength:(short)ac{
    if(ac >= MAX_SIZE)
        ac = MAX_SIZE;
    for(int i=0; i<ac; i++){
        am_list[i] = av[i].a_w.w_long;
    }
    am_count = ac;
}


- (void) am_in1:(long)value{
    am_valright = value;
}


- (void) am_bang{
    long res = am_list[0];
    for(int i=0; i<am_count-1; i++){
        res = [self am_calculate:res andV2:am_list[i+1]];
    }
    am_res = res;
}


- (long) am_calculate:(long)v1 andV2:(long) v2{
    if(am_valright < 0){
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

- (long) am_getRes{
    return am_res;
}

@end
