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


module FAC_tb;

  reg [15:0] x;
  reg [15:0] y;
  reg ci;
  reg sub;
  wire [31:0] z;
  wire [31:0] co;
  FAC uut (
    .x(x), 
    .y(y), 
    .ci(ci), 
    .sub(sub), 
    .z(z), 
    .co(co)
  );

  initial begin
    x = 16'h1234;
    y = 16'h5678;
    ci = 1'b0;
    sub = 1'b0;

    #100;
   
    x = 16'h1111;
    y = 16'h2222;
    ci = 1'b1;
    sub = 1'b1;
    #100;
  end
      
  initial begin
    $monitor("At time %dns, x = %b, y = %b, ci = %b, sub = %b, z = %b, co = %b", $time, x, y, ci, sub, z, co);
  end

endmodule


