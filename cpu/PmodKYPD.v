/******************************************************************************
*
*   Digilent PmodKYPD keypad controller
*
*   Description:
*       Columns are driven, rows have pulldowns and are sensed.
*
*   Key Codes:
*       10 -- '0'
*       11 -- '1'
*       12 -- '2'
*       13 -- '3'
*       14 -- '4'       
*       15 -- '5'
*       16 -- '6'
*       17 -- '7'
*       18 -- '8'
*       19 -- '9'
*       1A -- 'A'
*       1B -- 'B'
*       1C -- 'C'
*       1D -- 'D'
*       1E -- 'E'
*       1F -- 'F
*       00 -- <none>
*       
******************************************************************************/

module keypad (
    input CLK,                  // 50MHz clock
    input [3:0] ROWS,           // rows
    output reg [3:0] COLS,      // cols
    output reg [4:0] KEYCODE    // returned key code
);

    // scale the input clock to ~96Hz
    wire CLK_96Hz;
    prescaler #(.N(19)) ps2(CLK, CLK_96Hz);
    
    reg [2:0] state = 0;
    reg [4:0] localKeycode = 0;
    
    initial KEYCODE <= 0;
        
    always @(posedge CLK_96Hz) 
    begin
        case (state)
            0:
            begin
                COLS <= 4'b0111;
                case (ROWS)
                    4'b0111: localKeycode <= 5'h1A;  // col 1 row 1
                    4'b1011: localKeycode <= 5'h1B;  // col 1 row 2
                    4'b1101: localKeycode <= 5'h1C;  // col 1 row 3
                    4'b1110: localKeycode <= 5'h1D;  // col 1 row 4
                    //default:
                endcase
            end
            1:
            begin
                COLS <= 4'b1011;
                case (ROWS)
                    4'b0111: localKeycode <= 5'h11;  // col 2 row 1
                    4'b1011: localKeycode <= 5'h14;  // col 2 row 2
                    4'b1101: localKeycode <= 5'h17;  // col 2 row 3
                    4'b1110: localKeycode <= 5'h10;  // col 2 row 4
                    //default:
                endcase
            end
            2:
            begin
                COLS <= 4'b1101;
                case (ROWS)
                    4'b0111: localKeycode <= 5'h12;  // col 3 row 1
                    4'b1011: localKeycode <= 5'h15;  // col 3 row 2
                    4'b1101: localKeycode <= 5'h18;  // col 3 row 3
                    4'b1110: localKeycode <= 5'h1F;  // col 3 row 4
                    //default:
                endcase
            end
            3:
            begin
                COLS <= 4'b1110;
                case (ROWS)
                    4'b0111: localKeycode <= 5'h13;  // col 4 row 1
                    4'b1011: localKeycode <= 5'h16;  // col 4 row 2
                    4'b1101: localKeycode <= 5'h19;  // col 4 row 3
                    4'b1110: localKeycode <= 5'h1E;  // col 4 row 4
                    //default:
                endcase
            end
            4:
            begin
                KEYCODE <= localKeycode;
            end
            default:
            begin
                localKeycode <= 0;
            end
        endcase
        state = state + 1;
    end
    
endmodule
