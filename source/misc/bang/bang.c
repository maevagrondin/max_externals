// bang.c -- A very simple Max external object.
// Outputs a bang when a bang is received.


// initialization (required for all objects)
#include "ext.h"
void *this_class;


// definition of the data structure for the max object
typedef struct _bang {
    t_object b_ob; // references to the object
    void *b_out; // other information
} t_bang;


// Prototypes for methods: need a method for each incoming message
void *bang_new(void); // object creation method (called when a new instance of the object is created in Max)
void bang_bang(t_bang *bang); // method for the bang message (bang sent at the outlet)


void ext_main(void *r){
    // set up our class: create a class definition
    // first arg = global variable representing the object
    // second arg = pointer to the instance cretion method
    // third arg = bang_free replaced by OL because no memory needs to be freed
    // fourth arg = number of bytes required in memory to store the object
    // fifth arg = pointer to menufun (to be ignored here)
    // sixth arg = number of arguments to the object
    setup((t_messlist**) &this_class, (method)bang_new, 0L, (short)sizeof(t_bang), 0L, 0);
    
    // the object only responds to the bang message
    addbang((method)bang_bang); // bind method "bang_bang" to the "bang" message
    return (0);
}


// object creation method (called each time the object is created in Max)
void *bang_new(void) {
    t_bang *bang;
    // create the new instance and return a pointer to it
    bang = (t_bang *)newobject(this_class);
    bang->b_out = bangout(bang); // create a bang outlet (stored in the pointer b_out)
    return(bang);
}


void bang_bang(t_bang *bang) {
    outlet_bang(bang->b_out);
}

