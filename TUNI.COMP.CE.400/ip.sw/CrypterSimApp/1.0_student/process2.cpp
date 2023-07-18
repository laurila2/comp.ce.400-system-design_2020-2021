#include "process.hh"

void process2::write_value ()
{
    sc_uint<32> in_value; //Value from the fifo
    int temp_in;
    sc_uint<32> write_enable; //Becomes zero, when we may write.
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    const sc_time delay = sc_time(SC_ZERO_DELAY, SC_NS);
	
    while( true )
    {
	trans->set_command(TLM_READ_COMMAND);
	trans->set_data_length(4);
	trans->set_streaming_width(4);
	trans->set_byte_enable_ptr(0);
	trans->set_dmi_allowed(false);
	trans->set_data_ptr(reinterpret_cast<char*>&temp_in);
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	
	//Read a value from fifo.
	//NOTICE: blocking operation, so it may take more than one cycle!
	socket->b_transport(*trans, delay);
	in_value = temp_in
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

virtual void process2::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64 adr = trans.get_address() / 4;
    unsigned char* ptr = trans.get_data_ptr();
    unsigned int len = trans.get_data_length;
    unsigned char* byt = trans.get_byte_enable_ptr();
    unsigned int width = trans.get_streaming_width();

    if ( cmd == tlm::TLM_READ_COMMAND )
	memcpy(ptr, &values[adr], len);
    else if ( cmd == tlm::TLM_WRITE_COMMAND )
	memcpy(&values[adr], ptr, len);

    trans.set_response_status( tlm::TLM_OK_RESPONSE );
}
