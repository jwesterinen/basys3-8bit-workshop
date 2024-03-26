`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/16/2022 06:08:14 PM
// Design Name: 
// Module Name: ps2_keyboard
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module keyboard(
    input clk,
    input dataIn,
    output [7:0] dataOut
    );
    
    // actual keyboard buffer
    reg [7:0] buffer;
    
    reg [7:0] data_curr;
    reg [7:0] data_pre;
    reg [3:0] b;
    reg flag;
    
    initial begin
        b <= 4'h1;
        flag <= 1'b0;
        data_curr <= 8'hf0;
        data_pre <= 8'hf0;
        //buffer <= 16'hfff0;
        buffer <= 8'hf0;
    end
    
    // activating at negative edge of clock from keyboard
    always @(negedge clk) begin
        case(b)
            1:; //first bit
            2:  data_curr[0]<=dataIn;
            3:  data_curr[1]<=dataIn;
            4:  data_curr[2]<=dataIn;
            5:  data_curr[3]<=dataIn;
            6:  data_curr[4]<=dataIn;
            7:  data_curr[5]<=dataIn;
            8:  data_curr[6]<=dataIn;
            9:  data_curr[7]<=dataIn;
            10: flag<=1'b1; //Parity bit
            11: flag<=1'b0; //Ending bit
        endcase
        if(b<=10)
            b<=b+1;
        else if(b==11)
            b<=1;
    end
    
    // put obtained data in keyboard buffer
    always@(posedge flag) begin
        if(data_curr == 8'hf0)
            buffer <= data_pre;
        else
            data_pre<=data_curr;
    end
    
    assign dataOut = buffer;
    
endmodule
