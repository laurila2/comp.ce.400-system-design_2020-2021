#include "process.hh"


void process4::decrypt ()
{
	sc_uint<32> decrypted_value; 
	
	while( true )
	{
		decrypted_value = fifo.read();
		wait( P3_P4_DELAY, SC_NS );

		decrypted_value = decrypted_value ^ KEY;
		decrypted_value = ( ( decrypted_value.range( 15, 0 ) << 16 ) +
		  decrypted_value.range( 31, 16 ) );
	
		wait(P4_LATENCY, SC_NS);

		out_value = decrypted_value;
		output_valid.notify();
	}
}
