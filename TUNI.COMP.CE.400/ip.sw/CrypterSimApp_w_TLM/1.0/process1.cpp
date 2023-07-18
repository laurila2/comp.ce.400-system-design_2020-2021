#include "process.hh"

void process1::encrypt ()
{
    sc_uint<32> encrypted_value; //The crypted value, is to be fed to fifo.
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(0, SC_NS);
    int index = 0;
	
    while( true )
    {
	//Inform that we're ready for next input value
	//WAIT IS MANDATORY. Otherwise the feeder of input cannot proceed.
	input_ready.notify();
	wait();
		
	//Read input value...
	encrypted_value = in_value;
		
	//Switch places between first and last 16 bits.
	encrypted_value = ( (encrypted_value.range( 15, 0 ) << 16 ) +
			    encrypted_value.range( 31, 16 ) );
	//Encrypt the value with the key.
	encrypted_value = encrypted_value ^ KEY;
		
	//How long the processing takes
	wait( P1_LATENCY, SC_NS );
		
	//...and put it into fifo. NOTICE: blocking operation,
	//so it may take more than one cycle!
	//fifo.write(encrypted_value);

	uint temp_enc = encrypted_value.to_uint();
	
        trans->set_command(tlm::TLM_WRITE_COMMAND);
	trans->set_address(index++);
	trans->set_data_length(4);
	trans->set_streaming_width(4);
	trans->set_byte_enable_ptr(0);
	trans->set_dmi_allowed(false);
	trans->set_data_ptr(reinterpret_cast<unsigned char*>(&temp_enc));
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	
	socket->b_transport(*trans, delay);
		
	//How long the fifo communication takes
	wait( P1_P2_DELAY, SC_NS );
    }
}
