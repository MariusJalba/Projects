module booth(
input signed [15:0] multiplicand,
input signed [15:0] multiplier,
output reg [31:0] out);
integer i;
reg [15:0] A,Q,M;
reg [0:0] prev;

always @* begin
   A=15'b0;
   Q=multiplicand;
   M=multiplier;
   prev=1'b0;
   

   for(i=0;i<16;i=i+1) begin
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

module booth_tb;
   reg [15:0] multiplicand;
   reg [15:0] multiplier;
   wire [31:0] out;
   booth b(.multiplicand(multiplicand),.multiplier(multiplier),.out(out));
   
   initial begin
       multiplicand=16'b1111111110010111;
       multiplier=16'b0000000000101110;
       #200
       $display("out=%b",out);
   end
endmodule
//asta e

