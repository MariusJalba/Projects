module NRD(
    input [15:0] dividend,  // Dividend
    input [15:0] divisor,   // Divisor
    output reg [15:0] r,     // Remainder
    output reg [15:0] q      // Quotient
);

integer i;
reg [15:0] A, M, Q;

always @* begin
    A = 16'b0;  
    M = divisor;
    Q = dividend;

    for (i = 0; i < 16; i = i + 1) begin
        {A, Q} = {A, Q} << 1;
        A = A - M;
        if (A[15] == 1'b1) begin 
            Q[0] = 1'b0;
            A = A + M;  
        end else begin  
            Q[0] = 1'b1; 
        end
    end
    r = A;
    q = Q;
end

endmodule

module FAC(
    input [15:0] x,
    input [15:0] y,
    input ci,
    input sub,
    output reg [31:0] z,
    output reg [31:0] co
);
genvar i;
reg [15:0] y_reg;
initial begin
    z = 31'b0;
    co = 31'b0;
end
  always @(*) begin
    if(sub==1'b1) begin
      y_reg = ~y;
      y_reg = y_reg + 16'b1;
    end
    else
      y_reg = y;
  end
  generate
    for(i=0; i<16; i=i+1) begin
      always @(*) begin
        z[i] = x[i]^y_reg[i]^(i==0? 1'b0 : co[i-1]);
        co[i] = x[i]*y_reg[i]+x[i]*(i==0? 1'b0 : co[i-1])+y_reg[i]*(i==0? 1'b0 : co[i-1]);
      end
    end
  endgenerate
endmodule

module xor16(input [15:0] a, input [15:0] b, output [15:0] y);
    assign y = a ^ b;
endmodule

module and16(input [15:0] a, input [15:0] b, output [15:0] y);
    assign y = a & b;
endmodule

module or16(input [15:0] a, input [15:0] b, output wire [15:0] y);
    assign y = a | b;
endmodule

module inv(input [15:0] a, output wire [15:0] y);
    assign y = ~a;
endmodule

module dff(input d,input clk,input rst_b,output reg q);
   always@(posedge clk) begin
         if(!rst_b)
              q<=0;
         else
              q<=d;
   end
endmodule

module shift_reg(input wire clk,input wire rst_b,input wire dir,input wire [15:0] serial_in,output reg [15:0] out);
reg [15:0] d_inputs;
wire [15:0] d_outputs;
genvar i;
/*generate
   for(i=0;i<16;i=i+1) begin : dff_generator
        dff dff_inst(.d(d_inputs[i]),.clk(clk),.rst_b(rst_b),.q(d_outputs[i]));
   end
endgenerate*/

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
module booth(input signed [15:0] multiplicand,input signed [15:0] multiplier,output reg [31:0] out);
integer i;
reg [15:0] A,Q,M;
reg [0:0] prev;

always @* begin
   A=15'b0;
   Q=multiplicand;
   M=multiplier;
   prev=1'b0;
   

   for(i=0;i<16;i=i+1) begin
        $display("A = %b   ,Q = %b   , prev = %b",A,Q,prev);
        if(prev == 0 && Q[0] == 1'b1)
             A=A-M;
        if(prev == 1 && Q[0] == 1'b0)
             A=A+M;
      
        {A,Q,prev} = {A,Q,prev}>>1;
        A[15] = A[14];
   end
   out={A,Q};
end

endmodule

module ALU(input wire clk,input wire rst_b,input wire[15:0] A,input wire[15:0] B,input wire[3:0] op,output reg [31:0] rezultat);
reg [15:0] A_input_booth;
reg [15:0] B_input_booth;
wire [31:0] rezultat_output_booth;
reg [15:0] A_input_shiftreg;
wire [15:0] rezultat_output_shiftreg;
reg dir=1;
reg dir2=0;
reg [15:0] A_input_shiftreg2;
wire [15:0] rezultat_output_shiftreg2;
reg [15:0] A_input_inv;
wire [15:0] rezultat_inv;
reg [15:0] A_input_or;
reg [15:0] B_input_or;
wire [15:0] rezultat_or;
reg [15:0] A_input_and;
reg [15:0] B_input_and;
wire [15:0] rezultat_and;
reg [15:0] A_input_xor;
reg [15:0] B_input_xor;
wire [15:0] rezultat_xor;
reg [15:0] A_input_add;
reg [15:0] B_input_add;
reg ci=0;
reg sub=0;
wire [31:0] rezultat_add;
reg [15:0] A_input_sub;
reg [15:0] B_input_sub;
reg sub2=1;
wire [31:0] rezultat_sub;
reg [15:0] A_input_NRD;
reg [15:0] B_input_NRD;
wire [15:0] remainder_NRD;
wire [15:0] quotient_NRD;




booth b(.multiplicand(A_input_booth),.multiplier(B_input_booth),.out(rezultat_output_booth));
shift_reg rightshift(.clk(clk),.rst_b(rst_b),.dir(dir),.serial_in(A_input_shiftreg),.out(rezultat_output_shiftreg));
shift_reg leftshift(.clk(clk),.rst_b(rst_b),.dir(dir2),.serial_in(A_input_shiftreg2),.out(rezultat_output_shiftreg2));
inv i(.a(A_input_inv),.y(rezultat_inv));
or16 o(.a(A_input_or),.b(B_input_or),.y(rezultat_or));
and16 a(.a(A_input_and),.b(B_input_and),.y(rezultat_and));
xor16 x(.a(A_input_xor),.b(B_input_xor),.y(rezultat_xor));
FAC f(.x(A_input_add),.y(B_input_add),.ci(ci),.sub(sub),.z(rezultat_add));
FAC f2(.x(A_input_sub),.y(B_input_sub),.ci(ci),.sub(sub2),.z(rezultat_sub));
NRD n(.dividend(A_input_NRD),.divisor(B_input_NRD),.r(remainder_NRD),.q(quotient_NRD));


always @* begin
  if(op == 4'b0110) begin
    A_input_shiftreg = A;
    rezultat={16'b0000000000000000,rezultat_output_shiftreg};
  end
  if(op == 4'b1000) begin
    A_input_booth=A;
    B_input_booth=B;
    rezultat=rezultat_output_booth;
  end
  if(op == 4'b0111) begin
     A_input_shiftreg2 = A;
     rezultat={16'b0000000000000000,rezultat_output_shiftreg2};
   end
  if(op == 4'b0000) begin
     A_input_inv = A;
     rezultat={16'b0000000000000000,rezultat_inv};
  end
   if(op == 4'b0001) begin
     A_input_or = A;
     B_input_or = B;
     rezultat={16'b0000000000000000,rezultat_or};
   end
    if(op == 4'b0010) begin
     A_input_and = A;
     B_input_and = B;
     rezultat={16'b0000000000000000,rezultat_and};
   end
     if(op == 4'b0011) begin
      A_input_xor = A;
      B_input_xor = B;
      rezultat={16'b0000000000000000,rezultat_xor};
   end
     if(op == 4'b0100) begin
      A_input_add = A;
      B_input_add = B;
      rezultat=rezultat_add;
   end
     if(op == 4'b0101) begin
      A_input_sub = A;
      B_input_sub = B;
      rezultat = rezultat_sub;
   end  
     if(op == 4'b1001) begin
      A_input_NRD = A;
      B_input_NRD = B;
      rezultat={remainder_NRD,quotient_NRD};
    end   
  
end


endmodule

module ALU_tb;
reg [15:0] A;
reg [15:0] B;
reg [3:0] op;
wire [31:0] rezultat;
localparam CLK_PERIOD = 10;

reg clk=0;
reg rst_b=1;
ALU a(.clk(clk),.rst_b(rst_b),.A(A),.B(B),.op(op),.rezultat(rezultat));
always #((CLK_PERIOD/2)) clk = ~clk;




initial begin
A=16'b0000000000001001;
B=16'b0000000000000011;
op=4'b1001;
#200
$display("rezultat=%b",rezultat);
end
endmodule
