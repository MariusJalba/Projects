module and16(input [15:0] a, input [15:0] b, output wire [15:0] y);
    assign y = a & b;
endmodule

module and16_tb;
    reg [15:0] a, b;
    wire [15:0] y;

    
    and16 inst (
        .a(a), 
        .b(b), 
        .y(y)
    );

    initial begin
       
        a = 16'h0000;
        b = 16'h0000;

        
        #100;
        
        a = 16'hFFFF;
        b = 16'hAAAA;
        #100;
        
        a = 16'h5555;
        b = 16'hAAAA;
        #100;
        
        $finish;
    end

    initial begin
        $monitor("At time %dns, a = %b, b = %b, y = %b", $time, a, b, y);
    end
endmodule