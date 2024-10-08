`define INIT 3'b000
`define C05 3'b001
`define C10 3'b010
`define C15 3'b011
`define C20 3'b100
`define Expr 3'b101
`define ExpL 3'b110
`define Cap 3'b111

module coffee_fsm(
	input clk,
	input rst,
	input credit05,
	input credit10,
	input [1:0] coffee,
	output [2:0] current_state,
	output reg exprr,
	output reg expr_l,
	output reg capp
	);

	reg [2:0] state_nxt, state_reg; //starea următoare a 		registrului de stare, respectiv starea curentă
	//segmentul secvential– memorează starea curentă

always @( posedge clk or posedge rst)
	begin
	//dacă resetul este activ (0 logic)
	if (rst)
		state_reg <= 0; //q_reg denotă starea curentă
	else
	state_reg<= state_nxt;
end

//segmentul combinational – pregătește starea următoare, care va fi încărcată
//în registru de stare

always @( state_reg, coffee, credit05, credit10) //lista de senzitivități conține toate semnalele a căror modificare
	begin //produce re-evaluarea codului dintre begin ... end
		state_nxt = state_reg;
		exprr = 0;
		expr_l = 0;
		capp = 0;
//descrierea automatului
		case (state_reg)
			`INIT : begin
				state_nxt = `INIT;
				exprr = 0;
				expr_l = 0;
				capp = 0;
				if(credit05)
					state_nxt = `C05;
				if(credit10)
					state_nxt = `C10;
			end
			`C05 : begin
				exprr = 0;
				expr_l = 0;
				capp = 0;
				if(credit05)
					state_nxt = `C10;
				if(credit10)
					state_nxt = `C15;
			end
			`C10 : begin
				exprr = 0;
				expr_l = 0;
				capp = 0;
				if(credit05)
					state_nxt = `C15;
				if(credit10)
					state_nxt = `C20;
			end
			`C15 : begin
				exprr = 0;
				expr_l = 0;
				capp = 0;
				state_nxt = `C20;
			end
			`C20 : begin
				exprr = 0;
				expr_l = 0;
				capp = 0;
				if(coffee == 1)
					state_nxt = `Expr;
				if(coffee == 2)
					state_nxt = `ExpL;
				if(coffee == 3)
					state_nxt = `Cap;
			end
			`Expr : begin
				exprr = 1;
				expr_l = 0;
				capp = 0;
				state_nxt = `INIT;
			end
			`ExpL : begin
				exprr = 0;
				expr_l = 1;
				capp = 0;
				state_nxt = `INIT;
			end
			`Cap : begin
				exprr = 0;
				expr_l = 0;
				capp = 1;
				state_nxt = `INIT;
			end
		endcase	
	end
	assign current_state = state_reg;
endmodule
