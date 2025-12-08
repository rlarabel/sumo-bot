#include "io.h"
#include "../common/defines.h"

void io_set_select(io_e io, io_sel_e select)
{
    UNUSED(io);
    UNUSED(select);
    // TODO: Implement reg select
}

void io_set_direction(io_e io, io_dir_e direction)
{
    UNUSED(io);
    UNUSED(direction);
    // TODO: Implement reg direction
}

void io_set_resistor(io_e io, io_res_e resistor)
{
    UNUSED(io);
    UNUSED(resistor);
    // TODO: Implement resistor enable
}

void io_set_out(io_e io, io_out_e out)
{
    UNUSED(io);
    UNUSED(out);
    // TODO: Implement output/resistor signal
}

io_in_e io_get_input(io_e io)
{
    UNUSED(io);
    // TODO: Implement input reads
    return 0;
}