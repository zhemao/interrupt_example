module sockit_top (
    input  OSC_50_B8A,

    input  [3:0] KEY,
    input  [3:0] SW,
    output [3:0] LED
);

assign LED = 4'b0101;

endmodule
