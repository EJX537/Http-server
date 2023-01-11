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
    # Test input file.
    file="test_files/bible.txt"
    infile="bible.txt"
    outfile="outtemp.txt"

    # Create the input file to overwrite.
    echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" > $infile

    # Expected status code.
    expected=200

    # The only thing that is should be printed is the status code.
    actual=$(curl -s -w "%{http_code}" -o $outfile localhost:$port/$infile -T $file)

    # Check the status code.
    if [[ $actual -ne $expected ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        rm -f $infile $outfile
        exit 1
    fi

    # Check the diff.
    diff $file $infile
    if [[ $? -ne 0 ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        rm -f $infile $outfile
        exit 1
    fi
    # Clean up.
    rm -f $infile $outfile
done

expected_log="expected_logs/put_200.txt"
# Check the log
diff $expected_log $actual_log
if [[ $? -ne 0 ]]; then
    # Make sure the server is dead.
    kill -9 $pid
    wait $pid
    rm -f $infile $outfile
    exit 13
fi
# Make sure the server is dead.
kill -9 $pid
wait $pid

# Clean up.
rm -f $infile $outfile

exit 0
