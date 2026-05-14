#!/bin/bash

GAME="../build/game"
PASSED=0
FAILED=0

for in_file in *.in; do
    name="${in_file%.in}"
    
    $GAME "$in_file" > /dev/null 2>&1
    sed -i 's/\r$//' result.txt

    if diff result.txt "$name.expected" > /dev/null 2>&1; then
        echo "OK: $name"
        PASSED=$((PASSED + 1))
    else
        echo "FAIL: $name"
        echo ""
        echo "  EXPECTED:"
        cat "$name.expected"
        echo ""
        echo "  GOT:"
        cat result.txt
        FAILED=$((FAILED + 1))
    fi

    echo ""
    echo ""
    rm -f result.txt
done

echo "Passed: $PASSED  Failed: $FAILED"