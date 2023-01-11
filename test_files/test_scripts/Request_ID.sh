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

for i in {1..10}; do
    file="test_files/bible.txt"

    # Copy the input file.
    infile="bible.txt"
    cp $file $infile

    # Expected status code.
    expected=200
    # The only thing that is should be printed is the status code.
    printf "GET /$infile HTTP/1.1\r\nRequest-Id: $i\r\n\r\n" > requestfile
    actual=$(cat requestfile | test_files/my_nc.py localhost $port | head -n 1 | awk '{ print $2 }')

    # Check the status code.
    if [[ $actual -ne $expected ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        rm -f $infile
        exit 12
    fi

    # Clean up.
    rm -f $infile
done
expected_log="expected_logs/Request_ID.txt"
# Check the log
diff $expected_log $actual_log
if [[ $? -ne 0 ]]; then
    # Make sure the server is dead.
    kill -9 $pid
    wait $pid
    rm -f $infile $actual_log
    exit 1
fi
# Make sure the server is dead.
kill -9 $pid
wait $pid

# Clean up.
rm -f $infile $actual_log
exit 0
