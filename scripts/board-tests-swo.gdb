target extended-remote :3333
monitor reset halt
monitor tpiu config internal board-tests-swo.log uart off 16000000 125000
monitor itm port 0 on

load
monitor reset halt

break BoardTests:Done
continue

printf "\nUnity failure count: "
print board_test_result

if board_test_result == 0
	echo BOARD TESTS PASSED\n
else
	echo BOARD TESTS FAILED\n
	quit 1
end
