module cache_controller (
    input wire clk,
    input wire rst,
    input wire [31:0] address,
    input wire [31:0] WriteData,
    input wire read_enable,
    input wire write_enable,
    output reg [31:0] ReadData,
    output reg [31:0] EvictData,
    output wire [3:0] current_state
);
    parameter CacheSize = 32 * 1024;
    parameter BlockSize = 64;
    parameter sets = 128;
    parameter associativity = 4;
    parameter lines = CacheSize / BlockSize - 1; // associativity;
    
    reg [31:0] cache [0:sets-1][0:associativity-1]; // Data field in cache
    reg valid [0:sets-1][0:associativity-1]; // Valid field in cache
    reg dirty [0:sets-1][0:associativity-1]; // Dirty field in cache
    reg [15:0] tag [0:sets-1][0:associativity-1]; // Tag field in cache
    reg [1:0] age [0:sets-1][0:associativity-1]; // Age field in cache
    
    reg [15:0] address_tag;
    reg [6:0] address_index;
    reg [8:0] address_block_offset;
    
    parameter idle = 4'b0000;
    parameter write = 4'b0001;
    parameter read = 4'b0010;
    parameter read_hit = 4'b0011;
    parameter read_miss = 4'b0100;
    parameter write_hit = 4'b0101;
    parameter write_miss = 4'b0110;
    parameter evict = 4'b0111;
    
    reg [3:0] state_next;
    reg [3:0] state_reg;
    
    integer i, j;
    integer oldest_age = 0;
    integer oldest_line = 0;

    task print_cache; //task to print cache with valid,dirty and tag bits for every line
        integer i, j;
        begin
            $display("Cache Contents:");
            $display("Set | Line | Valid | Dirty |  Tag  | Data");
            for (i = 0; i < sets; i = i + 1) begin
                for (j = 0; j < associativity; j = j + 1) begin
                    $display("%3d | %3d  |  %1b    |  %1b   | %4h | %8h", 
                              i, j, valid[i][j], dirty[i][j], tag[i][j], cache[i][j]);
                end
            end
        end
    endtask
    
    initial begin   //this block empties the cache after reset signal is sent
        for (i = 0; i < sets; i = i + 1) begin        
            for (j = 0; j < associativity; j = j + 1) begin
                cache[i][j] = 32'h0;
                valid[i][j] = 1'b0;
                dirty[i][j] = 1'b0;
                tag[i][j] = 16'h0;
                age[i][j] = 2'b00;
            end
        end
    end
    
    always @(address) begin //partitions the address in tag,index and block offset 
        address_tag = address[31:16];
        address_index = address[15:9];
        address_block_offset = address[8:0];
    end
    
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            state_reg <= idle;
        end else begin
            state_reg <= state_next;
        end
    end
    
    always @(posedge clk) begin //logic for changing age for LRU policy
        if (state_reg == read_hit || state_reg == write_hit) begin 
            for (i = 0; i < associativity; i = i + 1) begin  
                if (address_tag == tag[address_index][i] && valid[address_index][i]) begin
                    age[address_index][i] <= 0; // Reset the age of the accessed line
                end else if (valid[address_index][i]) begin
                    age[address_index][i] <= age[address_index][i] + 1;//increasing the age for the line
                end
            end
        end
    end
    
    always @(state_reg or read_enable or write_enable ,write_hit,write_miss,read_hit,read_miss) begin
        state_next = state_reg;
        case (state_reg)
            idle: begin
                if (read_enable) begin
                    state_next = read;
                end else if (write_enable) begin
                    state_next = write;
                end
            end
            read: begin //tries to find if the address is present in cache or not when changes the state correspondingly
                state_next = read_miss; // Default to read_miss
                for (i = 0; i < associativity; i = i + 1) begin
                    if (address_tag == tag[address_index][i] && valid[address_index][i]) begin
                        state_next = read_hit;
                    end
                end
            end
            write: begin //tries to find if the address is present in cache or not when changes the state correspondingly
                state_next = write_miss; // Default to write_miss
                for (i = 0; i < associativity; i = i + 1) begin
                    if (address_tag == tag[address_index][i] && valid[address_index][i]) begin
                        state_next = write_hit;
                    end
                end
            end
            read_hit: begin //if the data was found it is read from the cache
                for (i = 0; i < associativity; i = i + 1) begin
                    if (address_tag == tag[address_index][i] && valid[address_index][i]) begin
                        ReadData <= cache[address_index][i];
                        $display("Read hit: Set %0d, Line %0d, Data: %h", address_index, i, ReadData);
                    end
                end
                state_next = idle;
            end
            write_hit: begin //logic if a write hit occurs which also updates the dirty bit
                for (i = 0; i < associativity; i = i + 1) begin
                    if (address_tag == tag[address_index][i] && valid[address_index][i]) begin
                        cache[address_index][i] <= WriteData;
                        dirty[address_index][i] = 1'b1;
                        $display("Write hit: Set %0d, Line %0d, Data: %h", address_index, i, WriteData);
                    end
                end
                state_next = idle;
            end
            read_miss: begin
                state_next = evict;
            end
            write_miss: begin //logic when a write miss occurs
                for (i = 0; i < associativity; i = i + 1) begin
                    if (!valid[address_index][i]) begin
                        cache[address_index][i] <= WriteData;
                        valid[address_index][i] = 1'b1;
                        tag[address_index][i] = address_tag;
                        dirty[address_index][i] = 1'b0;
                        $display("Write miss: Set %0d, Line %0d, Data: %h", address_index, i, WriteData);
                        state_next = idle;
                        i = associativity; // Exit loop early
                    end
                end
            end
            evict: begin //logic for eviction of data
                oldest_age = 0;
                oldest_line = 0;
                for (i = 0; i < associativity; i = i + 1) begin
                    if (valid[address_index][i] && age[address_index][i] > oldest_age) begin
                        oldest_age = age[address_index][i];
                        oldest_line = i;
                    end
                end
                EvictData <= cache[address_index][oldest_line];
                valid[address_index][oldest_line] = 1'b0;
                age[address_index][oldest_line] = 2'b00;
                dirty[address_index][oldest_line] = 1'b0;
                $display("Evict: Set %0d, Line %0d, EvictData: %h", address_index, oldest_line, EvictData);
                state_next = (write_enable) ? write : read;
            end
        endcase
    end
    
    assign current_state = state_reg;
