#include "process.hh"

void process2::write_value ()
{
    sc_uint<32> in_value; //Value from the fifo
    uint temp_in;
    sc_uint<32> write_enable; //Becomes zero, when we may write.
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(0, SC_NS);
    int index = 0;
	
    while( true )
    {
	while (values_available == 0)
	{
	    wait();
	}
	in_value = values[index++];
	values_available--;
	//How long the fifo communication takes
	wait( P1_P2_DELAY, SC_NS );
		
	//Wait until we get write enable
	//NOTICE: accessing shared memory WILL take clock cycles!
	do
	{
	    wait();
	    write_enable = memory->read( ENABLE_INDEX );
	}
	while ( write_enable != 0 );
        
	//Write value, set read enable
	memory->write( in_value, VALUE_INDEX );
	memory->write( 1, ENABLE_INDEX );
    }
}

void process2::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64 adr = trans.get_address();
    unsigned char* ptr = trans.get_data_ptr();
    unsigned int len = trans.get_data_length();
    unsigned char* byt = trans.get_byte_enable_ptr();
    unsigned int width = trans.get_streaming_width();

    if ( cmd == tlm::TLM_READ_COMMAND )
    {
	memcpy(ptr, &values[adr], len);
    }
    else if ( cmd == tlm::TLM_WRITE_COMMAND )
    {
	memcpy(&values[adr], ptr, len);
	values_available++;
    }

    trans.set_response_status( tlm::TLM_OK_RESPONSE );
}
