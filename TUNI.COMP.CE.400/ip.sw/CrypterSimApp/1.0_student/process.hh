#include "header.hh"
#include "systemc.h"
#include <tlm.h>
#inlcude <tlm_utils/simple_initiator_socket.h>
#inlcude <tlm_utils/simple_target_socket.h>

SC_MODULE (process1)
{
    sc_in_clk	 clock; //Clock input
    sc_in<bool>   reset; //Reset, active high
    sc_in<sc_uint<32> > in_value; //Value that is to be crypted in the system.
	
    void encrypt ();

    tlm_utils::simple_initiator_socket<process1> socket;

    SC_CTOR(process1) : socket("socket")
    {
	//Initialize process as a thread, with reset active high
	SC_CTHREAD(encrypt, clock.pos());
	reset_signal_is(reset, false);
    }
}; 

SC_MODULE (process2)
{
    sc_in_clk	 clock; //Clock input
    sc_in<bool>   reset; //Reset, active high
    value1* memory; //The shared memory

    void write_value ();

    tlm_utils::simple_target_socket<process2> socket;
    virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay);

    SC_CTOR(process2) : socket("socket")
    {
	//Initialize process as a thread, with reset active high
	SC_CTHREAD(write_value, clock.pos());
	reset_signal_is(reset, false);

	socket.register_b_tranport(this, &process2::b_transport);
    }
};

SC_MODULE (process3)
{
    sc_in_clk	 clock; //Clock input
    sc_in<bool>   reset; //Reset, active high
    sc_fifo_out<sc_uint<32> > fifo; //Fifo where from the values are obtained.
    value1* memory; //The shared memory

    void read_value ();

    SC_CTOR(process3)
    {
	//Initialize process as a thread, with reset active high
	SC_CTHREAD(read_value, clock.pos());
	reset_signal_is(reset, false);
    }
};

SC_MODULE (process4)
{
    sc_in_clk	 clock; //Clock input
    sc_in<bool>   reset; //Reset, active high
    sc_fifo_in<sc_uint<32> > fifo; //Fifo where from the values are obtained.
    sc_out<sc_uint<32> > out_value;

    void decrypt ();

    SC_CTOR(process4)
    {
	//Initialize process as a thread, with reset active high
	SC_CTHREAD(decrypt, clock.pos());
	reset_signal_is(reset, false);
    }
};
