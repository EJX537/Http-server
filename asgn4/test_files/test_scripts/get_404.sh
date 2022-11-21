#!/usr/bin/env bash

port=$(bash test_files/get_port.sh)
actual_log=temp_log.txt
# Start up server.
./httpserver $port -l $actual_log > /dev/null &
pid=$!

# Wait until we can connect.
while ! nc -zv localhost $port; do
    sleep 0.01
done

for i in {1..5}; do
    # Expected status code.
    expected=404
    # The only thing that is should be printed is the status code.
    printf "GET /DOESNOTEXIST.txt HTTP/1.1\r\n\r\n" > requestfile
    actual=$(cat requestfile | test_files/my_nc.py localhost $port | head -n 1 | awk '{ print $2 }')
    # Check the status code.
    if [[ $actual -ne $expected ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        rm -f $infile $outfile
        exit 1
    fi
    # Clean up.
    rm -f $infile $outfile
done
expected_log="expected_logs/get_404.txt"
# Check the log
diff $expected_log $actual_log
if [[ $? -ne 0 ]]; then
    # Make sure the server is dead.
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 12
fi
# Make sure the server is dead.
kill -9 $pid
wait $pid

# Clean up.
rm -f $infile $outfile

exit 0
