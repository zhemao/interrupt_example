module user_input_device (
    input clk,
    input reset,
    input [3:0] keys,
    input [3:0] switches,

    output avl_irq,
    input  avl_read,
    output [7:0] avl_readdata
);

reg [7:0] cur_inputs;
reg [7:0] last_inputs;
wire [7:0] changed_inputs = cur_inputs ^ last_inputs;

reg irq;

assign avl_irq = irq;
assign avl_readdata = last_inputs;

always @(posedge clk) begin
    if (reset) begin
        cur_inputs <= 8'd0;
        last_inputs <= 8'd0;
        irq <= 1'b0;
    end else begin
        cur_inputs <= {keys, switches};
        last_inputs <= cur_inputs;
        if (changed_inputs != 8'd0)
            irq <= 1'b1;
        else if (avl_read)
            irq <= 1'b0;
    end
end

endmodule
