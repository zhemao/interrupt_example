module user_input_device_tb ();

reg clk = 1'b1;
reg reset;
reg [3:0] keys;
reg [3:0] switches;

wire avl_irq_n;
wire avl_readdata;

user_input_device uinput (
    .clk (clk),
    .reset (reset),
    .keys (keys),
    .switches (switches),
    .avl_irq_n (avl_irq_n),
    .avl_readdata (avl_readdata)
);

always #10000 clk = !clk;

initial begin
    keys = 4'b1111;
    switches = 4'b0000;
    reset = 1'b1;
    #20000 reset = 1'b0;
    assert (avl_irq_n);

    switches = 4'b0001;
    #20000 assert (!avl_irq_n);
    #20000 assert (avl_irq_n);
    switches = 4'b0000;
    #20000 assert (!avl_irq_n);
    #20000 assert (avl_irq_n);

    keys = 4'b1101;
    #20000 assert (!avl_irq_n);
    #20000 assert (avl_irq_n);
    keys = 4'b1111;
    #20000 assert (!avl_irq_n);
    #20000 assert (avl_irq_n);
end

endmodule
