`default_nettype none
`include "global_project_defines.sv"

module doublesync_no_reset (indata,
				  outdata,
				  clk);

parameter synchronizer_depth = 2;
parameter CUT_TIMING_TO_INPUT = 0;
parameter num_of_add_extra_duplicatable_registers_at_end = 0;
parameter MAXFANOUT_IF_DUPLICATABLE = 1000;
input wire indata;
input wire clk;
output wire outdata;
wire outdata_raw;

    `ifdef SOFTWARE_IS_QUARTUS
	   generate
	                         `ifdef CUT_ALL_TIMING_PATHS_TO_DOUBLESYNCS
                             if (1)
                             `else							 
	                         if (CUT_TIMING_TO_INPUT)
							 `endif
							 begin							 
									 altera_std_synchronizer 
									 the_altera_std_synchronizer
									 (
									  .clk (clk),
									  .din (indata),
									  .dout (outdata_raw),
									  .reset_n (1'b1)
									);
									defparam the_altera_std_synchronizer.depth = synchronizer_depth;

							 end else
							 begin
									 
									 my_altera_std_synchronizer_nocut 
									 the_altera_std_synchronizer
									 (
									  .clk (clk),
									  .din (indata),
									  .dout (outdata_raw),
									  .reset_n (1'b1)
									);
									defparam the_altera_std_synchronizer.depth = synchronizer_depth;
						    end
		endgenerate
					
	`else
						//special coding for SRL16 inference in Xilinx devices
						(* ASYNC_REG = "TRUE" *) reg [synchronizer_depth-1:0] sync_srl16_inferred;
						   always @(posedge clk)
								 sync_srl16_inferred[synchronizer_depth-1:0] <= {sync_srl16_inferred[synchronizer_depth-2:0], indata};
									
						assign outdata_raw = sync_srl16_inferred[synchronizer_depth-1];
										
	`endif

			generate 
					 if (num_of_add_extra_duplicatable_registers_at_end > 0)
					 begin
  	 					  if (num_of_add_extra_duplicatable_registers_at_end > 1)
						  begin
							(* max_fanout = 1, maxfan = 1, altera_attribute = {"-name SYNCHRONIZER_IDENTIFICATION OFF"} *)  reg [num_of_add_extra_duplicatable_registers_at_end-2:0] out_duplicatable_reg;
							(* max_fanout = MAXFANOUT_IF_DUPLICATABLE, maxfan = MAXFANOUT_IF_DUPLICATABLE, altera_attribute = {"-name SYNCHRONIZER_IDENTIFICATION OFF"} *)  reg last_duplicatable_reg;

								always @(posedge clk)
								begin
									 out_duplicatable_reg[num_of_add_extra_duplicatable_registers_at_end-2:0] <= 
									 {out_duplicatable_reg[num_of_add_extra_duplicatable_registers_at_end-2:0], outdata_raw};
								end
								always @(posedge clk)
								begin
								      last_duplicatable_reg <= out_duplicatable_reg[num_of_add_extra_duplicatable_registers_at_end-2];
								end
								assign outdata = last_duplicatable_reg;		 
						end else
						begin //num_of_add_extra_duplicatable_registers_at_end == 1
						      (* max_fanout = MAXFANOUT_IF_DUPLICATABLE, maxfan = MAXFANOUT_IF_DUPLICATABLE, altera_attribute = {"-name SYNCHRONIZER_IDENTIFICATION OFF"} *)  reg last_duplicatable_reg;
                                always @(posedge clk)
								begin
								      last_duplicatable_reg <= outdata_raw;
								end
								assign outdata = last_duplicatable_reg;	
						end
					 end else
					 begin 
						   assign outdata = outdata_raw;
					 end
			endgenerate
endmodule
`default_nettype wire