endmodule

`timescale 1ns / 1ps

module tb_cache_controller;

    // Testbench signals
    reg clk;
    reg rst;
    reg [31:0] address;
    reg [31:0] WriteData;
    reg read_enable;
    reg write_enable;
    wire [31:0] ReadData;
    wire [31:0] EvictData;
    wire [3:0] current_state;

    // Instantiate the cache_controller
    cache_controller uut (
        .clk(clk),
        .rst(rst),
        .address(address),
        .WriteData(WriteData),
        .read_enable(read_enable),
        .write_enable(write_enable),
        .ReadData(ReadData),
        .EvictData(EvictData),
        .current_state(current_state)
    );

    // Clock generation
    always begin
        #5 clk = ~clk;
    end
    // Counters for hits and misses
    integer read_hits = 0;
    integer read_misses = 0;
    integer write_hits = 0;
    integer write_misses = 0;

    // Counters for access time
    integer read_cycles = 0;
    integer write_cycles = 0;
    integer total_read_time = 0;
    integer total_write_time = 0;

    integer total_reads;
    integer total_writes;
    integer total_accesses;

    reg [31:0] read_start_time;
    reg [31:32] write_start_time;

    always @(posedge clk) begin
        if (current_state == uut.read_hit) read_hits = read_hits + 1;
        if (current_state == uut.read_miss) read_misses = read_misses + 1;
        if (current_state == uut.write_hit) write_hits = write_hits + 1;
        if (current_state == uut.write_miss) write_misses = write_misses + 1;
    end

    // Task to print the cache contents
    initial begin
        // Initialize signals
        clk = 0;
        rst = 1;
        address = 0;
        WriteData = 0;
        read_enable = 0;
        write_enable = 0;

        // Reset the design
        #10 rst = 0;

        // Test sequence
        // 1. Write to cache (miss, then hit)
        address = 32'h0000_0000;
        WriteData = 32'hFFFF_FFFF;
        write_enable = 1;
        write_start_time = $time;
        #50 write_enable = 0;
        write_cycles = $time - write_start_time;
        total_write_time = total_write_time + write_cycles;
        $display("Write access time: %0d cycles", write_cycles);
        uut.print_cache();

        // 2. Read from cache (hit)
        address = 32'h0000_0000;
        read_enable = 1;
        read_start_time = $time;
        #50 read_enable = 0;
        read_cycles = $time - read_start_time;
        total_read_time = total_read_time + read_cycles;
        $display("Read access time: %0d cycles", read_cycles);
        $display("ReadData after read hit: %h", ReadData);

        // 3. Write to the same address again (hit)
        address = 32'h0000_0000;
        WriteData = 32'hAAAA_AAAA;
        write_enable = 1;
        write_start_time = $time;
        #50 write_enable = 0;
        write_cycles = $time - write_start_time;
        total_write_time = total_write_time + write_cycles;
        $display("Write access time: %0d cycles", write_cycles);
        uut.print_cache();

        // 4. Read from the same address again (hit)
        address = 32'h0000_0000;
        read_enable = 1;
        read_start_time = $time;
        #50 read_enable = 0;
        read_cycles = $time - read_start_time;
        total_read_time = total_read_time + read_cycles;
        $display("Read access time: %0d cycles", read_cycles);
        $display("ReadData after second read hit: %h", ReadData);

        // 5. Write to a new address (miss, then hit)
        address = 32'h0001_0000;
        WriteData = 32'hBBBB_BBBB;
        write_enable = 1;
        write_start_time = $time;
        #50 write_enable = 0;
        write_cycles = $time - write_start_time;
        total_write_time = total_write_time + write_cycles;
        $display("Write access time: %0d cycles", write_cycles);
        uut.print_cache();

        // 6. Read from the new address (miss, then hit)
        address = 32'h0001_0000;
        read_enable = 1;
        read_start_time = $time;
        #50 read_enable = 0;
        read_cycles = $time - read_start_time;
        total_read_time = total_read_time + read_cycles;
        $display("Read access time: %0d cycles", read_cycles);
        $display("ReadData after read miss: %h", ReadData);

        // 7. Write to another new address (to test eviction)
        address = 32'h0002_0000;
        WriteData = 32'hCCCC_CCCC;
        write_enable = 1;
        write_start_time = $time;
        #50 write_enable = 0;
        write_cycles = $time - write_start_time;
        total_write_time = total_write_time + write_cycles;
        $display("Write access time: %0d cycles", write_cycles);
        // 8. Evict and write to a previously used address
        address = 32'h0003_0000;
        WriteData = 32'hDDDD_DDDD;
        write_enable = 1;
        write_start_time = $time;
        #50 write_enable = 0;
        write_cycles = $time - write_start_time;
        total_write_time = total_write_time + write_cycles;
        $display("Write access time: %0d cycles", write_cycles);

        // 9. Read after evict
        address = 32'h0003_0000;
        read_enable = 1;
        read_start_time = $time;
        #50 read_enable = 0;
        read_cycles = $time - read_start_time;
        total_read_time = total_read_time + read_cycles;
        $display("Read access time: %0d cycles", read_cycles);
        $display("ReadData after eviction and read: %h", ReadData);

	// Allow time for cache to update
        #20;
        $display("Final cache state:");
        uut.print_cache();

        $display("Total Read Access Time: %0d cycles", total_read_time);
        $display("Total Write Access Time: %0d cycles", total_write_time);

	total_reads = read_hits + read_misses;
        total_writes = write_hits + write_misses;
        total_accesses = total_reads + total_writes;
        $display("Read Hits: %d, Read Misses: %d, Write Hits: %d, Write Misses: %d",
                 read_hits, read_misses, write_hits, write_misses);
        $display("Total Accesses: %d", total_accesses);
        // End of test
        #200 $finish;
    end

endmodule


