#include "ext.h" 

void *this_class;


typedef struct _abs {
    t_object a_ob;
    short a_mode;
    void *a_out;
} t_abs;



void *abs_new(Symbol *s, short ac, Atom *av);
void abs_int(t_abs *abs, long value);
void abs_flt(t_abs *abs, float value);





void ext_main(void * r)
{
    setup((t_messlist **) &this_class, (method)abs_new, 0L, (short)sizeof(t_abs), 0L, A_GIMME, 0);
    addint((method)abs_int);
    addfloat((method)abs_flt);
}




void *abs_new(Symbol *s, short ac, t_atom *av) {
    t_abs *abs = (t_abs *)newobject(this_class);
    abs->a_mode=A_LONG;
    
    if (ac && av[0].a_type == A_FLOAT) {
        abs->a_out = floatout(abs);
        abs->a_mode = A_FLOAT;
    }
    else
        abs->a_out = intout(abs);
    return(abs);
}




void abs_int(t_abs *abs, long value) {
    if (abs->a_mode == A_LONG)
        outlet_int(abs->a_out, (value >= 0) ? value: -value);
    else
        outlet_float(abs->a_out, (float)((value >= 0) ? value: -value));
}


void abs_flt(t_abs *abs, float value) {
    if (abs->a_mode == A_LONG)
        outlet_int(abs->a_out, (long)((value >= 0) ? value: -value));
    else
        outlet_float(abs->a_out, (value >= 0) ? value: -value);
}