#include "ext.h" 
#include "ext_mess.h"
void *class;
#define MAXSIZE 32



#define SETFLOAT(ap, x) ((ap)->a_type = A_FLOAT, (ap)->a_w.w_float = (x))
#define SETLONG(ap, x) ((ap)->a_type = A_LONG, (ap)->a_w.w_long = (x))



typedef struct minimum {
    t_object m_ob;
    Atom m_args[MAXSIZE];
    long m_count;
    short m_incount;
    short m_outtype;
    void *m_out;
} Minimum;


void DoAtomMin(Atom *min, Atom *new);
void minimum_bang(Minimum *x);
void minimum_int(Minimum *x, long n);
void minimum_in1(Minimum *x, long n);
void minimum_float(Minimum *x, double f);
void minimum_ft1(Minimum *x, double f);
void minimum_list(Minimum *x, Symbol *s, short ac, Atom *av);
void minimum_assist(Minimum *x, void *b, long m, long a, char *s);
void *minimum_new(Symbol *s, short ac, Atom *av);




void ext_main(void * r){
    setup((t_messlist **)&class, (method)minimum_new,0L, (short)sizeof(Minimum), 0L, A_GIMME, 0);
    addbang((method)minimum_bang);
    addint((method)minimum_int);
    addinx((method)minimum_in1, 1);
    addfloat((method)minimum_float);
    addftx((method)minimum_ft1, 1);
    addmess((method)minimum_list, "list", A_GIMME, 0);
    addmess((method)minimum_assist, "assist", A_CANT, 0);
    finder_addclass("Arith/Logic/Bitwise", "minimum");
}


void DoAtomMin(Atom *min, Atom *new){
    if (min->a_type == A_NOTHING){
        *min = *new;
        return;
    }
    if (min->a_type == A_FLOAT){
        if (new->a_type == A_FLOAT){
            if (new->a_w.w_float < min->a_w.w_float)
                min->a_w.w_float = new->a_w.w_float;
        }
        else {
            if ((float)new->a_w.w_long < min->a_w.w_float)
                min->a_w.w_float = (float)new->a_w.w_long;
        }
    }
    else{
        if (new->a_type == A_LONG){
            if (new->a_w.w_long < min->a_w.w_long)
                min->a_w.w_long = new->a_w.w_long;
        }
        else{
            if ((long)new->a_w.w_float < min->a_w.w_long)
                min->a_w.w_long = (long)new->a_w.w_float;
        }
    }
}





void minimum_bang(Minimum *x) {
    short i;
    Atom themin;
    long res;
    double fres;
    themin.a_type = A_NOTHING;
    for (i = 0; i < x->m_count; i++)
        DoAtomMin(&themin, x->m_args + i);
        if (x->m_outtype == A_LONG){
            if (themin.a_type == A_LONG)
                res = themin.a_w.w_long;
            else
                res = (long)themin.a_w.w_float;
            outlet_int(x->m_out,res);
        }
        else {
            if (themin.a_type == A_FLOAT)
                fres = themin.a_w.w_float;
            else
                fres = (float)themin.a_w.w_long;
            outlet_float(x->m_out,fres);
        }
}




void minimum_int(Minimum *x, long n) {
    SETLONG(x->m_args,n);
    minimum_bang(x);
}




void minimum_in1(Minimum *x, long n) {
    SETLONG(x->m_args+1,n);
    x->m_count = 2;
}



void minimum_float(Minimum *x, double f) {
    SETFLOAT(x->m_args,f);
    minimum_bang(x);
}


void minimum_ft1(Minimum *x, double f) {
    SETFLOAT(x->m_args+1,f);
    x->m_count = 2;
}




void minimum_list(Minimum *x, Symbol *s, short ac, Atom *av) {
    short i;
    if (ac >= MAXSIZE)
        ac = MAXSIZE - 1;
    for (i = 0; i < ac; i++, av++) {
        if (av->a_type == A_LONG)
            SETLONG(x->m_args + i, av->a_w.w_long);
        else if (av->a_type == A_FLOAT)
            SETFLOAT(x->m_args + i, av->a_w.w_float);
    }
    x->m_count = ac;
    minimum_bang(x);
}




void minimum_assist(Minimum *x, void *b, long m, long a, char *s) {
    // assist_string(ResourceID, m, a, 1, 3, s);
}



void *minimum_new(Symbol *s, short ac, t_atom *av) {
        Minimum *x;
        x = (Minimum *)newobject(class);
        x->m_count = 2;
        if (ac)
        {
            x->m_args[1] = *av;
            if (av->a_type == A_LONG){
                x->m_args[0].a_type = x->m_outtype = A_LONG;
                x->m_out = intout(x);
                x->m_args[0].a_w.w_long = 0;
                intin(x, 1);
            }
            else if (av->a_type == A_FLOAT) {
                x->m_args[0].a_type = x->m_outtype = A_FLOAT;
                x->m_out = floatout(x);
                x->m_args[0].a_w.w_float = 0;
                floatin(x, 1);
            }
        }
        else{
            x->m_outtype = A_LONG;
            intin(x,1);
            x->m_out = intout(x);
            SETLONG(x->m_args + 1, 0L);
            SETLONG(x->m_args, 0L);
        }
    return (x);
}




