module xor16(input [15:0] a, input [15:0] b, output wire [15:0] y);
    assign y = a ^ b;
endmodule
module xor16_tb;
    reg [15:0] a, b;
    wire [15:0] y;

    // Instantiate the Unit Under Test (UUT)
    xor16 inst (
        .a(a), 
        .b(b), 
        .y(y)
    );

    initial begin
        // Initialize Inputs
        a = 16'h0000;
        b = 16'h0000;

        // Wait 100 ns for global reset to finish
        #100;
        
        // Change input values
        a = 16'hFFFF;
        b = 16'hAAAA;
        #100;
        
        a = 16'h5555;
        b = 16'hAAAA;
        #100;
        
        // Add more test vectors as needed

        // Finish the simulation
        $finish;
    end

    initial begin
        $monitor("At time %dns, a = %b, b = %b, y = %b", $time, a, b, y);
    end
endmodule
