module user_input_device_tb ();

reg clk = 1'b1;
reg reset;
reg [3:0] keys;
reg [3:0] switches;

reg  avl_read;
wire avl_irq;
wire avl_readdata;

user_input_device uinput (
    .clk (clk),
    .reset (reset),
    .keys (keys),
    .switches (switches),
    .avl_irq (avl_irq),
    .avl_read (avl_read),
    .avl_readdata (avl_readdata)
);

always #10000 clk = !clk;

initial begin
    keys = 4'b1111;
    switches = 4'b0000;
    avl_read = 1'b0;
    reset = 1'b1;
    #20000 reset = 1'b0;
    assert (!avl_irq);

    switches = 4'b0001;
    #40000 assert (avl_irq);
    #20000 avl_read = 1'b1;
    #20000 assert(!avl_irq);
    avl_read = 1'b0;

    switches = 4'b0000;
    #40000 assert (avl_irq);
    #20000 avl_read = 1'b1;
    #20000 assert(!avl_irq);
    avl_read = 1'b0;

    keys = 4'b1101;
    #40000 assert (avl_irq);
    #20000 avl_read = 1'b1;
    #20000 assert(!avl_irq);
    avl_read = 1'b0;

    keys = 4'b1111;
    #40000 assert (avl_irq);
    #20000 avl_read = 1'b1;
    #20000 assert(!avl_irq);
    avl_read = 1'b0;
end

endmodule
