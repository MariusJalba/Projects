module inv(
input [15:0] a, 
output wire [15:0] y
);
    assign y = ~a;
endmodule