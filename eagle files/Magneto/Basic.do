bestsave on
rule pcb (width 0.800000)
rule pcb (clearance 0.600000)
rule pcb (clearance 0.203200 (type via_via))
rule pcb (clearance 0.203200 (type via_pin))
rule pcb (clearance 1.016000 (type via_area))
rule pcb (clearance 0.203200 (type wire_via))
rule pcb (clearance 0.203200 (type wire_pin))
rule pcb (clearance 1.016000 (type wire_area))
bus diagonal
fanout 5
route 20
clean 2
route 25 16
clean 2
filter 5
recorner diagonal
status_file
