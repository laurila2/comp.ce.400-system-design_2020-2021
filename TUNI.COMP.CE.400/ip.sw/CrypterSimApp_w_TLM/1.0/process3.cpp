#include "process.hh"

void process3::read_value()
{
    sc_uint<32> value;
    sc_uint<32> read_enable;
    
    while ( true )
    {
	//Wait until we get read enable.
	//NOTICE: accessing shared memory WILL take clock cycles!
	do
	{
	    wait();
	    read_enable = memory->read( ENABLE_INDEX );
	}
	while ( read_enable == 0 );

	//Read the value from shared memory, set write enable
	value = memory->read( VALUE_INDEX );
	memory->write( 0, ENABLE_INDEX );

	wait( P3_P4_DELAY, SC_NS );
	fifo.write( value );
    }

}

