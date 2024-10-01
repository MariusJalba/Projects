module dff(input d,input clk,input rst_b,output reg q);
   always@(posedge clk) begin
         if(!rst_b)
              q<=0;
         else
              q<=d;
   end
endmodule

module shift_reg(
    input wire clk,
    input wire rst_b,
    input wire dir,
    input reg [15:0] serial_in,
    output reg [15:0] out
);
reg [15:0] d_inputs;
reg [15:0] d_outputs;
genvar i;
generate
   for(i=0;i<16;i=i+1) begin : dff_generator
        dff dff_inst(.d(d_inputs[i]),.clk(clk),.rst_b(rst_b),.q(d_outputs[i]));
   end
endgenerate

dff dff_inst0(.d(d_inputs[0]),.clk(clk),.rst_b(rst_b),.q(d_outputs[0]));
dff dff_inst1(.d(d_inputs[1]),.clk(clk),.rst_b(rst_b),.q(d_outputs[1]));
dff dff_inst2(.d(d_inputs[2]),.clk(clk),.rst_b(rst_b),.q(d_outputs[2]));
dff dff_inst3(.d(d_inputs[3]),.clk(clk),.rst_b(rst_b),.q(d_outputs[3]));
dff dff_inst4(.d(d_inputs[4]),.clk(clk),.rst_b(rst_b),.q(d_outputs[4]));
dff dff_inst5(.d(d_inputs[5]),.clk(clk),.rst_b(rst_b),.q(d_outputs[5]));
dff dff_inst6(.d(d_inputs[6]),.clk(clk),.rst_b(rst_b),.q(d_outputs[6]));
dff dff_inst7(.d(d_inputs[7]),.clk(clk),.rst_b(rst_b),.q(d_outputs[7]));
dff dff_inst8(.d(d_inputs[8]),.clk(clk),.rst_b(rst_b),.q(d_outputs[8]));
dff dff_inst9(.d(d_inputs[9]),.clk(clk),.rst_b(rst_b),.q(d_outputs[9]));
dff dff_inst10(.d(d_inputs[10]),.clk(clk),.rst_b(rst_b),.q(d_outputs[10]));
dff dff_inst11(.d(d_inputs[11]),.clk(clk),.rst_b(rst_b),.q(d_outputs[11]));
dff dff_inst12(.d(d_inputs[12]),.clk(clk),.rst_b(rst_b),.q(d_outputs[12]));
dff dff_inst13(.d(d_inputs[13]),.clk(clk),.rst_b(rst_b),.q(d_outputs[13]));
dff dff_inst14(.d(d_inputs[14]),.clk(clk),.rst_b(rst_b),.q(d_outputs[14]));
dff dff_inst15(.d(d_inputs[15]),.clk(clk),.rst_b(rst_b),.q(d_outputs[15]));



 
always @* begin
    d_inputs = serial_in;
if(dir == 1) begin//right-shift
    d_inputs[0]=d_inputs[1];
    d_inputs[1]=d_inputs[2];
    d_inputs[2]=d_inputs[3];
    d_inputs[3]=d_inputs[4];
    d_inputs[4]=d_inputs[5];
    d_inputs[5]=d_inputs[6];
    d_inputs[6]=d_inputs[7];
    d_inputs[7]=d_inputs[8];
    d_inputs[8]=d_inputs[9];
    d_inputs[9]=d_inputs[10];
    d_inputs[10]=d_inputs[11];
    d_inputs[11]=d_inputs[12];
    d_inputs[12]=d_inputs[13];
    d_inputs[13]=d_inputs[14];
    d_inputs[14]=d_inputs[15];
    d_inputs[15]=1'b0;
end else begin//left-shift
    d_inputs[15]=d_inputs[14];
    d_inputs[14]=d_inputs[13];
    d_inputs[13]=d_inputs[12];
    d_inputs[12]=d_inputs[11];
    d_inputs[11]=d_inputs[10];
    d_inputs[10]=d_inputs[9];
    d_inputs[9]=d_inputs[8];
    d_inputs[8]=d_inputs[7];
    d_inputs[7]=d_inputs[6];
    d_inputs[6]=d_inputs[5];
    d_inputs[5]=d_inputs[4];
    d_inputs[4]=d_inputs[3];
    d_inputs[3]=d_inputs[2];
    d_inputs[2]=d_inputs[1];
    d_inputs[1]=d_inputs[0];
    d_inputs[0]=1'b0;
end
out = d_outputs;
end

 
endmodule
   




module tb_shiftreg;
localparam CLK_PERIOD = 10;

reg clk=0;
reg rst_b=1;
reg dir=0;
reg [15:0] serial_in = 16'b1011011101101011;
wire [15:0] out;

shift_reg u0(.clk(clk),.rst_b(rst_b),.dir(dir),.serial_in(serial_in),.out(out));
always #((CLK_PERIOD/2)) clk = ~clk;



initial begin
      #10; // Wait for a shift
      $display("%d\t%b\t%b", $time, serial_in, out);
end

endmodule




