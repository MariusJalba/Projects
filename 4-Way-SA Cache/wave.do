onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /tb_cache_controller/clk
add wave -noupdate /tb_cache_controller/rst
add wave -noupdate /tb_cache_controller/address
add wave -noupdate /tb_cache_controller/WriteData
add wave -noupdate /tb_cache_controller/read_enable
add wave -noupdate /tb_cache_controller/write_enable
add wave -noupdate /tb_cache_controller/ReadData
add wave -noupdate /tb_cache_controller/EvictData
add wave -noupdate /tb_cache_controller/current_state
add wave -noupdate /tb_cache_controller/read_hits
add wave -noupdate /tb_cache_controller/read_misses
add wave -noupdate /tb_cache_controller/write_hits
add wave -noupdate /tb_cache_controller/write_misses
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {0 ps} 0}
quietly wave cursor active 0
configure wave -namecolwidth 43
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {0 ps} {1103 ps}
