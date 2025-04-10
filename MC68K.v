`default_nettype none
module MC68K(
input wire	RS232_RxData,
input wire	CLOCK_50,
input wire	Reset_L,
input wire	TraceRequest_L,
input wire	IRQ2_L,
input wire	IRQ4_L,
input wire	[9:0] SW,
output wire	RS232_TxData,
output wire	DRAM_CLK,
output wire	DRAM_CKE,
output wire	DRAM_CS_N,
output wire	DRAM_CAS_N,
output wire	DRAM_RAS_N,
output wire	DRAM_WE_N,
output wire	DRAM_UDQM,
output wire	DRAM_LDQM,
output wire	CPUClock,
output wire	Dtack_L,
output wire	RomSelect_H,
output wire	RamSelect_H,
output wire	DramRamSelect_H,
output wire	IOSelect_H,
output wire	ResetOut,
output wire	DramDtack_L,
output wire	AS_L,
output wire	UDS_L,
output wire	RW,
output wire	[31:0] AddressBus,
output wire	[15:0] DataBusIn,
output wire	[15:0] DataBusOut,
output wire	[12:0] DRAM_ADDR,
output wire	[1:0] DRAM_BA,
inout wire	[15:0] DRAM_DQ,
output wire	[6:0] HEX0,
output wire	[6:0] HEX1,
output wire	[6:0] HEX2,
output wire	[6:0] HEX3,
output wire	[6:0] HEX4,
output wire	[6:0] HEX5,
output wire	[9:0] LEDR,


//////////// Audio //////////
input                       AUD_ADCDAT,
inout                       AUD_ADCLRCK,
inout                       AUD_BCLK,
output                      AUD_DACDAT,
inout                       AUD_DACLRCK,
output                      AUD_XCK,

//////////// I2C for Audio  //////////
output                      FPGA_I2C_SCLK,
inout                       FPGA_I2C_SDAT,

///////// ADC /////////
output              ADC_CONVST,
output             ADC_DIN,
input              ADC_DOUT,
output             ADC_SCLK,

///////// VGA /////////
output      [7:0]  VGA_B,
output             VGA_BLANK_N,
output             VGA_CLK,
output      [7:0]  VGA_G,
output             VGA_HS,
output      [7:0]  VGA_R,
output             VGA_SYNC_N,
output             VGA_VS


);


parameter INPUT_X_NUM_ADDRESS_BITS = 10;
parameter INPUT_Y_NUM_ADDRESS_BITS = 9;
parameter OUTPUT_X_NUM_ADDRESS_BITS = 8;
parameter OUTPUT_Y_NUM_ADDRESS_BITS = 5;

wire	[31:0] Address;
wire	Clock25Mhz;
wire	Clock30Mhz;
wire	Clock50Mhz;
wire	Clock50Mhz_Inverted;
wire	[15:0] DataBusIn_Composite;
wire	[15:0] DataBusOut_Composite;
wire	[31:0] DMA_Address;
wire	DMA_AS_L;
wire	[15:0] DMA_DataOut;
wire	DMA_LDS_L;
wire	DMA_RW;
wire	DMA_UDS_L;
wire	[7:0] InPortA;
wire	[7:0] InPortB;
wire	[7:0] OutPortB;
wire	TraceException_H;
wire	Timer1_IRQ;
wire	Timer3_IRQ;
wire	Timer2_IRQ;
wire	Timer4_IRQ;
wire	Timer7_IRQ;
wire	Timer6_IRQ;
wire	Timer8_IRQ;
wire	Timer5_IRQ;
wire	CPU_AS_L;
wire	CPU_UDS_L;
wire	CPU_LDS_L;
wire	CPU_RW;
wire	[31:0] CPU_Address;
wire	[15:0] CPU_DataBusOut;
wire	BR_L;
wire	[2:0] IPL;
wire	CanBusSelect_H;
wire	CanBusDtack_L;
wire	DataBusIn_OE;
wire	Trdata_OE;
wire	TraceIRQ_L;
wire	ACIA_IRQ;
wire	OffBoardMemory_H;
wire	RESET_H;
wire	BG_L;
wire	LDS_L;

(* keep = 1 *)  logic [12:0] onchip_memory_m68k_connection_address       ;                 
(* keep = 1 *)  logic        onchip_memory_m68k_connection_chipselect    ;
(* keep = 1 *)  logic        onchip_memory_m68k_connection_clken         ;
(* keep = 1 *)  logic        onchip_memory_m68k_connection_write         ;
(* keep = 1 *)  logic [7:0] onchip_memory_m68k_connection_readdata      ;
(* keep = 1 *)  logic [7:0] onchip_memory_m68k_connection_writedata     ;
//(* keep = 1 *)  logic [1:0]  onchip_memory_m68k_connection_byteenable    ;

(* keep = 1 *)  logic [12:0] shadow_buffer_address       ;                 
(* keep = 1 *)  logic        shadow_buffer_chipselect    ;
(* keep = 1 *)  logic        shadow_buffer_clken         ;
(* keep = 1 *)  logic        shadow_buffer_write         ;
(* keep = 1 *)  logic [7:0] shadow_buffer_readdata      ;
(* keep = 1 *)  logic [7:0] shadow_buffer_writedata     ;
//(* keep = 1 *)  logic [1:0]  shadow_buffer_byteenable    ;

(* keep = 1 *)  logic [12:0] onchip_memory_pixel_buffer_connection_address       ;                
(* keep = 1 *)  logic        onchip_memory_pixel_buffer_connection_clken         ;                  
(* keep = 1 *)  logic        onchip_memory_pixel_buffer_connection_chipselect    ;             
(* keep = 1 *)  logic        onchip_memory_pixel_buffer_connection_write         ;                  
(* keep = 1 *)  logic [7:0]  raw_onchip_memory_pixel_buffer_connection_readdata      ;               
(* keep = 1 *)  logic [7:0]  onchip_memory_pixel_buffer_connection_readdata      ;               
(* keep = 1 *)  logic [7:0]  onchip_memory_pixel_buffer_connection_writedata     ;              
                                                  
(* keep = 1 *) logic        video_pixel_buffer_dma_0_avalon_pixel_dma_master_readdatavalid;
(* keep = 1 *) logic        video_pixel_buffer_dma_0_avalon_pixel_dma_master_waitrequest  ;
(* keep = 1 *) logic [31:0] video_pixel_buffer_dma_0_avalon_pixel_dma_master_address      ;   
(* keep = 1 *) logic        video_pixel_buffer_dma_0_avalon_pixel_dma_master_lock         ;       
(* keep = 1 *) logic        video_pixel_buffer_dma_0_avalon_pixel_dma_master_read         ;       
(* keep = 1 *) logic [31:0] video_pixel_buffer_dma_0_avalon_pixel_dma_master_readdata     ;


logic vga_clk;

assign vga_clk = Clock25Mhz;

assign VGA_BLANK_N = 1'b1;
assign VGA_SYNC_N = 1'b1;

OnChipM68xxIO	b2v_inst11(
	.Clock_50Mhz(Clock50Mhz),
	.IOSelect(IOSelect_H),
	.UDS_L(UDS_L),
	.AS_L(AS_L),
	.WE_L(RW),
	.Clk(Clock25Mhz),
	.Reset_L(Reset_L),
	.RS232_RxData(RS232_RxData),
	.Address(Address),
	.DataIn(DataBusOut_Composite[15:8]),
	.ACIA_IRQ(ACIA_IRQ),
	.RS232_TxData(RS232_TxData),
	.DataOut(DataBusIn_Composite[15:8]));


assign	RESET_H =  ~Reset_L;


CPU_DMA_Mux	b2v_inst14(
	.CPU_DMA_Select(1'b1),
	.DMA_AS_L(DMA_AS_L),
	.DMA_RW(DMA_RW),
	.DMA_UDS_L(DMA_UDS_L),
	.DMA_LDS_L(DMA_LDS_L),
	.CPU_AS_L(CPU_AS_L),
	.CPU_UDS_L(CPU_UDS_L),
	.CPU_LDS_L(CPU_LDS_L),
	.CPU_RW(CPU_RW),
	.CPU_Address(CPU_Address),
	.CPU_DataBusOut(CPU_DataBusOut),
	.DMA_Address(DMA_Address),
	.DMA_DataBusOut(DMA_DataOut),
	.AS_L(AS_L),
	.UDS_L(UDS_L),
	.LDS_L(LDS_L),
	.RW(RW),
	.AddressOut(Address),
	.DataOut(DataBusOut_Composite)
);



OnChipROM16KWords	b2v_inst16(
	.RomSelect_H(RomSelect_H),
	.Clock(Clock25Mhz),
	.Address(Address[14:1]),
	.DataOut(DataBusIn_Composite));


M68000CPU	b2v_inst17(
	.Clk(Clock25Mhz),
	.Reset_L(Reset_L),
	.Dtack_L(Dtack_L),
	.BR_L(1'b1),
	.BGACK_L(1'b1),
	.DataBusIn(DataBusIn_Composite),
	.IPL(IPL),
	.AS_L(CPU_AS_L),
	.UDS_L(CPU_UDS_L),
	.LDS_L(CPU_LDS_L),
	.RW(CPU_RW),
	.BG_L(BG_L),
	.Address(CPU_Address),
	.DataBusOut(CPU_DataBusOut));

Dram	b2v_inst2(
	.WE_L(RW),
	.Clock50Mhz_Inverted(Clock50Mhz_Inverted),
	.Reset_L(Reset_L),
	.DramSelect_H(DramRamSelect_H),
	.LDS_L(LDS_L),
	.UDS_L(UDS_L),
	.AS_L(AS_L),
	.Address(Address),
	.DataIn(DataBusOut_Composite),
	.sdram_dq(DRAM_DQ),
	.sdram_cke(DRAM_CKE),
	.sdram_cs_n(DRAM_CS_N),
	.sdram_ras_n(DRAM_RAS_N),
	.sdram_cas_n(DRAM_CAS_N),
	.sdram_we_n(DRAM_WE_N),
	.DramDtack_L(DramDtack_L),
	.ResetOut_L(ResetOut),
	.DataOut(DataBusIn_Composite),
	
	.sdram_a(DRAM_ADDR),
	.sdram_ba(DRAM_BA),
	
	.sdram_dqm({DRAM_UDQM,DRAM_LDQM})
	);
               

AddressDecoder_Verilog	b2v_inst20(
	.Address(Address),
	.OnChipRomSelect_H(RomSelect_H),
	.OnChipRamSelect_H(RamSelect_H),
	.DramSelect_H(DramRamSelect_H),
	.IOSelect_H(IOSelect_H),
	.DMASelect_L(),
	.OffBoardMemory_H(OffBoardMemory_H),
	.CanBusSelect_H(CanBusSelect_H)
);


Dtack_Generator_Verilog	b2v_inst21(
	.AS_L(AS_L),
	.DramSelect_H(DramRamSelect_H),
	.DramDtack_L(DramDtack_L),
	.CanBusSelect_H(CanBusSelect_H),
	.CanBusDtack_L(CanBusDtack_L),
	.DtackOut_L(Dtack_L));


	wire [15:0] tr_data;
	assign tr_data = Trdata_OE ? DataBusOut_Composite : 16'bzzzz_zzzz_zzzz_zzzz;	
   assign DataBusIn_Composite = DataBusIn_OE ? tr_data : 16'bzzzz_zzzz_zzzz_zzzz;
	

InterruptPriorityEncoder	b2v_inst28(
	.IRQ7_L(1),
	.IRQ6_L(1),
	.IRQ5_L(1),
	.IRQ4_L(1),
	.IRQ3_L(1),
	.IRQ2_L(1),
	.IRQ1_L(1),
	.IPL(IPL));


TraceExceptionGenerator	b2v_inst30(
	.Clock(Clock25Mhz),
	.Reset(Reset_L),
	.AS_L(AS_L),
	.RW_L(RW),
	.SingleStep_H(TraceException_H),
	.TraceRequest_L(TraceRequest_L),
	.Address(Address),
	.TraceIRQ_L(TraceIRQ_L));

assign	Trdata_OE = RW & OffBoardMemory_H;

assign	DataBusIn_OE = ~(RW | AS_L);

OnChipRam256kbyte	b2v_inst6(
	.RamSelect_H(RamSelect_H),
	.UDS_L(UDS_L),
	.LDS_L(LDS_L),
	.WE_L(RW),
	.AS_L(AS_L),
	.Clock(Clock25Mhz),
	.Address(Address[17:1]),
	.DataIn(DataBusOut_Composite),
	.DataOut({DataBusIn_Composite[15:8],DataBusIn_Composite[7:0]}));


ClockGen	b2v_inst7(
	.refclk(CLOCK_50),
	.rst(RESET_H),
	.outclk_0(Clock25Mhz),
	.outclk_1(Clock30Mhz),
	.outclk_2(Clock50Mhz),
	.outclk_3(Clock50Mhz_Inverted)
	);


OnChipIO	b2v_inst8(
	.IOSelect(IOSelect_H),
	.WE_L(RW),
	.UDS_L(UDS_L),
	.AS_L(AS_L),
	.Clk(Clock25Mhz),
	.Reset_L(Reset_L),
	.Address(Address),
	.DataIn(DataBusOut_Composite[15:8]),
	.InPortA(InPortA),
	.InPortB(),
	.InPortC(),
	.InPortD(),
	.TraceExceptionBit_H(TraceException_H),
	.LCD_RS(),
	.LCD_E(),
	.LCD_RW(),
	.Timer1_IRQ(Timer1_IRQ),
	.Timer2_IRQ(Timer2_IRQ),
	.Timer3_IRQ(Timer3_IRQ),
	.Timer4_IRQ(Timer4_IRQ),
	.Timer5_IRQ(Timer5_IRQ),
	.Timer6_IRQ(Timer6_IRQ),
	.Timer7_IRQ(Timer7_IRQ),
	.Timer8_IRQ(Timer8_IRQ),
	.DataOut(DataBusIn_Composite[15:8]),
	.HexDisplay0(/*HEX0*/),
	.HexDisplay1(/*HEX1*/),
	.HexDisplay2(/*HEX2*/),
	.HexDisplay3(/*HEX3*/),
	.HexDisplay4(/*HEX4*/),
	.HexDisplay5(/*HEX5*/),	
	.LCD_DataOut(/*LCD_Data*/),
	.OutPortA(),
	.OutPortB(),
	.OutPortC()	
	);



assign	DRAM_CLK = Clock50Mhz;
assign	CPUClock = Clock25Mhz;
assign	AddressBus = Address;
assign	DataBusIn = DataBusIn_Composite;
assign	DataBusOut = DataBusOut_Composite;
assign	InPortA[7:0] = SW[7:0];
assign	InPortB[1] = SW[9];


I2C_AV_Config 
I2C_Configure_Audio_Chip(	
//	Host Side
  .iCLK(Clock50Mhz),
  .iRST_N(1'b1),
//	I2C Side
  .I2C_SDAT(FPGA_I2C_SDAT), // I2C Data
  .I2C_SCLK(FPGA_I2C_SCLK) // I2C Clock
);



SevenSegmentDisplayDecoder SevenSegmentDisplayDecoder_inst0(.ssOut(HEX0), .nIn(4'h1));
SevenSegmentDisplayDecoder SevenSegmentDisplayDecoder_inst1(.ssOut(HEX1), .nIn(4'h2));
SevenSegmentDisplayDecoder SevenSegmentDisplayDecoder_inst2(.ssOut(HEX2), .nIn(4'h3));
SevenSegmentDisplayDecoder SevenSegmentDisplayDecoder_inst3(.ssOut(HEX3), .nIn(4'hC));
SevenSegmentDisplayDecoder SevenSegmentDisplayDecoder_inst4(.ssOut(HEX4), .nIn(4'hB));
SevenSegmentDisplayDecoder SevenSegmentDisplayDecoder_inst5(.ssOut(HEX5), .nIn(4'hA));

vga_system 
vga_system_inst (
        .clk_clk                                                        (Clock25Mhz),                                                        //                                              clk.clk
        .reset_reset_n                                                  (Reset_L),                                                  //                                            reset.reset_n
        .vga_out_CLK                                                    (VGA_CLK),                                                    //                                          vga_out.CLK
        .vga_out_HS                                                     (VGA_HS),                                                     //                                                 .HS
        .vga_out_VS                                                     (VGA_VS),                                                     //                                                 .VS
        .vga_out_BLANK                                                  (),                                                  //                                                 .BLANK
        .vga_out_SYNC                                                   (),                                                   //                                                 .SYNC
        .vga_out_R                                                      (VGA_R),                                                      //                                                 .R
        .vga_out_G                                                      (VGA_G),                                                      //                                                 .G
        .vga_out_B                                                      (VGA_B),                                                      //                                                 .B
        .video_pixel_buffer_dma_0_avalon_pixel_dma_master_readdatavalid ,
        .video_pixel_buffer_dma_0_avalon_pixel_dma_master_waitrequest   ,
        .video_pixel_buffer_dma_0_avalon_pixel_dma_master_address       ,
        .video_pixel_buffer_dma_0_avalon_pixel_dma_master_lock          ,
        .video_pixel_buffer_dma_0_avalon_pixel_dma_master_read          ,
        .video_pixel_buffer_dma_0_avalon_pixel_dma_master_readdata    
);


buffer_system  
buffer_system_inst (
        .clk_clk                                                        (Clock25Mhz),                                                        //                                              clk.clk
        .reset_reset_n                                                  (Reset_L),                                                  //                                            reset.reset_n
        .onchip_memory_pixel_buffer_connection_address      (onchip_memory_pixel_buffer_connection_address     )                  ,              //            onchip_memory_pixel_buffer_connection.address
        .onchip_memory_pixel_buffer_connection_clken        (onchip_memory_pixel_buffer_connection_clken       )                  ,              //                                                 .clken
        .onchip_memory_pixel_buffer_connection_chipselect   (onchip_memory_pixel_buffer_connection_chipselect  )                  ,              //                                                 .chipselect
        .onchip_memory_pixel_buffer_connection_write        (onchip_memory_pixel_buffer_connection_write       )                  ,              //                                                 .write
        .onchip_memory_pixel_buffer_connection_readdata     (onchip_memory_pixel_buffer_connection_readdata    )                  ,              //                                                 .readdata
        .onchip_memory_pixel_buffer_connection_writedata    (onchip_memory_pixel_buffer_connection_writedata   )                  ,              //                                                 .writedata
        .onchip_memory_m68k_connection_address              (onchip_memory_m68k_connection_address             )                  ,              //                    onchip_memory_m68k_connection.address
        .onchip_memory_m68k_connection_chipselect           (onchip_memory_m68k_connection_chipselect          )                  ,              //                                                 .chipselect
        .onchip_memory_m68k_connection_clken                (onchip_memory_m68k_connection_clken               )                  ,              //                                                 .clken
        .onchip_memory_m68k_connection_write                (onchip_memory_m68k_connection_write               )                  ,              //                                                 .write
        .onchip_memory_m68k_connection_readdata             (onchip_memory_m68k_connection_readdata            )                  ,              //                                                 .readdata
        .onchip_memory_m68k_connection_writedata            (onchip_memory_m68k_connection_writedata           )                  ,              //                                                 .writedata
        .shadow_buffer_address                              (shadow_buffer_address                             )                  ,                                       //                                    shadow_buffer.address
        .shadow_buffer_clken                                (shadow_buffer_clken                               )                  ,                                       //                                                 .clken
        .shadow_buffer_chipselect                           (shadow_buffer_chipselect                          )                  ,                                       //                                                 .chipselect
        .shadow_buffer_write                                (shadow_buffer_write                               )                  ,                                       //                                                 .write
        .shadow_buffer_readdata                             (shadow_buffer_readdata                            )                  ,                                       //                                                 .readdata
        .shadow_buffer_writedata                            (shadow_buffer_writedata                           )                                                         //                                                 .writedata      
    );

midway8080_memory_adapter
midway8080_memory_adapter_inst
(
.input_x_address(video_pixel_buffer_dma_0_avalon_pixel_dma_master_address[INPUT_X_NUM_ADDRESS_BITS+1:2]),
.input_y_address(video_pixel_buffer_dma_0_avalon_pixel_dma_master_address[INPUT_X_NUM_ADDRESS_BITS+INPUT_Y_NUM_ADDRESS_BITS+1:INPUT_X_NUM_ADDRESS_BITS+2]),
.output_x_address(onchip_memory_pixel_buffer_connection_address[OUTPUT_X_NUM_ADDRESS_BITS-1:0]),
.output_y_address(onchip_memory_pixel_buffer_connection_address[OUTPUT_Y_NUM_ADDRESS_BITS+OUTPUT_X_NUM_ADDRESS_BITS-1:OUTPUT_X_NUM_ADDRESS_BITS]),
.raw_data_from_midway8080_memory(raw_onchip_memory_pixel_buffer_connection_readdata),
.rgb_data_out(video_pixel_buffer_dma_0_avalon_pixel_dma_master_readdata)
);

assign raw_onchip_memory_pixel_buffer_connection_readdata             = onchip_memory_pixel_buffer_connection_readdata;
assign onchip_memory_pixel_buffer_connection_clken                    = 1'b1;              //                                  
assign onchip_memory_pixel_buffer_connection_chipselect               = video_pixel_buffer_dma_0_avalon_pixel_dma_master_read; 
assign onchip_memory_pixel_buffer_connection_write                    = 1'b0;              // 

always @(posedge Clock25Mhz)
begin
     video_pixel_buffer_dma_0_avalon_pixel_dma_master_readdatavalid <= video_pixel_buffer_dma_0_avalon_pixel_dma_master_read;	
end


assign video_pixel_buffer_dma_0_avalon_pixel_dma_master_waitrequest   = 1'b0;


endmodule
`default_nettype wire