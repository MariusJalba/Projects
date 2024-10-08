project_new example1 -overwrite

set_global_assignment -name FAMILY MAX10
set_global_assignment -name DEVICE 10M50DAF484C7G 

set_global_assignment -name BDF_FILE example1.bdf
set_global_assignment -name VERILOG_FILE coffee_fsm.v
set_global_assignment -name SDC_FILE example1.sdc

set_global_assignment -name TOP_LEVEL_ENTITY coffee_fsm
set_location_assignment -to clk PIN_AH10

set_location_assignment PIN_C10 -to clk   ;# SW[0]
set_location_assignment PIN_C11 -to rst   ;# SW[0]
set_location_assignment PIN_D12 -to credit10   ;# SW[0]
set_location_assignment PIN_C12 -to credit05   ;# SW[0]
set_location_assignment PIN_A12 -to coffee[0]   ;# SW[0]
set_location_assignment PIN_B12 -to coffee[1]   ;# SW[0]
set_location_assignment PIN_A8  -to current_state[0]  ;# LED[0]
set_location_assignment PIN_A9  -to current_state[1]  ;# LED[0]
set_location_assignment PIN_A10  -to current_state[2]  ;# LED[0]
set_location_assignment PIN_B10  -to capp  ;# LED[0]
set_location_assignment PIN_D13  -to expr_l  ;# LED[0]
set_location_assignment PIN_C13  -to exprr  ;# LED[0]

load_package flow
execute_flow -compile

project_close
